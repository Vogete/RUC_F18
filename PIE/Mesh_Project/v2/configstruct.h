#ifndef _CONFIGSTRUCT_H_
#define _CONFIGSTRUCT_H_

#include <stdint.h>
#include <string>

struct ConfigStruct
{
    bool isPairingMode;
    String SSID;
    String Password;
    uint16_t CommPort;
};

#endif
