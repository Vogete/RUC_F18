#ifndef _LEDCONTROL_H_
#define _LEDCONTROL_H_



class LedControl
{
public:
    LedControl();
    void setupLed();
    void loopLed();
    void showLEDPattern();
    void changeLEDPattern();
    void ledRainbow();
    void ledRed();
    void ledNodeCount(int noNodes);
};

// extern LedControl ledControl;


#endif // !_LEDCONTROL_H_
