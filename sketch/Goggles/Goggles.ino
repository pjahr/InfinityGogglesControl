#include <Adafruit_BLE_Firmata.h>
#include <Adafruit_BLE_Firmata_Boards.h>
#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"
#include "FastLED.h"

#define ResetBleModule  1
#define DataPin         6
#define NumberOfPixels  16
#define IntMax          65536
#define ColorOrder      RGB        // in use?
#define LedStripType    NEOPIXEL
#define MaxBrightness   255        // full power
#define FramesPerSecond 120

// related to Fire2012
#define CoolingFactor  55
#define SparkingFactor 120

//#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


/*=========================================================================*/

CRGB     _leds[NumberOfPixels];
CRGB     _baseColor;
uint8_t  _baseColorHue = 180;

uint16_t _currentFrame     = 0;     // INFO: move this variable to the void loop() scope and save some CPU ?
uint16_t _animationSpeed   = 100;    // number of frames to increment per loop
uint8_t  _currentAnimation = 18;
uint8_t  _brightness       = 50;    //Global brightness percentage -IN USE?
bool     _reverseDirection = false; // INFO: currently constant

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// BLE module related, function prototypes over in packetparser.cpp
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
uint8_t                  readPacket(Adafruit_BLE *ble, uint16_t timeout);
float                    parsefloat(uint8_t *buffer);
void                     printHex(const uint8_t * data, const uint32_t numBytes);
extern uint8_t           _packetbuffer[];

// List of patterns to cycle through.  Each is defined as a separate function below.
//typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };


void SetupFastLed()
{
  FastLED.addLeds<LedStripType, DataPin>(_leds, NumberOfPixels).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MaxBrightness);
  FastLED.clear();
  FastLED.show();
}

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void SetupBleModule()
{
  if (!ble.begin(VERBOSE_MODE))
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  if ( ResetBleModule )
  {
    if (!ble.factoryReset())
    {
      error(F("Couldn't factory reset"));
    }
  }
    
  ble.echo(false);     // disable command echo 
  ble.info();          // print module information
  ble.verbose(false);  // debug info is a little annoying after this point!

  // wait for connection
  while (! ble.isConnected())
  {
      delay(500);
  }
  Serial.println("Connectin established");
  ble.setMode(BLUEFRUIT_MODE_DATA); // set mode to DATA
}

/**************************************************************************/
/*   Initialization. Runs once before main loop.
/**************************************************************************/
void setup(void)
{
  delay(3000);
  SetupFastLed();
  SetupBleModule();  
}

void RunCurrentAnimation()
{
  switch(_currentAnimation)
  {
    case  1: RingPair    (_currentFrame); break;
    case  2: DoubleChaser(_currentFrame); break;
    case  3: TripleBounce(_currentFrame); break;
    case  4: WaveInt     (_leds, _currentFrame, _baseColorHue); break;
    case  5: Wave        (_leds, _currentFrame, _baseColorHue); break;
    case  6: Spark       (_leds, _currentFrame, 255, _baseColorHue); break; // Overloaded version of "Spark" with Hue value, 255 for fade is the slowest fade possible. 256 is on/off
    case  7: Spark       (_leds, _currentFrame, 246, _baseColorHue);break; //Overloaded version of "Spark" with Hue value, 246 fade is faster which makes for a sharper dropoff
    case  8: Spark       (_leds, _currentFrame, 255); break;               //"Spark" function without hue make a white spark, 255 for fade is the slowest fade possible.
    case  9: Spark       (_leds, _currentFrame, 220); break;           //"Spark" function without hue make a white spark, 246 fade is faster which makes for a sharper dropoff      
    case 10: RainbowSpark(_leds,_currentFrame, 240); break;            //240 for dropoff is a pretty sharp fade, good for this _currentAnimation
    case 11: rainbow(); break;
    case 12: rainbowWithGlitter(); break;
    case 13: confetti(); break;
    case 14: sinelon(); break;
    case 15: juggle(); break;
    case 16: bpm(); break;
    case 17: Fire2012(); break;
    case 18: SimpleEigth(); delay(48); break;
    
    default: FastLED.clear();
      delay(100); //Animation OFF
  }
}

char GetCommandFromBleModule()
{
  // check if ble data is available -INFO: does this block?
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0)
  {
    return '-';                        // checksum failed
  }
  
  char command = _packetbuffer[1];
  if (command != 'C' && command != 'A') 
  {
     return '?';                       // unknown commands
  }
  
  return command;                      // ok
}

void ChangeCurrentAnimation()
{
    _currentAnimation = _packetbuffer[2];
    Serial.print("Changed animation to #");
    Serial.println(_currentAnimation);
}

void ChangeBaseColor()
{
  uint8_t red   = _packetbuffer[2];
  uint8_t green = _packetbuffer[3];
  uint8_t blue  = _packetbuffer[4];
  Serial.print("RGB #");
  if (red < 0x10) Serial.print("0");
  Serial.print(red, HEX);
  if (green < 0x10) Serial.print("0");
  Serial.print(green, HEX);
  if (blue < 0x10) Serial.print("0");
  Serial.println(blue, HEX);

  _baseColor.setRGB( red, green, blue);
  _baseColorHue=rgb2hsv_approximate(_baseColor).h;
}

