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
#include "FastLED.h"
#define allocatedLightPatterns 10
typedef void (*lightPatternFunction) ();
/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

int lightPatternsAssigned = 0;
int lightPatternSelected = 0;
lightPatternFunction lightPatternFunctions[allocatedLightPatterns];
String lightPatternNames[allocatedLightPatterns];

// How many leds in your strip?
#define NUM_LEDS 70

#define ULTRASONIC 0

String name = "ioGlow-x";

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long timer = 0;
unsigned int frameRate = 40;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();

  //Serial.setDebugOutput(true);
  setupWifi();
  addLightPattern(off, "Off");
  addLightPattern(red, "Red");
  addLightPattern(rgb, "RGB");
  addLightPattern(hsv, "HSV");
  addLightPattern(chase, "Chase");
  addLightPattern(freePattern, "Free Pattern");


  if (ULTRASONIC)
  {
    addLightPattern(interactive, "Interactive");
    usSetup();
  }


  setupLed();

}



void loop() {
  loopWifi();

  if (lightPatternsAssigned > lightPatternSelected && millis() - timer > 1000 / frameRate)
  {
    timer = millis();
    lightPatternFunctions[lightPatternSelected]();
    loopLed();
  }


}


unsigned long blinkTimer = 0;
void off()
{

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
    yield();
  }
  if (millis() - blinkTimer > 1000)
  {
    blinkTimer = millis();
   
    leds[0] = CRGB(0, 255, 0);
  }
}
void hsv() {


  //value = constrain(value,0,100);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(sliderValues[0], sliderValues[1], sliderValues[2]);
    yield();

  }

}

void rgb() {


  //value = constrain(value,0,100);
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(sliderValues[0], sliderValues[1], sliderValues[2]);
    yield();

  }
  Serial.println(sliderValues[0]);
}



void red() {

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB(255, 0, 0);
    yield();
  }
}

int value = 0;
void chase() {
  value = (value + 1) % NUM_LEDS;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    if (value == i)
    {
      leds[i] = CRGB(255, 0, 0);
    }
    else
    {
      leds[i].fadeToBlackBy( 64 ); // 25% 64/256th
    }
    yield();
  }
}


void interactive()
{
  if (ULTRASONIC)
  {
    float value = usRead();
    int valueMapped = constrain(map(value, 0, 30, 255, 0), 0, 255);
    Serial.println(value);

    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB(valueMapped, 0, 0);
      yield();
    }
  }

}

void freePattern()
{

  // free pattern that you can play with
}





