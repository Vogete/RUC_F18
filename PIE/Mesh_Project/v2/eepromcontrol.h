#ifndef _EEPROMCONTROL_H_
#define _EEPROMCONTROL_H_

#include <string>
#include "wificredstruct.h"

class EepromControl
{
public:
    EepromControl();
    // void dataToEEPROM(WifiCredStruct data, uint32_t eepromAddress);
    // WifiCredStruct eepromToData(uint32_t eepromAddress);
    void storeStruct(void *data_source, size_t size);
    void loadStruct(void *data_dest, size_t size);

};



#endif // !_EEPROMCONTROL_H_