void TripleBounce(uint16_t frame)   //3 chaser animations offset by 120 degrees each
{
  FastLED.clear();    //Clear previous buffer
  Bounce(frame,0);
  Bounce(frame+(IntMax/3),100);
  Bounce(frame+(IntMax/3)*2,150);
}

void DoubleChaser(uint16_t frame)   //2 chaser animations offset 180 degrees
{
  FastLED.clear();    //Clear previous buffer
  frame = frame * 2;
  Ring(frame, 0);
  Ring(frame + (IntMax / 2), 150);
}

void RingPair(uint16_t frame)     //2 rings animations at inverse phases
{
  FastLED.clear();    //Clear previous buffer
  Ring(frame, 30);
  Ring(IntMax - frame, 150);
}

void RainbowSpark(CRGB targetStrip[], uint16_t _currentAnimationFrame,uint8_t fade)
{
  // Color spark where hue is function of frame
  Spark(targetStrip, _currentAnimationFrame, fade, _currentAnimationFrame/255);
}

// Python-esc modulo.
int m(int a, int b)
{
  int c = a % b;
  if (c < 0)  { c += b; }
  return c;
}

// Shortcut for modulo 16.
int m16(int x)
{
  return m(x, 16);
}

int Order8[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8 };
int _step = 0;

void SimpleEigth()
{  
  for(int i = 0; i < NumberOfPixels; i++)
  { 
    int dot = Order8[i];

    if(i==m16(_step))
    {
      _leds[dot] = CRGB::Green; 
    }
    else
    {
      _leds[dot] = CRGB::Black;
    }
  }
  _step++;
}

