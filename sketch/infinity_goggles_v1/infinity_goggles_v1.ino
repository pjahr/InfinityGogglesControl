#include <string.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN                     6
#define NUMPIXELS               20

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, PIN);
unsigned long _time;
void setup()
{
  pixel.begin(); // This initializes the NeoPixel library.
  for(uint8_t i=0; i<NUMPIXELS; i++)
  {
    pixel.setPixelColor(i, pixel.Color(0,0,0));
  }
  pixel.show();
  Serial.begin (9600);
}

void loop()
{
  _time = millis();
  
  int timer=(_time%1000)/50;

  for(uint8_t i=0; i<NUMPIXELS; i++)
  {
    uint8_t r=0;
    uint8_t g=0;
    uint8_t b=0;

    if (i==timer+1)
    {
      r=0;
      g=255;
      b=0;
    }
  
    if (i==timer)
    {
      r=0;
      g=0;
      b=255;
    }
    if (i==timer-1)
    {
      r=0;
      g=0;
      b=150;
    }
    if (i==timer-2)
    {
      r=0;
      g=0;
      b=100;
        pixel.setPixelColor(i, pixel.Color(r,g,b));
    }
    if (i==timer-3)
    {
      r=0;
      g=0;
      b=50;
        pixel.setPixelColor(i, pixel.Color(r,g,b));
    }
    
  }
  
  pixel.show();
  delay(100);
}
