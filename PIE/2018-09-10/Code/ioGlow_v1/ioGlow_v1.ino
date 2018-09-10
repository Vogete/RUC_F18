#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <WebSocketsServer.h>
#include <Hash.h>

WebSocketsServer webSocket = WebSocketsServer(81);


int sliderValues[5] ;
#define FASTLED_INTERRUPT_RETRY_COUNT 0
//#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#define allocatedLightPatterns 10
typedef void (*lightPatternFunction) ();
/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";
int lightPatternsAssigned = 0;
int lightPatternSelected = 1;
lightPatternFunction lightPatternFunctions[allocatedLightPatterns];
String lightPatternNames[allocatedLightPatterns];

// How many leds in your strip?
#define NUM_LEDS 70
String name = "ioGlow-";

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long timer = 0;
unsigned int frameRate = 40;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.setDebugOutput(true);
  setupWifi();
  addLightPattern(off, "Off");
  addLightPattern(colorChanger, "Color Changer");
  addLightPattern(red, "Red");
  
 
  setupLed();

}



void loop() {
  loopWifi();

  if (lightPatternsAssigned > lightPatternSelected && millis()-timer > 1000/frameRate)
  {
    timer = millis();
    lightPatternFunctions[lightPatternSelected]();
    loopLed();
  }

 
}

float value = 0;

void off()
{

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
    yield();
  }
}
void colorChanger() {
  value++;

  //value = constrain(value,0,100);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV((i + (int)value) % 255, 155, sliderValues[0]);
    yield();

  }
}

void red() {
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255, 0, 0);
    yield();
  }
}





