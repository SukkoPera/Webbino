# Webbino

![GitHub release (latest by date)](https://img.shields.io/github/v/release/SukkoPera/Webbino)
![GitHub Release Date](https://img.shields.io/github/release-date/SukkoPera/Webbino?color=blue&label=last%20release)
![GitHub commits since latest release (by date)](https://img.shields.io/github/commits-since/SukkoPera/Webbino/latest?color=orange)
[![arduino/arduino-lint-action](https://github.com/SukkoPera/Webbino/actions/workflows/main.yml/badge.svg)](https://github.com/SukkoPera/Webbino/actions/workflows/main.yml)

Webbino is a webserver for Arduino.  I originally wrote it for my [SmartStrip project](https://github.com/SukkoPera/SmartStrip), which required several unique
features which were not present in other webservers that were available at the time:
- Compatible with many network interfaces:
  - WizNet W5100: Original Ethernet Shield
  - WizNet W5500: [Ethernet Shield 2](https://store.arduino.cc/arduino-ethernet-shield-2)
  - ENC28J60: [KMTronic DINo](http://sigma-shop.com/product/72/web-internet-ethernet-controlled-relay-board-arduino-compatible-rs485-usb.html) (first version)
  - [Teensy 4.1 Native Ethernet](https://www.pjrc.com/store/teensy41.html)
  - WINC1500 (Wi-Fi): [WiFi Shield 101](https://www.arduino.cc/en/Main/ArduinoWiFiShield101), [MKR1000](https://www.arduino.cc/en/Main/ArduinoMKR1000) - EXPERIMENTAL, MIGHT NOT WORK!
  - ESP8266 (Wi-Fi): Either as an add-on to Arduino, like the MLT shield, or standalone like [NodeMCU](http://www.nodemcu.com), WeMos D1/[D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html), etc...
  - [ESP32 (Wi-Fi)](http://esp32.net)
- HTML pages and other data can be embedded in the code. This avoids the need of an SD card for simple sites.
- If you prefer, HTML pages can also be stored on an SD card (long file names are supported!)
- Tags like `$WHATEVER$` in HTML pages can be replaced at page request time, allowing the delivery of dynamic contents.
- Arbitrary functions can be associated to a page, to perform any needed actions: turn on LEDs, relays, save data to EEPROM, etc.
- Supports HTTP Basic Authentication.

To learn how to use the library, I strongly recommend to have a look at the vast number of examples that are included with it.

**Full documentation, including a step-by-step tutorial, is available on [the Webbino wiki](https://github.com/SukkoPera/Webbino/wiki)**.

## License
Webbino is free software: you can redistribute it and/or modify it under the terms of version 3 of the GNU General Public License as published by the Free Software Foundation.

Webbino is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Webbino. If not, see <http://www.gnu.org/licenses/>.

## Support
Please use the [official github page](https://github.com/SukkoPera/Webbino) to report issues, suggestions and such.

## Thanks
Thanks to [gpb01](https://github.com/gpb01) for [Teensy 4.1](https://www.pjrc.com/store/teensy41.html) support and other contributions.
