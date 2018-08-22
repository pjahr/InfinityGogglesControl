#include <FastLED.h>
#define NUM_LEDS 16
#define DATA_PIN 6

CRGB leds[NUM_LEDS];

int Order8[16]={0,1,2,3,4,5,6,7,15,14,13,12,11,10,9,8};

void setup()
{
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  
}

void loop() 
{
  //CircleHardwareOrder();
  SimpleEigth();
}

void SimpleEigth()
{  
  for(int i = 0; i < NUM_LEDS; i++)
  { 
    int dot = Order8[i];
    
    leds[dot] = CRGB::Green;
    
    FastLED.show();
    
    // clear this led for the next time around the loop
    leds[dot] = CRGB::Black;
    delay(60);
   }
}

void CircleHardwareOrder()
{
  for(int dot = 0; dot < NUM_LEDS; dot++)
  { 
      leds[dot] = getColorForIndex(dot);
      
      FastLED.show();
      
      // clear this led for the next time around the loop
      leds[dot] = CRGB::Black;
      delay(30);
   }
}

CRGB getColorForIndex(int i)
{
  if (i==0) return CRGB::Red;
  if (i==8) return CRGB::Orange;
  return CRGB::Blue;
}

void LightAll(CRGB color)
{
  for(int dot = 0; dot < NUM_LEDS; dot++)
  {
    leds[dot]=color;
  }
  FastLED.show();
}