void CircleHardwareOrder()
{
  for(int dot = 0; dot < NumberOfPixels; dot++)
  { 
      _leds[dot] = getColorForIndex(dot);
      
      FastLED.show();
      
      // clear this led for the next time around the loop
      _leds[dot] = CRGB::Black;
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
  for(int dot = 0; dot < NumberOfPixels; dot++)
  {
    _leds[dot]=color;
  }
  FastLED.show();
}


//#######################################################################################################
//##                                PRIMATIVE ANIMATION FUNCTIONS                                      ##
//#######################################################################################################



//*********************     Bounce      ***************************
// Linear "Larson scanner" (or knight rider effect) with anti-aliasing
// Color is determined by "hue"
//*****************************************************************
void Bounce(uint16_t frame, uint8_t hue)
{
  uint16_t pos16;
  if (frame < (IntMax / 2))
  {
    pos16 = frame * 2;
  
  }
  else
  {
    pos16 = IntMax - ((frame - (IntMax/2))*2);
  }

  int position = map(pos16, 0, IntMax, 0, ((NumberOfPixels) * 16));
  drawFractionalBar(_leds, position, 3, hue,0);
}

//************************          Ring           ******************************
// Anti-aliased cyclical chaser, 3 pixels wide
// Color is determined by "hue"
//*****************************************************
void Ring(uint16_t frame, uint8_t hue)
{
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  int pos16 = map(frame, 0, IntMax, 0, ((stripLength) * 16));
  drawFractionalBar(_leds, pos16, 3, hue,1);
}

//***************************   Wave [Float Math]  *******************************
// Squeezed sine wave  
// Uses slow, Arduino sin() function
// Squeezing achieved by using an exponential (^8) sin value
// Color is determined by "hue"
//***********************************************************************************
void Wave(CRGB targetStrip[], uint16_t frame, uint8_t hue)
{
  FastLED.clear();    //Clear previous buffer
  float deg; 
  float value; 
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  for(uint8_t i=0;i<stripLength;i++)
  {
    deg=float(frame+((IntMax/stripLength)*i))/(float(IntMax)) * 360.0;
    value = pow(sin(radians(deg)),8);    //Squeeeeeeze

    if(value>=0){   //Chop sine wave (no negative values)
      targetStrip[i] += CHSV(hue,255,value*256);
    }
  } 
}

//***************************   Wave [Integer Math]  *******************************
// unadulterated sine wave.  
// Uses FastLED sin16() and no float math for efficiency. 
// Since im stuck with integer values, exponential wave-forming is not possible (unless i'm wrong???)
// Color is determined by "hue"
//***********************************************************************************
void WaveInt(CRGB targetStrip[], uint16_t frame, uint8_t hue)
{
  FastLED.clear();
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  uint8_t value;
  for(uint8_t i=0;i<stripLength;i++)
  {
    value=(sin16(frame+((IntMax/stripLength)*i)) + (IntMax/2))/256;   
    if(value>=0)
    {   
      targetStrip[i] += CHSV(hue,255,value);
    }
  } 
}

//********************************   Color Spark  ***********************************
// Color of the sparks is determined by "hue"
// Frequency of sparks is determined by global var "_animationSpeed"
// "_animationSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this _currentAnimation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetStrip[], uint16_t _currentAnimationFrame, uint8_t fade, uint8_t hue)
{
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  uint16_t rand = random16();

  for(int i=0;i<stripLength;i++)
  {   
    targetStrip[i].nscale8(fade);
  }

  if(rand < (IntMax / (256 - (constrain(_animationSpeed,1,256)))))  ;
  {
    targetStrip[rand % stripLength].setHSV(hue,255,255);
  }
}

//******************************       Spark       **********************************
// Same as color spark but no hue value, // in HSV white is any hue with 0 saturation
// Frequency of sparks is a percentage mapped to global var "_animationSpeed"
// "_animationSpeed" var contrained from 1 - 255 (0.4% - 100%)
// "fade" parameter specifies dropoff (next frame brightness = current frame brightness * (x/256)
// fade = 256 means no dropoff, pixels are on or off
// NOTE: this _currentAnimation doesnt clear the previous buffer because the fade/dropoff is a function of the previous LED state
//***********************************************************************************
void Spark(CRGB targetStrip[], uint16_t _currentAnimationFrame,uint8_t fade)
{
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  uint16_t rand = random16();
  
  for(int i=0;i<stripLength;i++)
  {   
    targetStrip[i].nscale8(fade);
  }  

  if(rand < (IntMax / (256 - (constrain(_animationSpeed,1,255)))))
  {
    targetStrip[rand % stripLength].setHSV(0,0,255);
  }
}

//******************************       Fire 2012       **********************************
void Fire2012()
{
  static byte heat[NumberOfPixels]; // Array of temperature readings at each simulation cell

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NumberOfPixels; i++) 
  {
    heat[i] = qsub8( heat[i],  random8(0, ((CoolingFactor * 10) / NumberOfPixels) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NumberOfPixels - 1; k >= 2; k--) 
  {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }
  
  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if( random8() < SparkingFactor ) 
  {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160,255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for( int j = 0; j < NumberOfPixels; j++)
  {
    CRGB color = HeatColor( heat[j]);
    int pixelnumber;
    if( _reverseDirection )
    {
      pixelnumber = (NumberOfPixels-1) - j;
    } else 
    {
      pixelnumber = j;
    }
    _leds[pixelnumber] = color;
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( _leds, NumberOfPixels, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    _leds[ random16(NumberOfPixels) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( _leds, NumberOfPixels, 10);
  int pos = random16(NumberOfPixels);
  _leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( _leds, NumberOfPixels, 20);
  int pos = beatsin16(13,0,NumberOfPixels);
  _leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NumberOfPixels; i++) { //9948
    _leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( _leds, NumberOfPixels, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    _leds[beatsin16(i+7,0,NumberOfPixels)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


//Anti-aliasing code care of Mark Kriegsman Google+: https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw
void drawFractionalBar(CRGB targetStrip[], int pos16, int width, uint8_t hue, bool wrap)
{
  uint8_t stripLength = sizeof(_leds)/sizeof(CRGB);
  uint8_t i = pos16 / 16;                               // convert from pos to raw pixel number

  uint8_t frac = pos16 & 0x0F;                          // extract the 'factional' part of the position
  uint8_t firstpixelbrightness = 255 - (frac * 16);
  
  uint8_t lastpixelbrightness = 255 - firstpixelbrightness;

  uint8_t bright;
  for (int n = 0; n <= width; n++) {
    if (n == 0) {
      // first pixel in the bar
      bright = firstpixelbrightness;
    }
    else if (n == width) {
      // last pixel in the bar
      bright = lastpixelbrightness;
    }
    else {
      // middle pixels
      bright = 255;
    }

    targetStrip[i] += CHSV(hue, 255, bright );
    i++;
    if (i == stripLength)
    {
      if (wrap == 1) {
        i = 0; // wrap around
      }
      else{
        return;
      }
    }
  }
}

/**************************************************************************/
/*   Main loop. Runs after Initialization until power loss.
/**************************************************************************/
void loop(void)
{
  RunCurrentAnimation();
  
  FastLED.show();                       //All animations are applied!..send the results to the strip(s)
  _currentFrame += _animationSpeed;
  //Serial.print('F');
  //Serial.println(_currentFrame);
  
  FastLED.delay(1000/FramesPerSecond);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  
  // // Call the current pattern function once, updating the 'leds' array
  // gPatterns[gCurrentPatternNumber]();

  // // send the 'leds' array out to the actual LED strip
  // FastLED.show();  
  // // insert a delay to keep the framerate modest
  // FastLED.delay(1000/FramesPerSecond); 

  // // do some periodic updates
  // EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  // EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  
  char command = GetCommandFromBleModule();

  switch (command)
  {
    case 'A': ChangeCurrentAnimation(); break;
    case 'C': ChangeBaseColor(); break;
  } 
}
