/*
IMPORTANT NOTES:
https://github.com/Coopdis/easyMesh/issues/16
ONLY version 2.3.0 of the esp8266 board package is working!

Most stable easymesh library: https://github.com/sfranzyshen/easyMesh
PainlessMesh is cool and all, but has time sync issues, and not stable
*/


#include "common.h"

#include <list>

// REST Server
#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "buttoncontrol.h"
#include "ledcontrol.h"
#include "meshcontrol.h"


// Scheduler userScheduler; // to control your personal task
ESP8266WebServer http_rest_server(Common::HTTP_REST_PORT);


// User stub
// void sendMessage(); // Prototype so PlatformIO doesn't complain

// Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);


int Common::HTTP_REST_PORT;
int Common::WIFI_RETRY_DELAY;

int Common::NUM_LEDS;

int Common::buttonState;
int Common::lastButtonState;

byte Common::scale;

unsigned long Common::timer;
unsigned int Common::frameRate;

byte Common::ledPattern;

uint32_t Common::myNodeID;
int Common::noNodes;

void setGlobalVariables(){
    Common::HTTP_REST_PORT = 80;
    Common::WIFI_RETRY_DELAY = 500;

    Common::NUM_LEDS = 9;

    Common::buttonState = 0;
    Common::lastButtonState = 0;

    Common::scale = 150; // 0-255

    Common::timer = 0;
    Common::frameRate = 40;

    Common::ledPattern = 1;

    Common::myNodeID = 0;
    Common::noNodes = 1;
}


void config_rest_server_routing()
{
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
                              "Welcome to the ESP8266 REST Web Server");
    });
    // http_rest_server.on("/mesh", HTTP_GET, getNodesInMesh);
    // http_rest_server.on("/changeLEDPattern", HTTP_GET, apiChangeLedPattern);

    // http_rest_server.on("/leds", HTTP_POST, post_put_leds);
    // http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}

void setup()
{
    setGlobalVariables();
    ButtonControl::setupButtons();
    LedControl::setupLed();

    delay(1000);

    Serial.begin(115200);

    MeshControl::setupMesh();

    // rest api
    config_rest_server_routing();
    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop()
{
    // userScheduler.execute(); // it will run mesh scheduler as well
    MeshControl::updateMesh();
    http_rest_server.handleClient();

    ButtonControl::handdleButtonPress();

    if (millis() - Common::timer > 1000 / Common::frameRate)
    {
        Common::timer = millis();
        LedControl::showLEDPattern();
        LedControl::loopLed();
    }
}

// REST Server stuff
void getNodesInMesh()
{
    String meshNodes = MeshControl::getNodesInMesh();
    http_rest_server.send(200, "application/json", meshNodes.c_str());
}


void apiChangeLedPattern()
{
    LedControl::changeLEDPattern();
    MeshControl::sendMeshMessage("switch light mode");
    http_rest_server.send(200, "text/html", "Changed LED Pattern! :)");
}
