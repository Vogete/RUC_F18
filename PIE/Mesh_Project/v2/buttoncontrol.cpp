#include "buttoncontrol.h"
#include "common.h"
#include "ledcontrol.h"
#include "meshcontrol.h"

// #define BUTTON_PIN 15 //D8

uint8_t buttonPin;

ButtonControl::ButtonControl(uint8_t pinNumber){
    buttonPin = pinNumber;
    setupButton(buttonPin);
}

void ButtonControl::setupButton(uint8_t pinNumber)
{
    pinMode(pinNumber, INPUT);
}

void ButtonControl::handdleButtonPress(void (*callbackFunc)())
{
    Common::buttonState = digitalRead(buttonPin);
    if (Common::buttonState != Common::lastButtonState)
    {
        if (Common::buttonState == HIGH)
        {
            (*callbackFunc) ();
        }
        Common::lastButtonState = Common::buttonState;
        delay(30);
    }
}


// ButtonControl buttonControl = ButtonControl();
