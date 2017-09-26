# tpm2net

This repo is a collection of tools related to the TPM2.Net Protocol. The protocol was designed by forum members of http://www.ledstyles.de and later [translated into english by jblang](https://gist.github.com/jblang/89e24e2655be6c463c56). See the translation for further links.

## ESP8266

Support for ESP8266 is provided using the Arduino IDE and the library https://github.com/esp8266/Arduino.
Keep in mind: This is only a Proof-of-Concept solution and may not work on your boards. It is tested with WeMos D1 mini and the ESP8266 ESP12-Q (not quite sure about the exact version) by ai thinker on a custom pcb.

Data-Pins: D3 (WeMos), GPIO12 (ESP12)

Replace the connection settings for your access point.

The full potential of the microcontroller is not used and many features could be added, such as configuring the network, OTA or led strip configuration. Feel free to provide valuable suggestions.

### Additional Hardware
* 5V DC Power Supply
* WS2812 LED Strip or Matrix
* Optional: Soldering iron and wires

### Arduino IDE Settings
Following settings worked with the WeMos D1 mini, but may be changed depending on your hardware:
* Board: WeMos D1 R2
* CPU Frequency: 80 MHz
* Flash Size: 4M (3M SPIFFS)
* Upload Speed/Baud Rate: 921600

## Controller-Software

* [Jinx!](http://www.live-leds.de/) is confirmed to work with the provided code. At least up to 64 pixels.
  * Fast Patch: GRB, Vertical-Snake Top Left (for my 8x8 matrix)