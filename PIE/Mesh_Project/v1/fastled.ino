void setupLed()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setMaxRefreshRate(200);
}

void loopLed()
{
    FastLED.show(scale);
}
