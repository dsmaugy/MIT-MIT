#include <XBee.h>
#include <Printers.h>
#include <SoftwareSerial.h>
#define MAX_FRAME_DATA_SIZE 110
SoftwareSerial xbeeSerial(2, 3);
uint8_t data;

XBee xbeeObj = XBee();

Rx16Response rx16 = Rx16Response();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600); //HW Serial 
  xbeeSerial.begin(9600);
  xbeeObj.setSerial(xbeeSerial);
}

void loop() {
  // put your main code here, to run repeatedly:
  xbeeObj.readPacket(100);

  if (xbeeObj.getResponse().isAvailable()) {
      if (xbeeObj.getResponse().getApiId() == RX_16_RESPONSE) {
          xbeeObj.getResponse().getRx16Response(rx16);
          data = rx16.getData(0);
          Serial.print(data);
          
          rx16.getRemoteAddress16();
          rx16.getRssi();

          if (xbeeObj.getResponse().isError()) {
              xbeeObj.getResponse().getErrorCode();
          }

          for (int i = 0; i < rx16.getDataLength(); i++) {
            Serial.println(rx16.getData(i));
          }
        }
    }

  
}
