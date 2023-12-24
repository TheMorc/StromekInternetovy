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
  Serial.print("WELCOME TO UNOSTROMEK - INTERNETOVY STROMEK PROJEKT BISKUPOVA");
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {fade, rainbow, sinelon, still};


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gSat = 0;
uint8_t gRPiSat = 200;

int serialColor, currColor, lastColor;
bool gHueEnabled = false;

void loop(){
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/75);
 
  while (Serial.available() > 0) {
    char receivedByte = Serial.read();
    serialColor = (serialColor * 10) + (receivedByte - '0');
  }

  if (serialColor != 0){
     Serial.print("NOVA FARBA: ");
     Serial.print(serialColor);
     currColor = serialColor;
     serialColor = 0;
  }
  
  if(lastColor != currColor) {
     if (currColor == 437) { //W_
        gRPiSat = 0;
     }else if (currColor == 387){ //R_
        gHue = 65;
        gRPiSat = 60;
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
        gRPiSat = 200;
     }
  }
  
  if (gRPiSat != 0)
     gSat = gRPiSat + random8(55);
  else
     gSat = 0;
  
  EVERY_N_SECONDS(120) { Serial.print("MENIM PATTERN"); nextPattern(); } // change patterns periodically 
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern(){ gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns); }

int slowdown = 0;
int pos = 0;

void rainbow() {
  fadeToBlackBy( leds, NUM_LEDS, 1);
  leds[pos] += CHSV(gHue, gSat, 50 + random8(100));
  if (slowdown > 3){
     slowdown = 0;
     pos = random16(NUM_LEDS);
  }
  slowdown++;
}

void sinelon(){
  fadeToBlackBy( leds, NUM_LEDS, 1);
  int pos = beatsin16( 5, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, gSat, 50 + random8(100));
}

void still() {
  pos = random16(NUM_LEDS);
  leds[pos] = CHSV( gHue, gSat, 100 + random8(100));
}

int fadeState = 0;
int fadeDirection = 1;

void fade(){
   for(int i; i < NUM_LEDS; i++){
      if (fadeDirection == 1)
          leds[i] += CHSV( gHue, gSat, 1);
      else
          leds[i] -= CHSV( gHue, gSat, 1);
   }
   
   if (fadeState >= 254)
	   fadeDirection = -1;
   if (fadeState <= 128)
       fadeDirection = 1;
 
   fadeState = fadeState + fadeDirection;
}
