/*******************************************************************************
********************************************************************************
IMPORTANT NOTES:
https://github.com/Coopdis/easyMesh/issues/16
ONLY version 2.3.0 of the esp8266 board package is working!

Most stable easymesh library: https://github.com/sfranzyshen/easyMesh
PainlessMesh is cool and all, but has time sync issues, and not stable

Other useful stuff:
https://github.com/BlackEdder/easyMesheD



Pin mapping for wemos D1 mini (GPIO numbers) (https://github.com/esp8266/Arduino/blob/master/variants/d1_mini/pins_arduino.h#L49-L61):
    D0 = 16
    D1 = 5
    D2 = 4
    D3 = 0
    D4 = 2 //LED_BUILTIN
    D5 = 14
    D6 = 12
    D7 = 13
    D8 = 15
    RX = 3
    TX = 1
********************************************************************************
*******************************************************************************/


#include <string>
#include <list>

#include "common.h"
#include "buttoncontrol.h"
#include "ledcontrol.h"
#include "meshcontrol.h"
#include "httpservercontrol.h"
#include "eepromcontrol.h"
#include "wificredstruct.h"

ButtonControl button1(15); //D8
ButtonControl pairingButton(12); //D6

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


void SwitchLightMode(){
    Serial.println("Button pressed in callback");
    LedControl::changeLEDPattern();
    MeshControl::sendMeshMessage("switch light mode");
}


void setup()
{
    setGlobalVariables();
    LedControl::setupLed();
    delay(1000);
    Serial.begin(115200);

    Serial.println("--------------------");

    WifiCredStruct testData = {
        "storedTestSSID",
        "testpassword",
        1234
    };
    Serial.println(sizeof(testData));

    EepromControl test;
    test.storeStruct(&testData, sizeof(testData));

    WifiCredStruct backData;
    test.loadStruct(&backData, sizeof(testData));

    Serial.println(backData.SSID);
    Serial.println(backData.Password);
    Serial.println(backData.CommPort);

    Serial.println("--------------------");

    String meshSSID = backData.SSID;
    String meshPW = backData.Password;

    // MeshControl::setupMesh("zfrWemosMesh", "potatochips3214");
    MeshControl::setupMesh(meshSSID, meshPW);

    // HttpServerControl::http_rest_server(Common::HTTP_REST_PORT);
    // rest api
    HttpServerControl::config_rest_server_routing();
    HttpServerControl::http_rest_server.begin();
    Serial.println("HTTP REST Server Started");

    // void (*lightmodeSwitchPtr)() = &SwitchLightMode;
    button1.setShortButtonPressMethod(&SwitchLightMode);
}

void loop()
{
    // userScheduler.execute(); // it will run mesh scheduler as well
    MeshControl::updateMesh();
    HttpServerControl::http_rest_server.handleClient();

    button1.handdleButtonPress();

    if (millis() - Common::timer > 1000 / Common::frameRate)
    {
        Common::timer = millis();
        LedControl::showLEDPattern();
        LedControl::loopLed();
    }
}




