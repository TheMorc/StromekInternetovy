/// @file    UnoStromek.ino
/// @brief   FastLED seriovka na pouziti s raspberryckom na kerom je cheerlights
/// @example UnoStromek.ino

#include <FastLED.h>
FASTLED_USING_NAMESPACE
#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    198
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  Serial.println("WELCOME TO UNOSTROMEK - INTERNETOVY STROMEK PROJEKT BISKUPOVA");
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, sinelon, blwhite };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

String serialColor;  
String lastSerialColor;
bool gHueEnabled = false;

void loop(){
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(16);
 
  if (Serial.available()){ serialColor = Serial.readString(); if(lastSerialColor != serialColor) gHue = serialColor.toInt(); Serial.println("DOSTAL SOM FARBU"); gHueEnabled = false; }
  

  EVERY_N_MILLISECONDS(100) { if(gHueEnabled) gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(120) {  nextPattern(); } // change patterns periodically
  EVERY_N_SECONDS(60) { returnToAutohue(); }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void returnToAutohue(){  if(serialColor == lastSerialColor) {gHueEnabled = true; Serial.println("IDEM NA AUTOHUE");} lastSerialColor = serialColor; }

void nextPattern(){ gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); }

void rainbow() {
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue, 16 + random8(200), 100 + random8(100));
}

void sinelon(){
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = beatsin16( 5, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

int fader = 0;

void blwhite(){
  CRGBPalette16 palette = OceanColors_p;
  int pos = beatsin16(5, 0, NUM_LEDS);
  leds[pos] = ColorFromPalette(palette, 0+random8(255), 50+random8(100));
  if (fader >= 2){
     fadeToBlackBy(leds, NUM_LEDS, 1);
     fader = 0;
  }else{
     fader++;
  }
}
