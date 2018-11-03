#ifndef _BUTTONS_H_
#define _BUTTONS_H_

class ButtonControl
{
private:
public:
    ButtonControl(uint8_t pinNuber);

    uint8_t buttonPin;
    void setupButton(uint8_t pinNuber);
    void handdleButtonPress(void (*callbackFunc)());
};


// extern ButtonControl buttonControl;


#endif // !_BUTTONS_H_
