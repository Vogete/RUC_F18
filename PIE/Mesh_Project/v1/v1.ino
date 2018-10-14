#include "painlessMesh.h"

// REST Server
#include <stdio.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 50

ESP8266WebServer http_rest_server(HTTP_REST_PORT);

// -------------LED ------------
//stop LED flickering when wifi is on: https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#include <FastLED.h>
#define NUM_LEDS 8
#define DATA_PIN D2
#define BUTTON_PIN D8

byte scale = 150; // 0-255

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long timer = 0;
unsigned int frameRate = 40;
byte ledPattern = 1;

// -----------------------------

#define MESH_PREFIX "zfrWemosMesh"
#define MESH_PASSWORD "potatochips3214"
#define MESH_PORT 5555
#define MESH_CHANNEL 1
#define HOSTNAME "WEMOS-MESH"
#define STATION_SSID "Kukucs Guest"
#define STATION_PASSWORD "kukucs357"

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// list of all the nodes in the mesh (excluding the current one)
std::list<uint32_t> meshNodes;
uint32_t myNodeID;

// User stub
// void sendMessage(); // Prototype so PlatformIO doesn't complain

// Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage(String message, uint32_t destination = 0)
{
    // String msg = "Hello from node ";
    // msg += mesh.getNodeId();
    // taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
    if (destination == 0)
    {
        mesh.sendBroadcast(message);
    }
    else
    {
        mesh.sendSingle(destination, message);
    }
}


// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
    if (msg == "switch light mode")
    {
        changeLEDPattern();
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

    String nodesOutput = "";

    Serial.printf("%d mesh in the network: ", nodeNumber + 1);
    Serial.printf("(My ID) %u | ", myNodeID);
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

void setup()
{
    setupButtons();

    setupLed();
    delay(1000);

    Serial.begin(115200);


    //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    myNodeID = mesh.getNodeId();
    Serial.printf("MyNodeID: %u\n", myNodeID);

    if (myNodeID == 3943449884)
    {
        mesh.stationManual(STATION_SSID, STATION_PASSWORD);
        mesh.setHostname(HOSTNAME);
        Serial.printf("Connected to WiFi network %s\n", STATION_SSID);
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

    handdleButtonPress();

    if (millis() - timer > 1000 / frameRate)
    {
        timer = millis();
        showLEDPattern();
        loopLed();
    }
}

// REST Server stuff

void getNodesInMesh()
{
    http_rest_server.send(200, "application/json", mesh.subConnectionJson().c_str());
}

void config_rest_server_routing()
{
    http_rest_server.on("/", HTTP_GET, []() {
        http_rest_server.send(200, "text/html",
                              "Welcome to the ESP8266 REST Web Server");
    });
    http_rest_server.on("/mesh", HTTP_GET, getNodesInMesh);
    http_rest_server.on("/changeLEDPattern", HTTP_GET, apiChangeLedPattern);
    // http_rest_server.on("/leds", HTTP_POST, post_put_leds);
    // http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}

void apiChangeLedPattern()
{
    changeLEDPattern();
    sendMessage("switch light mode");
    http_rest_server.send(200, "text/html", "Changed LED Patter! :)");
}
