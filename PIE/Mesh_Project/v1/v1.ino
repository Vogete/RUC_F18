//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"

// -------------LED ------------
//stop LED flickering when wifi is on: https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0  //?????

#include <FastLED.h>
#define NUM_LEDS 8
#define DATA_PIN D2
byte scale = 150; // 0-255

// Define the array of leds
CRGB leds[NUM_LEDS];

unsigned long timer = 0;
unsigned int frameRate = 40;

// -----------------------------

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

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage()
{
    String msg = "Hello from node ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast(msg);
    taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg)
{
    Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
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

    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();

    myNodeID = mesh.getNodeId();
}

void loop()
{
    userScheduler.execute(); // it will run mesh scheduler as well
    mesh.update();

    if (millis() - timer > 1000 / frameRate)
    {
        timer = millis();
        ledNodeCount();
        loopLed();
    }
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
    for (int i = 0; i <  NUM_LEDS; i++)
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
