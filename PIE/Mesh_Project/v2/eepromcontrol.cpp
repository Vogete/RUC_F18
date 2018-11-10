#include "eepromcontrol.h"
#include <EEPROM.h>

EepromControl::EepromControl() {
}

// void EepromControl::dataToEEPROM(WifiCredStruct data, uint32_t eepromAddress)
// {
//     EEPROM.begin(512);

//     // for(uint32_t i = 0; i < textToStore.length(); i++){
//     //     EEPROM.write(eepromAddress, textToStore[i]);
//     // }

//     // bool success = EEPROM.commit();
//     // return success;

//     EEPROM.write(eepromAddress, data);
//     EEPROM.commit();

// }

// WifiCredStruct EepromControl::eepromToData(uint32_t eepromAddress)
// {
//     // // TODO
//     // std::string temp = "";
//     // uint32_t length = 512;


//     // for(uint32_t i = 0; i < eepromAddress + length; ++i)
//     // {

//     //     if (char(EEPROM.read(i)) != ';') {
//     //         temp = temp + char(EEPROM.read(i));
//     //     }
//     //     else
//     //     {
//     //         i = eepromAddress+length;
//     //     }
//     // }

//     // return temp;

//     WifiCredStruct data = EEPROM.read(eepromAddress);

//     return data;
// }




void EepromControl::storeStruct(void *data_source, size_t size)
{
  EEPROM.begin(size * 2);
  for(size_t i = 0; i < size; i++)
  {
    char data = ((char *)data_source)[i];
    EEPROM.write(i, data);
  }
  EEPROM.commit();
}

void EepromControl::loadStruct(void *data_dest, size_t size)
{
    EEPROM.begin(size * 2);
    for(size_t i = 0; i < size; i++)
    {
        char data = EEPROM.read(i);
        ((char *)data_dest)[i] = data;
    }
}
