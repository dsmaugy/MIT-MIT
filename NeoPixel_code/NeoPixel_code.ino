#include <Adafruit_NeoPixel.h> 
#define PIN 6 // able to change later
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800); 
void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  )
}

void loop() {
  // put your main code here, to run repeatedly:
 strip.setPixelColor(0, 255, 0, 255);
  strip.setPixelColor(1, 255, 0, 255);
  strip.setPixelColor(2, 255, 0, 255);
  strip.setPixelColor(3, 255, 0, 255);
  strip.setPixelColor(3, 255, 0, 255);
  strip.setPixelColor(5, 255, 0, 255);
  strip.setPixelColor(6, 255, 0, 255);
  strip.setPixelColor(7, 255, 0, 255);
  strip.setPixelColor(8, 255, 0, 255);
  strip.setPixelColor(9, 255, 0, 255);
}
