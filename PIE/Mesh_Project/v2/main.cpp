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

Common common;
LedControl ledControl;
ButtonControl buttonControl;
MeshControl meshControl;

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

    Common::NUM_LEDS = 4;

    Common::buttonState = 0;
    Common::lastButtonState = 0;

    Common::scale = 150; // 0-255

    Common::timer = 0;
    Common::frameRate = 40;

    Common::ledPattern = 1;

    Common::myNodeID = 0;
    Common::noNodes = 0;
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
    buttonControl.setupButtons();

    ledControl.setupLed();
    delay(1000);

    Serial.begin(115200);

    meshControl.setupMesh();

    // rest api
    config_rest_server_routing();
    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop()
{
    // userScheduler.execute(); // it will run mesh scheduler as well
    meshControl.updateMesh();
    http_rest_server.handleClient();

    buttonControl.handdleButtonPress();

    if (millis() - Common::timer > 1000 / Common::frameRate)
    {
        Common::timer = millis();
        ledControl.showLEDPattern();
        ledControl.loopLed();
    }
}

// REST Server stuff
void getNodesInMesh()
{
    String meshNodes = meshControl.getNodesInMesh();
    http_rest_server.send(200, "application/json", meshNodes.c_str());
}


void apiChangeLedPattern()
{
    ledControl.changeLEDPattern();
    common.sendMessage("switch light mode");
    http_rest_server.send(200, "text/html", "Changed LED Patter! :)");
}
