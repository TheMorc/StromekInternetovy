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
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  Serial.println("WELCOME TO UNOSTROMEK - INTERNETOVY STROMEK PROJEKT BISKUPOVA");
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { fade, rainbow, sinelon, still };


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gSat = 255;

int serialColor, currColor, lastColor;
bool gHueEnabled = false;

void loop(){
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/60);
 
  while (Serial.available() > 0) {
    char receivedByte = Serial.read();
    serialColor = (serialColor * 10) + (receivedByte - '0');
  }

  if (serialColor != 0){
     Serial.print("NOVA FARBA: ");
     Serial.println(serialColor);
     currColor = serialColor;
     serialColor = 0;
  }
  
  if(lastColor != currColor) {
     if (currColor == 437) { //W_
        gSat = 0;
     }else if (currColor == 2140){ //BR0
        FastLED.setBrightness(0);
     }else if (currColor == 2141){ //BR1
        FastLED.setBrightness(32);
     }else if (currColor == 2142){ //BR2
        FastLED.setBrightness(128);
     }else if (currColor == 2143){ //BR3
        FastLED.setBrightness(255);
     }else{
        gHue = currColor;
        gSat = 255;
     }
    gHueEnabled = false;
  }

  EVERY_N_MILLISECONDS(100) { if(gHueEnabled) gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(120) { Serial.println("MENIM PATTERN"); nextPattern(); } // change patterns periodically
  //EVERY_N_SECONDS(300) { returnToAutohue(); }
  
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void returnToAutohue(){  if(currColor == lastColor) { gHueEnabled = true; gSat = 255; Serial.println("IDEM NA AUTOHUE");} lastColor = currColor; }

void nextPattern(){ gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); }

int slowdown = 0;

void rainbow() {
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = random16(NUM_LEDS);
  if (slowdown > 1){
     slowdown = 0;
     leds[pos] += CHSV( gHue, 100 + random8(155), 100 + random8(100));
  }
  slowdown++;
}

void sinelon(){
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = beatsin16( 5, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, gSat, 192);
}

void still() {
  int pos = random16(NUM_LEDS);
  leds[pos] = CHSV( gHue, 200 + random8(55), 100 + random8(100));
}

int fadeState = 0;
int fadeDirection = 1;

void fade(){
   for(int i; i < NUM_LEDS; i++){
      leds[i] = CHSV( gHue, gSat, fadeState);
   }
   
   if (fadeState >= 254)
	   fadeDirection = -1;
   if (fadeState <= 128)
       fadeDirection = 1;
 
   fadeState = fadeState + fadeDirection;
}
