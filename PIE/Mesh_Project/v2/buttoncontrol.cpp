#include "buttoncontrol.h"
#include "common.h"
#include "ledcontrol.h"
#include "meshcontrol.h"

#define BUTTON_PIN D8


ButtonControl::ButtonControl(){
}

void ButtonControl::setupButtons()
{
    pinMode(BUTTON_PIN, INPUT);
}

void ButtonControl::handdleButtonPress()
{
    Common::buttonState = digitalRead(BUTTON_PIN);
    if (Common::buttonState != Common::lastButtonState)
    {
        if (Common::buttonState == HIGH)
        {
            Serial.println("Button pressed");
            LedControl::changeLEDPattern();
            MeshControl::sendMeshMessage("switch light mode");
        }
        Common::lastButtonState = Common::buttonState;
        // delay(30);
    }
}


// ButtonControl buttonControl = ButtonControl();
