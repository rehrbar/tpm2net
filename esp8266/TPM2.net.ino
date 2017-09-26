/*
 *  Settings for Controller Software: GRB, Vertical-Snake Top Left
 *  TODO Clean up code and add comments.
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "tpm2net.h"
#include <Arduino.h>
//#include <eagle_soc.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      64

void ICACHE_RAM_ATTR espShow(
 uint8_t pin, uint8_t *pixels, uint32_t numBytes, boolean is800KHz);


struct TPM2NET_HEADER {
  byte blockType;
  byte length[2];
  byte packetNumber;
  byte packetCount;
};

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

char packetBuffer[255]; //buffer to hold incoming packet

// Create an instance of the server
// specify the port to listen on as an argument
WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  pinMode(PIN, OUTPUT);
  delay(10);


  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  udp.begin(TPM2_NET_PORT);
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // if there's data available, read a packet
  int packetSize = udp.parsePacket();
  if (packetSize > 7 && udp.read() == TPM2_NET_BLOCK_START_BYTE) // Check for protocol.
  {
    TPM2NET_HEADER tmp;
    byte asdf[sizeof(tmp)];
    udp.read(asdf, sizeof(tmp));
    memcpy(&tmp, asdf, sizeof(tmp));

    if(tmp.blockType != TPM2_BLOCK_TYPE_DATA){
      Serial.println("Unsupported block type received.");
      return;
    }

    uint16_t payloadLength = word(tmp.length[0],tmp.length[1]);
    byte data[payloadLength];
    udp.read(data, payloadLength);
    if(udp.read() == TPM2_BLOCK_END_BYTE){
      espShow(PIN, data, payloadLength, true);
    } else {
      Serial.println("Malformed packet received.");
    }
  }
}

// WS2812 stuff
// Reduced version of Adafruit NeoPixel Library, but I can't
// remember why I did it like this.
// https://github.com/adafruit/Adafruit_NeoPixel/blob/master/esp8266.c
// TODO: Replace with library import that it conforms to LGPL of the library.
static uint32_t _getCycleCount(void) __attribute__((always_inline));
static inline uint32_t _getCycleCount(void) {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
  return ccount;
}

void ICACHE_RAM_ATTR espShow(
 uint8_t pin, uint8_t *pixels, uint32_t numBytes, boolean is800KHz) {

  noInterrupts();
#define CYCLES_800_T0H  (F_CPU / 2500000) // 0.4us
#define CYCLES_800_T1H  (F_CPU / 1250000) // 0.8us
#define CYCLES_800      (F_CPU /  800000) // 1.25us per bit
#define CYCLES_400_T0H  (F_CPU / 2000000) // 0.5uS
#define CYCLES_400_T1H  (F_CPU /  833333) // 1.2us
#define CYCLES_400      (F_CPU /  400000) // 2.5us per bit

  uint8_t *p, *end, pix, mask;
  uint32_t t, time0, time1, period, c, startTime, pinMask;

  pinMask   = _BV(pin);
  p         =  pixels;
  end       =  p + numBytes;
  pix       = *p++;
  mask      = 0x80;
  startTime = 0;

#ifdef NEO_KHZ400
  if(is800KHz) {
#endif
    time0  = CYCLES_800_T0H;
    time1  = CYCLES_800_T1H;
    period = CYCLES_800;
#ifdef NEO_KHZ400
  } else { // 400 KHz bitstream
    time0  = CYCLES_400_T0H;
    time1  = CYCLES_400_T1H;
    period = CYCLES_400;
  }
#endif

  for(t = time0;; t = time0) {
    if(pix & mask) t = time1;                             // Bit high duration
    while(((c = _getCycleCount()) - startTime) < period); // Wait for bit start
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);       // Set high
    startTime = c;                                        // Save start time
    while(((c = _getCycleCount()) - startTime) < t);      // Wait high duration
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);       // Set low
    if(!(mask >>= 1)) {                                   // Next bit/byte
      if(p >= end) break;
      pix  = *p++;
      mask = 0x80;
    }
  }
  while((_getCycleCount() - startTime) < period); // Wait for last bit
  interrupts();
}



