// http://www.cplusplus.com/forum/beginner/4639/






// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN D3







void addLightPattern(lightPatternFunction _function, String _name)
{
  if (allocatedLightPatterns > lightPatternsAssigned)
  {
    lightPatternFunctions[lightPatternsAssigned] = _function;
    lightPatternNames[lightPatternsAssigned] = _name;
    lightPatternsAssigned++;
  }

}



void setupLed()
{
 FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
 FastLED.setMaxRefreshRate(200); 
  
}


void loopLed()
{
 


  FastLED.show();
}

