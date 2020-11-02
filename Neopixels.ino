#include <Adafruit_NeoPixel.h>
#define LED_PIN           3
#define NUM_NOTES         5
#define NUM_LEDS_PER_HIT  4
#define NUM_LEDS          20


// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


//how bright are all the pixels
float pixelBright[NUM_LEDS];

//how fast to dim the pixels
float pixelDimSpeed = 0.11;

//default color of the pixels
uint32_t color;

//how long should it take to dim the pixels
int pixelTimeOut = 100;

LPFilter pixelBrightFilter[NUM_NOTES];

void initPixels() {
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(250); // Set BRIGHTNESS to about 1/5 (max = 255)


  for (int i = 0; i < strip.numPixels(); i ++) {
    strip.setPixelColor(i, strip.Color(100, 255, 10));
    strip.show();
    delay(50);
  }
  delay(100);
  for (int i = 0; i < strip.numPixels(); i ++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();

  color = strip.Color(50, 120, 250);

}

void updatePixels() {
  for (int i = 0; i < NUM_LEDS; i++){
    uint8_t filterIndex = i / NUM_LEDS_PER_HIT;
    pixelBright[filterIndex] = pixelBrightFilter[filterIndex].update(pixelBright[filterIndex], 0.8);
    
    if (pixelBright[filterIndex] > 0) {
      pixelBright[filterIndex] -= pixelDimSpeed;
      if (pixelBright[filterIndex] < 0){
        pixelBright[filterIndex] = 0;
      }
      
      Serial.print("LED" + String(i) + ",");
      Serial.println(pixelBright[filterIndex]);
    }

    if (pixelBright[filterIndex] > 1){
      pixelBright[filterIndex] = 1;
    }
  
    strip.setPixelColor(i, strip.Color(50 * pixelBright[filterIndex], 120 * pixelBright[filterIndex], 250 * pixelBright[filterIndex]));
  }
  strip.show();
}

void pixelHit(int i) {
  pixelBrightFilter[i].force(1);
  pixelBright[i] = 1;
}
