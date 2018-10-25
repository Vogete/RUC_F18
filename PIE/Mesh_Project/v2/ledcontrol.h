#ifndef _LEDCONTROL_POTATO_H_
#define _LEDCONTROL_POTATO_H_



class LedControl
{
public:
    LedControl();
    void setupLed();
    void loopLed();
    void showLEDPattern();
    void changeLEDPattern();
    void ledRainbow();
    void ledNodeCount(int noNodes);
};

// extern LedControl ledControl;


#endif // !_LEDCONTROL_H_
