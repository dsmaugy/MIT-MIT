#include <Adafruit_NeoPixel.h>
#include <XBee.h>
#include <SoftwareSerial.h>
#define DRUM1 0 //Analog sensor 1
int drumSens = 600;
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
 
SoftwareSerial mySerial(2, 3); // RX, TX
SoftwareSerial xbeeSoftSerial(A0, A1);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

//Xbee vars
int statusLed = 11;
int errorLed = 12;
int dataLed = 10;
int dataCorrectLed = 13;

//MIDI vars
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator

int valA = 0;


uint8_t option = 0;
uint8_t data = 0;



void setup() {
   Serial.begin(9600);
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);
  
  // start serial
  Serial.begin(19200);
  xbeeSoftSerial.begin(9600);
  xbee.setSerial(xbeeSoftSerial);
  
  //MIDI stuff
  mySerial.begin(31250);

  //Reset the VS1053 playCNote(74, 100);
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  //talkMIDI(0xB0, 0x07, 127); //0xB0 is channel message, set channel volume to near max (127)
  Serial.print("Serial started");
}

// continuously reads packets, looking for RX16 or RX64
void loop() {
    int sensorValue = analogRead(DRUM1);
    if(sensorValue < 650) {
    int y = map(sensorValue, 650, 300, 255, 0);
    allOn(strip.Color(255 - y, y, 0));
  } else {
    allOff();
  }
   delay(5);

   readInputs();

   if(valA < drumSens) {
      playBNote(4, 120);
      delay(100);
    } else if (valA > drumSens) {
      noteOff(0, 59, 120);
      readInputs();
      }
  
}
void allOn(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void allOff() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
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
  
void readInputs() {
  valA = analogRead(DRUM1);  // Read the voltage
  Serial.println(valA, DEC); // Print the voltseSerial.println(valAHex);tage to the terminal
}



//MIDI note declaration
void playANote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 57, noteVelocity);
  delay(duration);
  noteOff(0, 57, noteVelocity);
}
  
void playBNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  if(valA > drumSens){
    //noteOff(0, 59, noteVelocity);
    readInputs();
  }
  readInputs();
  noteOn(0, 59, noteVelocity);
}

void playCNote(byte instrument) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  //talkMIDI(0xB0, 12, )
  //setVolume();

  //Volume setting stuff
  if (valA < drumSens) {
    do { 
      readInputs();
      noteOn(0, 60, 120);
      } while(valA < drumSens);
    } else {
        noteOff(0, 60, 120);
      }
}

void playDNote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 62, noteVelocity);
  delay(duration);
  noteOff(0, 62, noteVelocity);
}

void playENote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 64, noteVelocity);
  delay(duration);
  noteOff(0, 64, noteVelocity);
}

void playFNote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 65, noteVelocity);
  delay(duration);
  noteOff(0, 65, noteVelocity);
}

void playGNote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 67, noteVelocity);
  delay(duration);
  noteOff(0, 67, noteVelocity);
}



void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
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




