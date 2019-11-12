// A20B30C50D234E28F10G
#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <SPI.h>
#include <SD.h>
SoftwareSerial xbee(3, 2);

//NODE INFORMATION/////////////////////////////////////////////////////
String NodeNumber = "c";
float A = 1;
//////////////////////////////////////////////////////////////////////
//Experiment INFO/////////////////////////////////////////////////////
// Here we have the experiment info
// RestTime is the resting period in each network cycle, Period is the Network Period
// Byte number is the number of Bytes in each message on the network
// BegginingTime is the millisecond time the network will start it's cycle!
long RestTime = 250000, Period = 5000000;
long InitialRestTime = 2000000;
int ByteNumber = 2, ByteTrack = 0;
//////////////////////////////////////////////////////////////////////

File FileM;
String Message;
char letter, SendLetter;
String Time;
int link = 0;
int TimeSet = 0;
int SentMessageCounter = 0;

int StartMessage = 0, EndMessage = 0;
int MessagePart;
String readHour, readMinute, readSecond, readMillis, readDay, readMonth;
String readMNumber, readNode, readMTime;

long TotalTime, SetAt, SetMilli, SetCycle2;
float SetCycle;

long RTime;
int flag = 0, startSending = 0;
volatile int flagSendMessage = 0;

long CyclicByteDebbug, CyclicByteDebbug2;
char CyclicByteDebbug3;

void setup() {
  // Connection with the python code
  // The arduino reeatly prints "Arduino" if the python code finds this word it responds
  // When the arduino has responded with Ok the python script and arduino are officially conected             
  Serial.begin(115200); 
  xbee.begin(57600);

  while(link == 0)
  {
    Serial.println("Arduino");
    delay(100);

    if(Serial.available())
    {
      link = 1;
      Serial.println("Ok");
    }
  }

 // Here the time is set, there isn't much worry about the offset since it is assumed to be the same for all members of the network
 // Since they will be all using the same code
 // The time comes in the format AHourBMinuteCSecondDMillisecondE
 // But the time is only stored in MilliSeconds 
 while( TimeSet != 7) {
    while(Serial.available()) {    
        letter = Serial.read();
        if( TimeSet == 1 && letter != 66 ) { readHour = readHour + letter; }
        if( TimeSet == 2 && letter != 67 ) { readMinute = readMinute + letter; }
        if( TimeSet == 3 && letter != 68 ) { readSecond = readSecond + letter; }
        if( TimeSet == 4 && letter != 69 ) { readMillis = readMillis + letter; }
        if( TimeSet == 5 && letter != 70 ) { readDay = readDay + letter; }
        if( TimeSet == 6 && letter != 71 ) { readMonth = readMonth + letter; }
        if( letter == 65 ) { TimeSet = 1;  SetAt = millis(); }
        if( letter == 66 ) { TimeSet = 2; }
        if( letter == 67 ) { TimeSet = 3; }
        if( letter == 68 ) { TimeSet = 4; }
        if( letter == 69 ) { TimeSet = 5; }
        if( letter == 70 ) { TimeSet = 6; }
        if( letter == 71 ) { TimeSet = 7; }
        
    }
    if( TimeSet == 7 ) {
        SetMilli = readHour.toInt()*60*60*1000 + readMinute.toInt()*60*1000 + readSecond.toInt()*1000 + readMillis.toInt();  
    }
  }

  // Here we start the drift constant mesuring proceadure 
  // The arduino sends it's time to the python script waits 5 minutes and them sends it's time again 
  // The python script takes note of when it has received both of those messages and uses the difference to calculate the TimeDrift
  letter = "";
  link = 0;
  while(link == 0) {
    if(Serial.available()) {
        letter = Serial.read();
        if( letter == 84 ) { 
          Serial.println(millis()); 
          link = 1;
        }
    }
  }
  
  //delay(300000); 
  delay(2000);
  
  letter = "";
  link = 0;
  while(link == 0) {
    if(Serial.available()) {
        letter = Serial.read();
        if( letter == 84 ) { 
          Serial.println(millis()); 
          link = 1;
        }
    }
  }


  //Serial.println("Drift Measured");
  // Here the python code returns the Drift in order to be used by the Arduino
  delay(5000);
  letter = "";
  link = 0;
  //delay(2000);
  while(link == 0) {
    if(Serial.available()) {
        A = Serial.parseFloat();
        link = 1;
    }
  }

  //Serial.print("The final read is: ");
  //Serial.println(A, 7);
  
  // Now armed with the Drift we set the period cycle
  // We need to convert arduino time to real time
  Period = Period/A;
  RestTime = RestTime/A;
  InitialRestTime = InitialRestTime/A;
  
  // We must start the SD card right now

  /*
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  */

  //FileM = SD.open( NodeNumber + readDay + readMonth +".txt", FILE_WRITE);

  
  while( link >= 4) {
    link = TheTime()%5000;
  }
  RTime = random(InitialRestTime, Period - RestTime);
  Timer1.initialize(RTime);
  Timer1.attachInterrupt(IAMTHELAW);  

  SetCycle = SetMilli/(64.0*5000.0);
  SetCycle2 = (SetCycle - round(SetCycle - 0.4999))*64*5000;
  SentMessageCounter = round(SetCycle2/5000.0 - 0.4999);
  SendLetter = SentMessageCounter + 33;


}


long TheTime() {
  TotalTime = SetMilli + long(A*millis()) - A*SetAt;
  return TotalTime;
}

void sendData() {
  
  Time = TheTime();
  Message = NodeNumber + SendLetter;
  Serial.println("S: " + Message + " Time: " + Time + " " );
  //FileM.println("S: " + Message + " Time: " + Time + " " );
  //FileM.flush();
  xbee.print(Message);

}

void readData() {
  // Here we read incoming data
  // All messages must contain a node identifier and the current byte their are cycling thorugh 
  // So if the start byte is a non capital letter, it is a message stater 
  // We use the fact that messages have a predetermined number of bytes in order to finish reading them
  // As a message is read we store it in the SimCard
  while(xbee.available()) {
      
      letter = xbee.read();

      if( letter >= 97 && letter <= 122) {
        readNode = "";
        ByteTrack = 1;
        readNode = readNode + letter;
        Time = TheTime();
      }

      if( letter >= 33 && letter <= 96 && ByteTrack >= 1) {
        readNode = readNode + letter;
        ByteTrack = ByteTrack + 1;
        if( ByteTrack == ByteNumber ) { 
              ByteTrack = 0;
              Serial.println("R: " + readNode + " Time: " + Time + " " );
              //FileM.println("R: " + readNode + " Time: " + Time + " " );
              //FileM.flush();
          }
         
      }
  }
  
}  

void loop() {

  if(xbee.available()) { readData(); }
  if( flagSendMessage == 1 ) {
    SentMessageCounter = SentMessageCounter + 1;
    SendLetter = SentMessageCounter%64 + 33;
    sendData();
    flagSendMessage = 0;
  }
  
}

void IAMTHELAW() {
  // Here we have the message sending function
  // Selecting a random uniform period to transmit 
  // And keeping count of the Cycle Number and the current subcycle due to drifting issues 
  flag = (flag + 1)%2;
  if(flag == 0) {
    flagSendMessage = 1;
    Timer1.setPeriod(Period - RTime);
  }
  if(flag == 1) {
    RTime = random(RestTime, Period - RestTime);
    Timer1.setPeriod(RTime);
  }
}
