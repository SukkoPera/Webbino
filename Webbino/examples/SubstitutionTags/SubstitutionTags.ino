/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012 by SukkoPera                                       *
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

 #define USE_ENC28J60

/* See http://provideyourown.com/2011/advanced-arduino-including-multiple-libraries/
 * to understand why this is unfortunately necessary.
 */
#ifdef USE_ENC28J60
	#include <EtherCard.h>
#else
	#include <SPI.h>
	#include <Ethernet.h>
#endif

#include <avr/pgmspace.h>
#include <Webbino.h>
#include "html.h"

// Instantiate the WebServer
WebServer webserver;


/******************************************************************************
 * DEFINITION OF PAGES                                                        *
 ******************************************************************************/

static Page indexPage PROGMEM = {index_html_name, index_html, NULL};

static Page *pages[] PROGMEM = {
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

static char *evaluate_ip () {
 	return ip2str (webserver.getIP ());
}

static char *evaluate_netmask () {
	return ip2str (webserver.getNetmask ());
}

static char *evaluate_gw () {
	return ip2str (webserver.getGateway ());
}

// static char int2hex (int i) {
	// if (i < 10)
		// return i + '0';
	// else
		// return i - 10 + 'A';
// }

const char COLON_STR[] PROGMEM = ":";
	
static char *evaluate_mac_addr () {
#if 0
	byte tmp;

	tmp = EEPROM.read (EEPROM_MAC_B1_ADDR);
	replaceBuffer[0] = int2hex (tmp / 16);
	replaceBuffer[1] = int2hex (tmp % 16);

	replaceBuffer[2] = ':';

	tmp = EEPROM.read (EEPROM_MAC_B2_ADDR);
	replaceBuffer[3] = int2hex (tmp / 16);
	replaceBuffer[4] = int2hex (tmp % 16);

	replaceBuffer[5] = ':';

	tmp = EEPROM.read (EEPROM_MAC_B3_ADDR);
	replaceBuffer[6] = int2hex (tmp / 16);
	replaceBuffer[7] = int2hex (tmp % 16);

	replaceBuffer[8] = ':';

	tmp = EEPROM.read (EEPROM_MAC_B4_ADDR);
	replaceBuffer[9] = int2hex (tmp / 16);
	replaceBuffer[10] = int2hex (tmp % 16);

	replaceBuffer[11] = ':';

	tmp = EEPROM.read (EEPROM_MAC_B5_ADDR);
	replaceBuffer[12] = int2hex (tmp / 16);
	replaceBuffer[13] = int2hex (tmp % 16);

	replaceBuffer[14] = ':';

	tmp = EEPROM.read (EEPROM_MAC_B6_ADDR);
	replaceBuffer[15] = int2hex (tmp / 16);
	replaceBuffer[16] = int2hex (tmp % 16);

	replaceBuffer[17] = '\0';
#endif

	const byte *buf = webserver.getIP ();
	
	itoa (buf[0], replaceBuffer, HEX);
	strcat_P (replaceBuffer, COLON_STR);
	itoa (buf[1], replaceBuffer + strlen (replaceBuffer), HEX);
	strcat_P (replaceBuffer, COLON_STR);
	itoa (buf[2], replaceBuffer + strlen (replaceBuffer), HEX);
	strcat_P (replaceBuffer, COLON_STR);
	itoa (buf[3], replaceBuffer + strlen (replaceBuffer), HEX);
	strcat_P (replaceBuffer, COLON_STR);
	itoa (buf[4], replaceBuffer + strlen (replaceBuffer), HEX);
	strcat_P (replaceBuffer, COLON_STR);
	itoa (buf[5], replaceBuffer + strlen (replaceBuffer), HEX);

	return replaceBuffer;
}


const char CHECKED_STRING[] PROGMEM = "checked";
const char SELECTED_STRING[] PROGMEM = "selected=\"true\"";

static char *evaluate_ip_src () {
	if (webserver.usingDHCP)
		strlcpy_P (replaceBuffer, PSTR ("DHCP"), REP_BUFFER_LEN);
	else
		strlcpy_P (replaceBuffer, PSTR ("MANUAL"), REP_BUFFER_LEN);
		
	return replaceBuffer;	
}

static char *evaluate_webbino_version () {
	strlcpy (replaceBuffer, WEBBINO_VERSION, REP_BUFFER_LEN);
	
	return replaceBuffer;
}

// Wahahahah! Prolly the most advanced function of its kind!
// FIXME: Save some bytes removing temp vars.
// FIXME: Check that string does not overflow buffer (which is likely!)
static char *evaluate_uptime () {
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

	if (h > 0) {
		itoa (h, replaceBuffer + strlen (replaceBuffer), DEC);
		strcat_P (replaceBuffer, h == 1 ? PSTR (" hour, ") : PSTR (" hours, "));
	}

	if (m > 0) {
		itoa (m, replaceBuffer + strlen (replaceBuffer), DEC);
		strcat_P (replaceBuffer, m == 1 ? PSTR (" minute, ") : PSTR (" minutes, "));
	}

	// We always have seconds Maybe we could avoid them if h > 1 or so.
	itoa (s, replaceBuffer + strlen (replaceBuffer), DEC);
	strcat_P (replaceBuffer, s == 1 ? PSTR (" second") : PSTR (" seconds"));
	
	return replaceBuffer;
}

static char *evaluate_free_ram () {
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

static var_substitution subMacAddrVarSub PROGMEM = {subMacAddrStr, evaluate_mac_addr};
static var_substitution subIPAddressVarSub PROGMEM = {subIPAddressStr, evaluate_ip};
static var_substitution subNetmaskVarSub PROGMEM = {subNetmaskStr, evaluate_netmask};
static var_substitution subGatewayVarSub PROGMEM = {subGatewayStr, evaluate_gw};
static var_substitution subNetConfSrcVarSub PROGMEM = {subNetConfSrcStr, evaluate_ip_src};
static var_substitution subWebbinoVerVarSub PROGMEM = {subWebbinoVerStr, evaluate_webbino_version};
static var_substitution subUptimeVarSub PROGMEM = {subUptimeStr, evaluate_uptime};
static var_substitution subFreeRAMVarSub PROGMEM = {subFreeRAMStr, evaluate_free_ram};
	
static var_substitution *substitutions[] PROGMEM = {
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
	byte mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

	Serial.begin (9600);
	Serial.println (F("Webbino " WEBBINO_VERSION));

	webserver.setPages (pages);
	webserver.setSubstitutions (substitutions);

	Serial.println (F("Trying to get an IP address through DHCP"));
	if (!webserver.begin (mac)) {
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
	webserver.processPacket ();
}
