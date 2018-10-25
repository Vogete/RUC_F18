#include "common.h"

#include <list>
#include <painlessMesh.h>

// REST Server
#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "buttoncontrol.h"
#include "ledcontrol.h"

// -------------LED ------------
//stop LED flickering when wifi is on: https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0

painlessMesh mesh;
Scheduler userScheduler; // to control your personal task
ESP8266WebServer http_rest_server(Common::HTTP_REST_PORT);

Common common;
LedControl ledControl;
ButtonControl buttonControl;

// User stub
// void sendMessage(); // Prototype so PlatformIO doesn't complain

// Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

#define MESH_PREFIX "zfrWemosMesh"
#define MESH_PASSWORD "potatochips3214"
#define MESH_PORT 5555
#define MESH_CHANNEL 1
#define HOSTNAME "WEMOS-MESH"
#define STATION_SSID "Kukucs Guest"
#define STATION_PASSWORD "kukucs357"


// list of all the nodes in the mesh (excluding the current one)
std::list<uint32_t> meshNodes;
// extern uint32_t myNodeID;



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

    Common::NUM_LEDS = 8;

    Common::buttonState = 0;
    Common::lastButtonState = 0;

    Common::scale = 150; // 0-255

    Common::timer = 0;
    Common::frameRate = 40;

    Common::ledPattern = 1;

    Common::myNodeID = 0;
    Common::noNodes = 0;
}



// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    if (msg == "switch light mode")
    {
        ledControl.changeLEDPattern();
    }
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
    Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());


    meshNodes = mesh.getNodeList();
    int nodeNumber = meshNodes.size();
    Common::noNodes = meshNodes.size() + 1;

    String nodesOutput = "";

    Serial.printf("%d mesh in the network: ", Common::noNodes);
    Serial.printf("(My ID) %u | ", Common::myNodeID);
    for (std::list<uint32_t>::iterator node = meshNodes.begin(); node != meshNodes.end(); ++node)
    {
        Serial.printf("%u | ", *node);
    }
    Serial.printf("\n");

    // LED update could be put here :)
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
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


    //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    Common::myNodeID = mesh.getNodeId();
    Serial.printf("MyNodeID: %u\n", Common::myNodeID);

    if (Common::myNodeID == 3943449884)
    {
        mesh.setRoot(); //maybe helps with stabilization?
        mesh.stationManual(STATION_SSID, STATION_PASSWORD);
        mesh.setHostname(HOSTNAME);
        Serial.printf("Connected to WiFi network %s\n", STATION_SSID);
    }
    else
    {
        mesh.setContainsRoot(); //maybe helps with stabilization?
    }


    // userScheduler.addTask(taskSendMessage);
    // taskSendMessage.enable();


    // rest api
    config_rest_server_routing();
    http_rest_server.begin();
    Serial.println("HTTP REST Server Started");
}

void loop()
{
    userScheduler.execute(); // it will run mesh scheduler as well
    mesh.update();
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
    http_rest_server.send(200, "application/json", mesh.subConnectionJson().c_str());
}



void apiChangeLedPattern()
{
    ledControl.changeLEDPattern();
    common.sendMessage("switch light mode");
    http_rest_server.send(200, "text/html", "Changed LED Patter! :)");
}
