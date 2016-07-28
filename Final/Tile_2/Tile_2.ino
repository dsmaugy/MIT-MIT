  

//Library inclusions
#include <XBee.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

//Neopixel thing
#ifdef __AVR__
#include <avr/power.h>
#endif

//Constants
#define PIN 9
#define DRUM1 0 //Analog sensor 1

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs) 
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRBW + NEO_KHZ800);





/***************************\
*                           *
*      Xbee Variables       *
*                           *
*                           *
\***************************/
//Debug lights
int statusLed = 11; 
int errorLed = 12;
int dataLed = 10;
int dataCorrectLed = 13;
uint8_t payloadOn[] = {0};
uint8_t payloadOff[] = {0};
uint8_t payloadNeutral[] = {0};




//Object initializations
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();
Tx16Request txNoteOn = Tx16Request(0x1874, payloadOn, sizeof(payloadOn));
Tx16Request txNoteOff = Tx16Request(0x1874, payloadOff, sizeof(payloadOff));
Tx16Request txNoteNeutral = Tx16Request(0x1874, payloadNeutral, sizeof(payloadNeutral));

//Serial initialization



SoftwareSerial xbeeSerial(2, 3); // RX, TX 





/***************************\
*                           *
*    Velostat Variables     *
*                           *
*                           *
\***************************/
int drumSens = 700;
int drumMinimum = 550;
int valA = 0; //Velostat value
uint32_t currentLED;
bool isPlaying = false;




/***************************\
*                           *
*    Neopixel Variables     *
*                           *
*                           *
\***************************/

int fadeTime = 1000;
int fadeSteps = 10;


void setup() {
  //Hardware serial start
  Serial.begin(9600);
  Serial.print("Serial started");

  //Neopixel LED Code
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'


  //Xbee setup
  pinMode(statusLed, OUTPUT); 
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);
  xbee.setSerial(Serial); //Asign software serial port 
  spiralOn();
  payloadOn[0] = 2;
  payloadOff[0] = 6;
  payloadNeutral[0] = 10;
}


void loop() {
  //Neopixel LED code
  //Serial.println(isTriggered());
  readInputs();
  if(isTriggered()) {
    int y = map(valA, drumSens, drumMinimum, 255, 0);
    y = constrain(y, 0, 255);
    allOn(strip.Color(255 - y, y, 0));
    //Serial.println(y);
    currentLED = strip.Color(255 - y, y, 0);
  } else {
      allOff();
  }
    delay(5);

  //Play note through velostat TODO: Fix rapid note playing issue 
  if(valA < drumSens && !isPlaying) { //If held down
      xbee.send(txNoteOn);
      isPlaying = true;
      readInputs();
     
  } else if (valA > drumSens && isPlaying) { //If released
     xbee.send(txNoteOff);
     Serial.println("Note just released");
     isPlaying = false;
     readInputs();
  } else {
      //xbee.send(txNoteNeutral);
    }
}
     

/***************************\
*                           *
*    Neopixel Functions     *
*                           *
*                           *
\***************************/
void allOn(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.setBrightness(90);
  strip.show();
}

void pixelDefault() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(255, 250, 250));
  }
  strip.setBrightness(20);
  strip.show();  
}

void allOff() {
  allOn(0x000000);
}

int spiralOn() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, 255 - i, 255 - i, 255 - i);    
       strip.show();
       delay(30);       
    }
    allOff();
}


/***************************\
*                           *
*     Velostat Reading      *
*                           *
*                           *
\***************************/
void readInputs() {
  valA = analogRead(DRUM1);  // Read the voltage
  Serial.println(valA, DEC); // Print the voltseSerial.println(valAHex);tage to the terminal
}

bool isTriggered() {
  readInputs();
  if(valA < drumSens) {
    return true;
  }
  return false;
}




