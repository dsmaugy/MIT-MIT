#include <SoftwareSerial.h>
#include <XBee.h>

#define DRUM1 0 //Analog sensor 1
int drumSens = 530;

SoftwareSerial mySerial(2, 3); // RX, TX

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();

Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();

byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
int valA = 0;
uint8_t data = 0;
uint8_t option = 0;
uint8_t lastNotePlayed;

bool isPlaying = false;

void setup() {

  Serial.begin(9600);    
       
  xbee.setSerial(Serial);

  //Setup soft serial for MIDI control
  mySerial.begin(31250);

  //Reset the VS1053 playCNote(74, 100);
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 127); //0xB0 is channel message, set channel volume to near max (127)
  //talkMIDI(0xC0. 5, 0x00);

  //Startup check
  noteOn(0, 60, 127);
  delay(500);
  noteOff(0, 60, 120);
}

void loop() {
  // put your main code here, to run repeatedly:
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
          xbee.getResponse().getRx16Response(rx16);
          option = rx16.getOption();
          data = rx16.getData(0);
          if (data != 0) {
              lastNotePlayed = data;
            }
          Serial.print(data);
        } else {
          xbee.getResponse().getRx64Response(rx64);
          option = rx64.getOption();
          data = rx64.getData(0);
        }
        
        // TODO check option, rssi bytes    
        //flashLed(statusLed, 1, 10);
        
        // set dataLed PWM to value of the first byte in the data
        //analogWrite(dataLed, data);
      } else {
        // not something we were expecting
        //flashLed(errorLed, 1, 25);    
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
    }
    
  switch(data) {

  case (uint8_t) 0:
    arbitNoteOff(lastNotePlayed);
    break;
  
  case (uint8_t) 1:
    Serial.print("B NOTE PLAYED");
    if (!isPlaying) {
        playBNote(4, 120);
        isPlaying = false;
      }
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
  }
  
  
} 

void arbitNoteOff(uint8_t whichNote) { //Arbitrary note off
  switch (whichNote) {
    case (uint8_t) 1:
      noteOff(0, 57, 120);
      break;
    case (uint8_t) 2:
      noteOff(0, 59, 120);
      break;
    case (uint8_t) 3:
      noteOff(0, 60, 120);
      break;
    case (uint8_t) 4:
      noteOff(0, 62, 120);
      break;
     
   }
}

void playANote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //Useless
  }
  
  //();
  noteOn(0, 57, noteVelocity);
}
  
void playBNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 59, noteVelocity);
}

void playCNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //uselss
  }
  
  //();
  noteOn(0, 60, noteVelocity);
}

void playDNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //uselss
  }
  
  //();
  noteOn(0, 62, noteVelocity);
}

void playENote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //uselss
  }
  
  //();
  noteOn(0, 64, noteVelocity);
}

void playFNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //uselss
  }
  
  //();
  noteOn(0, 65, noteVelocity);
}

void playGNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  
  if(valA > drumSens){
    //(); //uselss
  }
  
  //();
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






