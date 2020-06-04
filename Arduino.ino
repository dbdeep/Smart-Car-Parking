#include <SoftwareSerial.h>
#include "SevSeg.h"
#include <Servo.h>
#define RX 10 
#define TX 11  
#define Exit 7         //Pin connected to the EXIT button.
#define In 6         //Pin connected to the IN button.
#define CAPACITY 8         //Capacity of the parking lot.

SevSeg sevseg;
int  Available= 9;

Servo myservo;  // create servo object to control a servo
Servo myservo1;

#define ServoM    12        //Connected to the servo motor.
#define ServoM1    13 


#define BarLow    177       //Low position of the barrier.
#define BarUp     95        //Up position of the barrier.

String AP = "Realme";       // CHANGE ME
String PASS = "12345678"; // CHANGE ME
String API = "W1HD7VDT8G4HC49O";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 
 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);

  myservo.attach(ServoM);          // attaches the servo.
  myservo1.attach(ServoM1);
  
  //Set to 1 for single digit display
  byte numDigits = 1;
  
  //defines common pins while using multi-digit display. Left empty as we have a single digit display
  byte digitPins[] = {};

  //Defines arduino pin connections in order: A, B, C, D, E, F, G, DP
  byte segmentPins[] = {3, 2, 8, A0, A1, 4, 5, 9};
  bool resistorsOnSegments = true;

  //Initialize sevseg object. Uncomment second line if you use common cathode 7 segment
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  //sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);


  sevseg.setBrightness(90);
  pinMode(Exit, INPUT);           // set "EXIT" button pin to input
  pinMode(In, INPUT);             // set "IN" button pin to input
  digitalWrite(Exit, HIGH);       
  digitalWrite(In, HIGH); 
  myservo.write(BarLow); 
  myservo1.write(BarLow); 
  
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() 
{
Display(Available);

 valSensor = Available;
 
if(digitalRead(In)==0)
{
  if(Available != 0){
    Available--;
    myservo.write(BarUp);
    delay(3000);
    myservo.write(BarLow);
    }
  }
if(digitalRead(Exit)==0)
{
  if(Available != CAPACITY){
    Available++;
    myservo1.write(BarUp);
    delay(3000);
    myservo1.write(BarLow);
    }
  }



 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

 /*-------------------------------------------------------------------
Put the segments according to the number.
--------------------------------------------------------------------*/
void Display(int number)
{

  sevseg.setNumber(number);
  sevseg.refreshDisplay(); 


}