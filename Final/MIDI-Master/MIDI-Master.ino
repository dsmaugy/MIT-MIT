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

int noteMode = 1;
//Note Mode 1: C, E, G, B Flat
//Note Mode 2: F, A, C, E Flat

bool isPlaying = false;

void setup() {

  Serial.begin(9600);    
  Serial.println("Serial started");
  
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
          if (data != (uint8_t)0) {
              lastNotePlayed = data;
          }
        } else {
          xbee.getResponse().getRx64Response(rx64);
          option = rx64.getOption();
          data = rx64.getData(0);
        }
      } else {
          //Weird thing happened 
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
    }
//F, A, C, D#
  switch(data) {

  case (uint8_t) 0:
    isPlaying = false;
    Serial.println(lastNotePlayed);
    arbitNoteOff(lastNotePlayed);
    break;
  
  case (uint8_t) 1:
    if (!isPlaying) {
      if (noteMode == 1) {
          Serial.println("C NOTE PLAYED");
          playCNote(4, 120);
          isPlaying = true;
        } else if (noteMode == 2) {
          Serial.println("F FLAT NOTE PLAYED");
          playFNote(4, 120);
          isPlaying = true;
         }
      }
    break;
    
  case (uint8_t) 2:
    if (!isPlaying) {
      if (noteMode == 1) {
          Serial.println("E NOTE PLAYED");
          playENote(4, 120);
          isPlaying = true;
        } else if (noteMode == 2) {
          Serial.println("A NOTE PLAYED");
          playANote(4, 120);
          isPlaying = true;
          break;
         }
      }
    break;
    
  case (uint8_t) 3:
    if (!isPlaying) {
      if (noteMode == 1) {
          Serial.println("G NOTE PLAYED");
          playGNote(4, 120);
          isPlaying = true;
        } else if (noteMode == 2) {
          Serial.println("C NOTE PLAYED");
          playCNote(4, 120);
          isPlaying = true;
         }
      }
    break;
    
  case (uint8_t) 4:
    if (!isPlaying) {
      if (noteMode == 1) {
          Serial.println("B FLAT NOTE PLAYED");
          playBFlatNote(4, 120);
          isPlaying = true;
        } else if (noteMode == 2) {
          Serial.println("E FLAT NOTE PLAYED");
          playEFlatNote(4, 120);
          isPlaying = true;
         }
      }
    break;
  }
} 

void arbitNoteOff(uint8_t whichNote) { //Arbitrary note off
  switch (whichNote) {
    case (uint8_t) 1:
      if(noteMode == 1) {
        noteOff(0, 60, 120);
      } else if (noteMode == 2) {
        noteOff(0, 65, 120);
        }
      break;
    case (uint8_t) 2:
      if(noteMode == 1) {
        noteOff(0, 64, 120);
      } else if (noteMode == 2) {
        noteOff(0, 57, 120);
        }
      break;
    case (uint8_t) 3:
      if(noteMode == 1) {
        noteOff(0, 67, 120);
      } else if (noteMode == 2) {
        noteOff(0, 60, 120);
        }
      break;
    case (uint8_t) 4:
      if(noteMode == 1) {
        noteOff(0, 70, 120);
      } else if (noteMode == 2) {
        noteOff(0, 63, 120);
        }
      break;
     
   }
}

void playAFlatNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 68, noteVelocity);
}

void playANote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 57, noteVelocity);
}

void playBFlatNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 70, noteVelocity);
}
  
void playBNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 59, noteVelocity);
}


void playCNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 60, noteVelocity);
}

void playDNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 62, noteVelocity);
}

void playEFlatNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 63, noteVelocity);
}

void playENote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 64, noteVelocity);
}

void playFNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 65, noteVelocity);
}

void playGNote(byte instrument, byte noteVelocity) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
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






