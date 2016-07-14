#include <SoftwareSerial.h>

#define DRUM1 0 //Analog sensor 1
int drumSens = 530;

SoftwareSerial mySerial(2, 3); // RX, TX
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
int valA = 0;
#define piezoPin 1
void setup() {

  Serial.begin(57600);

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
  //noteOn(0, 60, 127);
}

void loop() {
  // put your main code here, to run repeatedly:
  /**int piezoValue = analogRead(piezoPin);
  if (piezoValue > 1000) {
    playCNote(5, 120, 500);
    } **/
  //Serial.println(piezoValue);
  readInputs();
  playCNote(3);
} 

void playANote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 57, noteVelocity);
  delay(duration);
  noteOff(0, 57, noteVelocity);
}
  
void playBNote(byte instrument, byte noteVelocity, int duration) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  noteOn(0, 59, noteVelocity);
  delay(duration);
  noteOff(0, 59, noteVelocity);
}

void playCNote(byte instrument) {
  talkMIDI(0xC0, instrument, 0x00); //Instrument Change
  //talkMIDI(0xB0, 12, )
  //setVolume();

  //Volume setting stuff
  int volRange = (drumSens - valA);
  int desiredVolume = map(volRange, 0, 500, 0, 127);
  int realDesiredVolume = constrain(desiredVolume, 0, 127);
  
  if(valA < drumSens) {
    readInputs();
    noteOn(0, 60, realDesiredVolume);
  } else {
    noteOff(0, 60, realDesiredVolume);
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


/** void maryLamb() {
  playENote(5, 120, 500);
  playDNote(5, 120, 500);
  playCNote(5, 120);
  playDNote(5, 120, 500);
  playENote(5, 120, 500);
  playENote(5, 120, 500);
  playENote(5, 120, 500);
  
  playDNote(5, 120, 500);
  playDNote(5, 120, 500);
  playDNote(5, 120, 500);
  playENote(5, 120, 500);
  playGNote(5, 120, 500);
  playGNote(5, 120, 500);

  playENote(5, 120, 500);
  playDNote(5, 120, 500);
  playCNote(5, 120);
  playDNote(5, 120, 500);
  playENote(5, 120, 500);
  playENote(5, 120, 500);
  playENote(5, 120, 500);

  playENote(5, 120, 500);
  playDNote(5, 120, 500);
  playDNote(5, 120, 500);
  playENote(5, 120, 500);
  playDNote(5, 120, 500);
  playCNote(5, 120);
  
} **/

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

void readInputs() {
  valA = analogRead(DRUM1);  // Read the voltage
  Serial.println(valA, DEC); // Print the voltseSerial.println(valAHex);tage to the terminal
}

void setVolume() {
  int desiredVolume = 0;
  int volRange = (drumSens - valA);
  int volMult = (volRange * 50);
  desiredVolume = map(volMult, 0, drumSens, 0, 16383);
  //desiredVolume = constrain(desiredVolume, 0, 16383); 
  //Serial.println("Vol: " + desiredVolume);
  talkMIDI(0xB0, 7, 100);
}




