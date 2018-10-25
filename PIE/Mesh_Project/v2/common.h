#ifndef _COMMON_POTATO_H_
#define _COMMON_POTATO_H_

#define byte uint8_t

#include <string>
#include <list>


class Common
{
public:
    Common();
    static int HTTP_REST_PORT;
    static int WIFI_RETRY_DELAY;

    static int NUM_LEDS;

    static int buttonState;
    static int lastButtonState;

    static byte scale;

    static unsigned long timer;
    static unsigned int frameRate;

    static byte ledPattern;

    static uint32_t myNodeID;
    static int noNodes;

    void sendMessage(std::string message, uint32_t destination = 0);
};

// extern std::string MESH_PREFIX = "zfrWemosMesh";
// extern std::string MESH_PASSWORD = "potatochips3214";
// extern int MESH_PORT = 5555;
// extern int MESH_CHANNEL = 1;
// extern std::string HOSTNAME = "WEMOS-MESH";
// extern std::string STATION_SSID = "Kukucs Guest";
// extern std::string STATION_PASSWORD = "kukucs357";

// extern Common common;


#endif // !_COMMON_H_
