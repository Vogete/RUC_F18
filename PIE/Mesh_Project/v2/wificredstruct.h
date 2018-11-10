#ifndef _WIFICREDSTRUCT_H_
#define _WIFICREDSTRUCT_H_

#include <stdint.h>
#include <string>

struct WifiCredStruct
{
    char SSID[128];
    char Password[128];
    uint16_t CommPort;
};

#endif
