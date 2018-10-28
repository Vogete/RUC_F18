/*******************************************************************************
********************************************************************************
IMPORTANT NOTES:
https://github.com/Coopdis/easyMesh/issues/16
ONLY version 2.3.0 of the esp8266 board package is working!

Most stable easymesh library: https://github.com/sfranzyshen/easyMesh
PainlessMesh is cool and all, but has time sync issues, and not stable

Other useful stuff:
https://github.com/BlackEdder/easyMesheD



Pin mapping for wemos D1 mini (GPIO numbers):
    D8 = 15
    D6 = 12
    D2 = 4
********************************************************************************
*******************************************************************************/


#include <list>
#include "common.h"

#include "buttoncontrol.h"
#include "ledcontrol.h"
#include "meshcontrol.h"
#include "httpservercontrol.h"


// Scheduler userScheduler; // to control your personal task

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


// ESP8266WebServer HttpServerControl::http_rest_server;


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



void setup()
{
    setGlobalVariables();
    ButtonControl::setupButtons();
    LedControl::setupLed();

    delay(1000);

    Serial.begin(115200);

    MeshControl::setupMesh();

    // HttpServerControl::http_rest_server(Common::HTTP_REST_PORT);
    // rest api
    HttpServerControl::config_rest_server_routing();
    HttpServerControl::http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop()
{
    // userScheduler.execute(); // it will run mesh scheduler as well
    MeshControl::updateMesh();
    HttpServerControl::http_rest_server.handleClient();

    ButtonControl::handdleButtonPress();

    if (millis() - Common::timer > 1000 / Common::frameRate)
    {
        Common::timer = millis();
        LedControl::showLEDPattern();
        LedControl::loopLed();
    }
}

