

//Library inclusions
#include <XBee.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

//Neopixel thing
#ifdef __AVR__
#include <avr/power.h>
#endif

//Constants
#define PIN 8
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);





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

//Incoming data vars
uint8_t option = 0;
uint8_t data = 0; 

//Object initializations
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

//Serial initialization
SoftwareSerial xbeeSoftSerial(A0, A1); // RX, TX




/***************************\
*                           *
*      MIDI Variables       *
*                           *
*                           *
\***************************/
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
bool isPlaying = false;
SoftwareSerial mySerial(2, 3); // RX, TX 





/***************************\
*                           *
*    Velostat Variables     *
*                           *
*                           *
\***************************/
int drumSens = 520;
int drumMinimum = 350;
int valA = 0; //Velostat value
uint32_t currentLED;




void setup() {
  //Hardware serial start
  Serial.begin(19200);
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
  xbeeSoftSerial.begin(9600); //Xbee serial start
  xbee.setSerial(xbeeSoftSerial); //Asign software serial port 
  
  
  
  
  //MIDI setup
  mySerial.begin(31250); //Serial start
  //Reset the VS1053 playCNote(74, 100);
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 127); //Set channel volume to 127
  fadeOff();
  playCNote(70, 120);
  delay(500);
  noteOff(0, 60, 120);
}


void loop() {
  //Neopixel LED code
  int sensorValue = analogRead(DRUM1);
  if(sensorValue < drumSens) {
    int y = map(sensorValue, drumSens, drumMinimum, 255, 0);
    y = constrain(y, 0, 255);
    allOn(strip.Color(255 - y, y, 0));
    //Serial.println(y);
    currentLED = strip.Color(255 - y, y, 0);
  } else {
      pixelDefault();
  }
    delay(5);

  //Play note through velostat TODO: Fix rapid note playing issue 
  readInputs(); //Start reading velostat value
  if(valA < drumSens) { //If held down
    if(!isPlaying) {
      playBNote(4, 120);
      isPlaying = true;
      //Serial.println(valA);
      }
  } else if (valA > drumSens) { //If released
     noteOff(0, 59, 120);
     isPlaying = false;
     readInputs();
  }
     
/*switch(data) {
  
  case (uint8_t) 1:
    playBNote(4, 120, 500);
    break;
    
  case (uint8_t) 2:
    //blah
    break;
    
  case (uint8_t) 3:
    //blah
    break;
    
  case (uint8_t) 4:
    //blah
    break;
    
  case (uint8_t) 5:
    //blah
    break;
    
  case (uint8_t) 6:
    //blah
    break;
  case (uint8_t) 7:
    //blah
    break;
  case (uint8_t) 8:
    //blah
    break;
  } */
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
  strip.setBrightness(30);
  strip.show();  
}

void allOff() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
}

int fadeOff() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, 255 - i, 255 - i, 255 - i);    
       strip.show();
       delay(20);       
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



/***************************\
*                           *
*   MIDI Note Declarations  *
*                           *
*                           *
\***************************/
void playANote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //Useless
  }
  
  readInputs();
  noteOn(0, 57, noteVelocity);
}
  
void playBNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //Useless
  }
  
  readInputs();
  noteOn(0, 59, noteVelocity);
}

void playCNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //uselss
  }
  
  readInputs();
  noteOn(0, 60, noteVelocity);
}

void playDNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //uselss
  }
  
  readInputs();
  noteOn(0, 62, noteVelocity);
}

void playENote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //uselss
  }
  
  readInputs();
  noteOn(0, 64, noteVelocity);
}

void playFNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //uselss
  }
  
  readInputs();
  noteOn(0, 65, noteVelocity);
}

void playGNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    readInputs(); //uselss
  }
  
  readInputs();
  noteOn(0, 67, noteVelocity);
}



/***************************\
*                           *
*    MIDI Backend Stuff     *
*                           *
*                           *c
\***************************/

//Play a note
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Release a note
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Sends commands to MIDI chip
void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

  digitalWrite(ledPin, LOW);
}




