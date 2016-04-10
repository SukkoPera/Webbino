/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012-2015 by SukkoPera                                  *
 *                                                                         *
 *   SmartStrip is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   SmartStrip is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with SmartStrip.  If not, see <http://www.gnu.org/licenses/>.   *
 ***************************************************************************/

#include <Webbino.h>
#include <avr/pgmspace.h>

// Instantiate the WebServer
WebServer webserver;

//~ #define USE_ENC28J60
//~ #include <WebServer_ENC28J60.h>
//~ NetworkInterfaceENC28J60 netint;

#define USE_ESP8266
#include <WebServer_ESP8266.h>

#include <SoftwareSerial.h>
SoftwareSerial swSerial (7, 8);

// Wi-Fi parameters
#define WIFI_SSID        "ssid"
#define WIFI_PASSWORD    "password"

NetworkInterfaceESP8266 netint;


/******************************************************************************
 * DEFINITION OF PAGES                                                        *
 ******************************************************************************/

#include "html.h"

static const Page indexPage PROGMEM = {index_html_name, index_html, NULL};

static const Page * const pages[] PROGMEM = {
	&indexPage,
 	NULL
};


/******************************************************************************
 * DEFINITION OF TAGS                                                         *
 ******************************************************************************/

#define REP_BUFFER_LEN 32

static char replaceBuffer[REP_BUFFER_LEN];

static char *ip2str (const byte *buf) {
	itoa (buf[0], replaceBuffer, DEC);
	strcat_P (replaceBuffer, PSTR ("."));
	itoa (buf[1], replaceBuffer + strlen (replaceBuffer), DEC);
	strcat_P (replaceBuffer, PSTR ("."));
	itoa (buf[2], replaceBuffer + strlen (replaceBuffer), DEC);
	strcat_P (replaceBuffer, PSTR ("."));
	itoa (buf[3], replaceBuffer + strlen (replaceBuffer), DEC);

	return replaceBuffer;
}

static char *evaluate_ip (void *data __attribute__ ((unused))) {
 	return ip2str (netint.getIP ());
}

static char *evaluate_netmask (void *data __attribute__ ((unused))) {
	return ip2str (netint.getNetmask ());
}

static char *evaluate_gw (void *data __attribute__ ((unused))) {
	return ip2str (netint.getGateway ());
}

const char COLON_STRING[] PROGMEM = ":";

static char *evaluate_mac_addr (void *data __attribute__ ((unused))) {
	const byte *buf = netint.getMAC ();

	replaceBuffer[0] = '\0';

	for (byte i = 0; i < 5; i++) {
		if (buf[i] < 16)
			strcat_P (replaceBuffer, PSTR ("0"));
		itoa (buf[i], replaceBuffer + strlen (replaceBuffer), HEX);
		strcat_P (replaceBuffer, COLON_STRING);
	}
	if (buf[5] < 16)
			strcat_P (replaceBuffer, PSTR ("0"));
	itoa (buf[5], replaceBuffer + strlen (replaceBuffer), HEX);

	return replaceBuffer;
}

static char *evaluate_ip_src (void *data __attribute__ ((unused))) {
	if (netint.usingDHCP ())
		strlcpy_P (replaceBuffer, PSTR ("DHCP"), REP_BUFFER_LEN);
	else
		strlcpy_P (replaceBuffer, PSTR ("MANUAL"), REP_BUFFER_LEN);

	return replaceBuffer;
}

static char *evaluate_webbino_version (void *data __attribute__ ((unused))) {
	strlcpy (replaceBuffer, WEBBINO_VERSION, REP_BUFFER_LEN);

	return replaceBuffer;
}

