void setupLed()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setMaxRefreshRate(200);
}

void loopLed()
{
    FastLED.show(scale);
}


void showLEDPattern()
{
    switch (ledPattern)
    {
    case 1:
        ledNodeCount();
        break;
    case 2:
        ledRainbow();
        break;
    default:
        break;
    }
}

void changeLEDPattern()
{
    if (ledPattern == 1)
    {
        ledPattern = 2;
    }
    else
    {
        ledPattern = 1;
    }

    showLEDPattern();
}

void ledRainbow()
{

    for (int i = 0; i < NUM_LEDS; i++)
    {
        uint8_t hue = round((255 / NUM_LEDS) * i);
        leds[i] = CHSV(hue, 255, 255);
        Serial.println(hue);
        yield();
    }
}

void ledNodeCount()
{
    int noNodes = meshNodes.size() + 1;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (i <= noNodes - 1)
        {
            leds[i] = CRGB(0, 179, 60);
        }
        else
        {
            leds[i].fadeToBlackBy(100);
        }
        yield();
    }
}
