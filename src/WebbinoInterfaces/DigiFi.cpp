/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2021 by SukkoPera                                  *
 *                                                                         *
 *   Webbino is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Webbino is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Webbino. If not, see <http://www.gnu.org/licenses/>.       *
 ***************************************************************************/

#include "DigiFi.h"

#ifdef WEBBINO_USE_DIGIFI

#include "webbino_debug.h"


WebClientDigiFi::WebClientDigiFi (DigiFi& _wifi): wifi (_wifi) {
}

void WebClientDigiFi::begin (char* req) {
	request.parse (req);

	bufUsed = 0;
	headerLen = 0;
}

size_t WebClientDigiFi::write (uint8_t c) {
	size_t ret;

	if (bufUsed < BUFFER_SIZE) {
		if (headerLen == 0 && bufUsed >= 3 &&
		  buf[bufUsed - 3] == '\r' && buf[bufUsed - 2] == '\n' &&
		  buf[bufUsed - 1] == '\r' && c == '\n') {

			/* Manipulate headers a bit for later, i.e. replace \r (last char
			 * added to buffer) with \0 and don't insert \n (current char)
			 */
			buf[bufUsed - 1] = '\0';
			headerLen = bufUsed;	// Remember header len
		} else {
			buf[bufUsed++] = c;
		}

		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

void WebClientDigiFi::sendReply () {
	//~ DPRINTLN (F("HEADERS:"));
	//~ DPRINT ((char*) buf);

	//~ DPRINTLN (F("BODY:"));
	//~ DPRINTLN ((char*) buf + headerLen);

	// Send headers
	wifi.write (buf, headerLen - 1);

	// Add content length header
	wifi.print (F("Content-Length: "));
	wifi.print (bufUsed - headerLen);
	wifi.print (F("\r\n"));

	// Add connection close header
	wifi.print (F("Connection: close\r\n"));

	// Send end of headers
	wifi.print (F("\r\n"));

	// Send body
	wifi.write (buf + headerLen, bufUsed - headerLen);

	// No way to close connection with this chip :(
}

/****************************************************************************/

NetworkInterfaceDigiFi::NetworkInterfaceDigiFi (): webClient (wifi) {
}

boolean NetworkInterfaceDigiFi::begin () {
	boolean ret = false;

	DPRINTLN (F("Using DigiFi library"));

	if (SERVER_PORT == 80) {
		/* Port 80 is used by the configuration web interface, so it can't be
		 * used for our server
		 */
		DPRINTLN (F("Port 80 is not supported on this chip"));
	} else {
		/* Note that speed here must match the speed set under "Other Setting"
		 * -> "Serial Port Parameters Setting" in the wifi module web
		 * interface. If no speed is specified, it defaults to 9600 bps, but
		 * note that the module defaults to 115200!
		 */
		wifi.begin (115200);

		// Attempt to connect to WiFi network
		DPRINTLN (F("Connecting to network"));
		while (!wifi.ready ()) {
			DPRINTLN (F("Failed, retrying in 5 seconds"));
			delay (5000);
		}

		// Sets up server and returns IP
		/* String address = */ wifi.server (SERVER_PORT);

		DPRINT (F("Joined AP, local IP address: "));
		DPRINTLN (wifi.localIP ());

		/* This is the ONLY way we can close the TCP connection after we have
		 * replied. See http://digistump.com/board/index.php/topic,1270.0.html
		 * for details.
		 *
		 * Note that this must allow time for all tags to be replaced, which
		 * might take a bit, since some tags might need to talk to the wifi
		 * module to retrieve IP settings, etc.
		 */
		wifi.setTCPTimeout (15);

		ret = true;
	}

	return ret;
}

boolean lineIsInteresting (const char *line) {
	return strncmp_P (line, PSTR ("Authorization: Basic "), 21) == 0;
}

enum RequestState {
	RS_URI,
	RS_HEADERS,
	RS_BODY,
	RS_COMPLETE
};

/* The DigiFi library is total crap, so we have to go low-level. Of course it
 * would be much better to rewrite said library from scratch, but it doesn't
 * really look it is worth the effort, so let's make do with this.
 */
WebClient* NetworkInterfaceDigiFi::processPacket () {
	WebClient *ret = NULL;

	if (wifi.available ()) {
		DPRINTLN (F("New client"));

#ifdef CLIENT_TIMEOUT
		lastPacketReceived = millis ();
#endif

		// An http request ends with a blank line
		boolean currentLineIsBlank = true;
		ethernetBufferSize = 0;
		RequestState state = RS_URI;
		unsigned int lastLineStart = 0;
		while (!ret && state != RS_COMPLETE) {
#ifdef CLIENT_TIMEOUT
		    // Check for connection timeout
		    if (millis () - lastPacketReceived > CLIENT_TIMEOUT) {
		        DPRINTLN (F("Client connection timeout"));
		        break;
		    }
#endif

			if (wifi.available ()) {
				char c = wifi.read ();

#ifdef CLIENT_TIMEOUT
				// Something was received, reset timeout
				lastPacketReceived = millis();
#endif

				/* We are only interested in the first line of the HTTP request
				 * (i.e.: the one that contains the method and the URI), so if
				 * we haven't seen an LF yet, let's append to our buffer
				 */
				// Reserve one place for the terminator we'll append later
				if (ethernetBufferSize < sizeof (ethernetBuffer) - 1) {
					ethernetBuffer[ethernetBufferSize++] = c;
				} else {
					// No more space in buffer, ignore
					DPRINTLN (F("Ethernet buffer overflow"));
				}

				if (c == '\n') {		// End of a line
					switch (state) {
						case RS_URI:
							/* Great, we have extracted the request line! The
							 * following lines will be header lines.
							 */
							state = RS_HEADERS;
							lastLineStart = ethernetBufferSize;
							break;;
						case RS_HEADERS:
							if (currentLineIsBlank) {
								/* We got to the end of the line and the line is blank,
								 * this means the http request has ended
								 */
								 state = RS_BODY;
							} else {
								/* Got a header line, see if it's an interesting one
								 * or discard it
								 */
								const char *line = reinterpret_cast<char *> (ethernetBuffer + lastLineStart);
								ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate
								if (lineIsInteresting (line)) {
									lastLineStart = ethernetBufferSize;
								} else {
									DPRINT (F("Discarding header line: "));
									DPRINTLN (line);
									ethernetBufferSize = lastLineStart;
								}
							}
							break;
						case RS_BODY:
							// Just keep it
							lastLineStart = ethernetBufferSize;

							// Request complete, get out
							ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate

							state = RS_COMPLETE;
							break;
						default:
							break;
					}

					// A new line is starting
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// Got a character on the current line
					currentLineIsBlank = false;
				}
			} else if (state == RS_BODY) {
				DPRINTLN (F("COMPLETE"));
				ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate
				state = RS_COMPLETE;
			}
		}

		// FIXME: Make const
		if (state == RS_COMPLETE && ethernetBufferSize > 0) {
			// Got a request to parse
			char *req = reinterpret_cast<char *> (ethernetBuffer);
			webClient.begin (req);
			ret = &webClient;
		} else {
			// Close the connection
			//~ client.stop ();
			DPRINTLN (F("Client disconnected"));
		}
	}

	return ret;
}

/* FIXME All of the following methods are very slow, since they need to talk to
 * the module. Maybe we'd better cache the results in begin().
 */
boolean NetworkInterfaceDigiFi::usingDHCP () {
	wifi.startATMode ();
	String str = wifi.getSTANetwork ();
	wifi.endATMode ();

	//~ DPRINT ("getSTANetwork = ");
	//~ DPRINT (str);

	// Response is like "+ok=DHCP,10.0.0.152,255.255.255.0,10.0.0.254"
	return str.startsWith ("+ok") && str.length () > 4 && str.substring (4).startsWith ("DHCP");
}

byte *NetworkInterfaceDigiFi::getMAC () {
	wifi.startATMode ();
	String str = wifi.getSTAMac ();
	wifi.endATMode ();

	//~ DPRINT ("getSTAMac = ");
	//~ DPRINTLN (str);

	// Response is like "+ok=ACCF223657B4" (+\r\n, I guess)
	if (str.startsWith ("+ok") && str.length () >= 16) {
		for (byte b = 0; b < 6; b++) {
			String oct = str.substring (b * 2 + 4, b * 2 + 4 + 2);
			macAddress[b] = (byte) strtol (oct.c_str (), NULL, 16);
		}
	}

	return macAddress;
}

IPAddress NetworkInterfaceDigiFi::getIP () {
	return wifi.localIP ();
}

IPAddress NetworkInterfaceDigiFi::getNetmask () {
	return wifi.subnetMask ();
}

IPAddress NetworkInterfaceDigiFi::getGateway () {
	return wifi.gatewayIP ();
}

IPAddress NetworkInterfaceDigiFi::getDns () {
	return wifi.dnsServerIP ();
}

#endif
