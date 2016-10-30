Webbino
=======
Webbino is a webserver for Arduino.  I originally wrote it for my [SmartStrip
project](https://github.com/SukkoPera/SmartStrip), which required several unique
features which were not present in other webservers that were available at the
time:
- Compatible with many network interfaces:
  - WizNet W5100: Official Ethernet Shield
  - WizNet W5500: Official Ethernet Shield 2 by arduino.org
  - ENC28J60: KMTronic's DINo
  - ESP8266 (Wi-Fi)
  - WINC1500 (Wi-Fi): Official WiFi Shield 101, MKR1000
- HTML pages and other data can be embedded in the code. This avoids the need of
  an SD card for simple sites.
- Otherwise, HTML pages can also be stored on an SD card (long file names are
  supported!)
- Tags like "#WHATEVER#" in HTML pages can be replaced at page request time,
  allowing the delivery of dynamic contents.
- Arbitrary functions can be associated to a page, to perform any needed
  actions.

The ENC28J60 Ethernet Controller is supported it through the EtherCard library,
so you will need it to compile the sketch. Get it at
https://github.com/jcw/ethercard/.

ESP8266 support is achieved through Bruno Portaluri's WiFiEsp library available
in the Library Manager or at https://github.com/bportaluri/WiFiEsp.

To switch between the target chipsets, and to enable/disable server features,
have a look at the webbino_config.h file: the WEBBINO_USE_* #defines will enable
support for the chosen network interface. Please be aware of the following:
- When ENC28J60 support is enabled, a lot of RAM is used for the packet buffer,
  as EtherCard currently needs it to be allocated in the main RAM. It is
  currently set at 800 bytes, which include TCP/IP headers, leaving about 750
  bytes for data to be sent to the client. Since the library also forces all
  communications to be single-packet, about 750 bytes is the maximum size of
  any data that is exchanged between server and client.
- When W5100 support is enabled, more RAM will be free, since the packets are
  stored on a small RAM inside the chip itself. Although, the binary size will
  increase quite a bit: about 5000 bytes. I'm not sure why this happens, as the
  W5100 chip should require much less code than the ENC28J60 to operate.
- Any file that must be available on the webserver can be converted to a C
  header file throught a Python script that is available in the "tools"
  directory. You can then include the generated file in the sketch as shown in
  the examples.

Included are a lot of examples showing how to use all the different features.


Webbino is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

License
=======
Where not otherwise indicated, the Webbino libraries are distributed under the
GNU General Public License v3.

Webbino is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Webbino.  If not, see <http://www.gnu.org/licenses/>.

That's all for the moment, please use the official github page to report issues
and such: https://github.com/SukkoPera/Webbino.

Enjoy!
