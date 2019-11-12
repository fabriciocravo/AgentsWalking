#include <SoftwareSerial.h>

SoftwareSerial xbee(3, 2);
int link = 0;
char letter;
String Message = "";
int OKcounter = 0;

void setup() {       
     
  Serial.begin(9600); 
  delay(100);

  Serial.println("Starting Setup Procedure");
  
  while( link == 0 ) {
    
    if( Serial.available() ) {
       letter = Serial.read();
       if( letter == 83 )
          link = 1;
    }
  }

  
  
  Serial.println("Got Here");

  //xbee.begin(9600);
  xbee.begin(57600);
  delay(1100);
  xbee.print("+++");
  delay(1000);
  
  while(xbee.available()) {
     letter = xbee.read();
     Message = Message + letter;
  }
  if( Message[0] == 79 and Message[1] == 75 ) {
     PrepareForNetwork();
  }
  else {
     Serial.println("It appears the Arduino is already ready for the Network");
     Serial.println("Please see the troubleshooting document if problems still persist");
  }
  Serial.println("Setup Finished");

}

void PrepareForNetwork() {
  
  Serial.println("Preparing the Arduino for the Network");
  
  xbee.println("ATID FAFA");
  delay(100);

  xbee.println("ATDH 0");
  delay(100);

  xbee.println("ATDL FFFF");
  delay(100);

  //xbee.println("ATBD E100");
  xbee.println("ATBD A");
  delay(100);

  xbee.println("ATNH 00");
  delay(100);
  
  xbee.println("ATWR");
  delay(100);
  
}



void loop() {
  
}
