#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(2, 3); //RX, TX
char message;
 
void setup()  {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  Serial.println("Goodnight moon!");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Hello, world?");
}
 
 
 
void loop()                     // run over and over again
{
 
  if (mySerial.available()) {
      Serial.print((char)mySerial.read());
      message = (char)mySerial.read();
  }
  if (Serial.available()) {
      mySerial.print((char)Serial.read());
  }

//  if(message == (int)2) {
    //  digitalWrite(13, HIGH);
    } 
 // else if (message ==(int) 3){ 
 // digitalWrite(13, LOW);
// }
// delay(100);
 
  
//}
