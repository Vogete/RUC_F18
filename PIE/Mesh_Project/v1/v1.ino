//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
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

// ---------------- Button press ----------------

int buttonState = 0;
int lastButtonState = 0;

// ----------------------------------------------

#define MESH_PREFIX "zfrWemosMesh"
#define MESH_PASSWORD "potatochips3214"
#define MESH_PORT 5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// list of all the nodes in the mesh (excluding the current one)
std::list<uint32_t> meshNodes;
uint32_t myNodeID;

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

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
    pinMode(BUTTON_PIN, INPUT);

    setupLed();
    delay(1000);

    Serial.begin(115200);

    //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    // userScheduler.addTask(taskSendMessage);
    // taskSendMessage.enable();

    myNodeID = mesh.getNodeId();

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

void handdleButtonPress()
{
    buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState)
    {
        if (buttonState == HIGH)
        {
            changeLEDPattern();
            sendMessage("switch light mode");
        }
        lastButtonState = buttonState;
        delay(50);
    }
}

void showLEDPattern()
{
    switch (ledPattern)
    {
    case 1:
        ledNodeCount();
        break;
    case 2:
        ledRainbow();
        break;
    default:
        break;
    }
}

void changeLEDPattern()
{
    if (ledPattern == 1)
    {
        ledPattern = 2;
    }
    else
    {
        ledPattern = 1;
    }

    showLEDPattern();
}

void ledRainbow()
{

    for (int i = 0; i < NUM_LEDS; i++)
    {
        uint8_t hue = round((255 / NUM_LEDS) * i);
        leds[i] = CHSV(hue, 255, 255);
        Serial.println(hue);
        yield();
    }
}

void ledNodeCount()
{
    int noNodes = meshNodes.size() + 1;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (i <= noNodes - 1)
        {
            leds[i] = CRGB(0, 179, 60);
        }
        else
        {
            leds[i].fadeToBlackBy(100);
        }
        yield();
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
    http_rest_server.on("/changeLEDPattern", HTTP_GET, changeLEDPattern);
    // http_rest_server.on("/leds", HTTP_POST, post_put_leds);
    // http_rest_server.on("/leds", HTTP_PUT, post_put_leds);
}