static char *evaluate_uptime (void *data __attribute__ ((unused))) {
	unsigned long uptime = millis () / 1000;
	byte d, h, m, s;

	d = uptime / 86400;
	uptime %= 86400;
	h = uptime / 3600;
	uptime %= 3600;
	m = uptime / 60;
	uptime %= 60;
	s = uptime;

	replaceBuffer[0] = '\0';

	if (d > 0) {
		itoa (d, replaceBuffer, DEC);
		strcat_P (replaceBuffer, d == 1 ? PSTR (" day, ") : PSTR (" days, "));
	}

	// Shorter format: "2 days, 4:12:22", saves 70 bytes and doesn't overflow :D
	if (h < 10)
		strcat_P (replaceBuffer, PSTR ("0"));
	itoa (h, replaceBuffer + strlen (replaceBuffer), DEC);
	strcat_P (replaceBuffer, PSTR (":"));
	if (m < 10)
		strcat_P (replaceBuffer, PSTR ("0"));
	itoa (m, replaceBuffer + strlen (replaceBuffer), DEC);
	strcat_P (replaceBuffer, PSTR (":"));
	if (s < 10)
		strcat_P (replaceBuffer, PSTR ("0"));
	itoa (s, replaceBuffer + strlen (replaceBuffer), DEC);

	return replaceBuffer;
}

static char *evaluate_free_ram (void *data __attribute__ ((unused))) {
	extern int __heap_start, *__brkval;
	int v;

	itoa ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval), replaceBuffer, DEC);

	return replaceBuffer;
}


// Max length of these is MAX_TAG_LEN (24)
static const char subMacAddrStr[] PROGMEM = "NET_MAC";
static const char subIPAddressStr[] PROGMEM = "NET_IP";
static const char subNetmaskStr[] PROGMEM = "NET_MASK";
static const char subGatewayStr[] PROGMEM = "NET_GW";
static const char subNetConfSrcStr[] PROGMEM = "NET_CONF_SRC";
static const char subWebbinoVerStr[] PROGMEM = "WEBBINO_VER";
static const char subUptimeStr[] PROGMEM = "UPTIME";
static const char subFreeRAMStr[] PROGMEM = "FREERAM";

static const var_substitution subMacAddrVarSub PROGMEM = {subMacAddrStr, evaluate_mac_addr, NULL};
static const var_substitution subIPAddressVarSub PROGMEM = {subIPAddressStr, evaluate_ip, NULL};
static const var_substitution subNetmaskVarSub PROGMEM = {subNetmaskStr, evaluate_netmask, NULL};
static const var_substitution subGatewayVarSub PROGMEM = {subGatewayStr, evaluate_gw, NULL};
static const var_substitution subNetConfSrcVarSub PROGMEM = {subNetConfSrcStr, evaluate_ip_src, NULL};
static const var_substitution subWebbinoVerVarSub PROGMEM = {subWebbinoVerStr, evaluate_webbino_version, NULL};
static const var_substitution subUptimeVarSub PROGMEM = {subUptimeStr, evaluate_uptime, NULL};
static const var_substitution subFreeRAMVarSub PROGMEM = {subFreeRAMStr, evaluate_free_ram, NULL};

static const var_substitution * const substitutions[] PROGMEM = {
	&subMacAddrVarSub,
	&subIPAddressVarSub,
	&subNetmaskVarSub,
	&subGatewayVarSub,
	&subNetConfSrcVarSub,
	&subWebbinoVerVarSub,
	&subUptimeVarSub,
	&subFreeRAMVarSub,
	NULL
};


/******************************************************************************
 * MAIN STUFF                                                                 *
 ******************************************************************************/

void setup () {
	Serial.begin (9600);
	Serial.println (F("Webbino " WEBBINO_VERSION));

#if defined (USE_ENC28J60)
	byte mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	netint.begin (mac);
#elif defined (USE_ESP8266)
	swSerial.begin (9600);
	netint.begin (swSerial, WIFI_SSID, WIFI_PASSWORD);
#endif

	webserver.setPages (pages);
	webserver.setSubstitutions (substitutions);

	Serial.println (F("Trying to get an IP address through DHCP"));
	if (!webserver.begin (netint)) {
		Serial.println (F("Failed to get configuration from DHCP"));
		while (42)
			;
	} else {
		Serial.println (F("DHCP configuration done"));
#if 0
		ether.printIp ("IP:\t", webserver.getIP ());
		ether.printIp ("Mask:\t", webserver.getNetmask ());
		ether.printIp ("GW:\t", webserver.getGateway ());
		Serial.println ();
#endif
	}
}

void loop () {
	webserver.loop ();
}
