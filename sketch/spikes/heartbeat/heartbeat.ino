#include <string.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN                     6
#define NUMPIXELS               16

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, PIN);

bool             _on = false;

void setup()
{
  pixel.begin(); // This initializes the NeoPixel library.
  for(uint8_t i=0; i<NUMPIXELS; i++)
  {
    pixel.setPixelColor(i, pixel.Color(50,0,0));
  }
  pixel.show();
  Serial.begin (9600);

  delay(1000);
}

void loop()
{
  uint8_t r=0;
  uint8_t g=0;
  uint8_t b=50;

  bool on = !(millis() & 640);

  if (_on==on)
  {
    return;
  }

  toggle(on);
}

void toggle(bool state)
{
  if (state)
  {
    setColor(255,0,0);
    _on=true;
  }
  else
  {
    setColor(0,0,0);
    _on=false;
  }
}

void setColor(uint8_t r, uint8_t g, uint8_t b)
{
  Serial.println(r);
    
  for(uint8_t i=0; i<NUMPIXELS; i++)
  {
    pixel.setPixelColor(i, pixel.Color(r,g,b));
  }
  pixel.show();
}
