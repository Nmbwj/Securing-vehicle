#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_Fingerprint_Due.h"
#include <Scheduler.h>
#define SAD 10
#define RST 5
#define BUTTONPIN 5
#define STARTENGINE 7
#define ENGINECUT 8

MFRC522 nfc(SAD, RST);
Adafruit_Fingerprint_Due finger = Adafruit_Fingerprint_Due();

// RFID CARD
byte samuelTag[5] = {0x88, 0x4, 0x16, 0xDA};
byte naolTag[5] = {0x5D, 0xE5, 0xA2, 0x82}; 
byte testTag[5] = {0x13, 0xC1, 0x90, 0xFC}; 

// FINGER PRINT ID
uint16_t mohamedid[] ={1,2};
uint16_t naolid[] ={5,6};

byte status;
byte data[MAX_LEN];
byte serial[5];

bool rfidVerified = false;
bool fingerprintVerified = false;

bool messageReceived = false;
String incomingMessage = "";
String message;
String compareValue;
String convertedValue;
int on=1;
int cutted=0;


/*
String url = "\"eu-central-1.sftpcloud.io\"";
String username = "\"4c9f4fb87fbb4c599e13cd6f637985ea\"";
String password = "\"9QajNJs9xa67guInVk75HqSl1bTtni57\"";
*/

String url = "\"49.13.205.144\"";
String username = "\"fleet\"";
String password = "\"fleetmanagement2demosamnawa\"";

String file = "\"A30.csv\"";
String file1 = "\"status.csv\"";

//int buttonPin =5;
int buttonRead =0;
int after =0, emerg=0, net=1, i=1, j=1, m=1;

//int fingerlight;

int emergecyabort =0;
void setup() {
  // Setup for Serial communication
  Serial.begin(115200);
  Serial3.begin(9600);
  
  SPI.begin();
  nfc.begin();
  byte version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.print("Didn't find MFRC522 board.");
    while (1); // halt
  }

  
  // Setup for Fingerprint sensor
  finger.begin(57600);
  pinMode(STARTENGINE, OUTPUT);
  pinMode(ENGINECUT, OUTPUT);
  //pinMode(buttonPin,OUTPUT);
  digitalWrite(STARTENGINE, HIGH);
  digitalWrite(ENGINECUT, HIGH);
  //digitalWrite(buttonPin,LOW);
  pinMode(BUTTONPIN,INPUT);

  //fingerlight =1;
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
  Scheduler.startLoop(loop4);
  
  
}

void loop() {
  //digitalWrite(22, LOW);
  //if(fingerlight)
  //{
  checkRFID();
  if(on)
    checkFingerprint();
  //}

  
  

  if (rfidVerified && fingerprintVerified) {
    Serial.println("Access Granted.");
    rfidVerified = false;
    fingerprintVerified = false;
  }
  
  delay(500); // Adjust the delay as necessary
  yield();
}

void loop2(){
  while(emerg){
    if(j){
      Serial.println("It is in Emergency, so let me finish that task first solder.");
      j=0;
    }
    yield();
  }
  j=1;
  Serial.println("Now it not in Emergency or out of Emergency so lets update the EngineCut");
  delay(300000); // 300000
  while(after || emerg){
    //Serial.println("It has been Queed by sensor or Emergency come on!.");
    yield();
    continue;
  }
  after = 1;
  setupFTP(url, username, password);
  sendGETFtp(file1);
  delay(500);
  //setupFTP(url, username, password);
  //sendGETFtp(file2);
  after = 0;
  yield(); 
}

void loop3(){
  buttonRead= digitalRead(BUTTONPIN);
  if(emergecyabort)buttonRead=0;
  if(buttonRead){
    // Your code
    emerg= 1;
    while(after){
      if(i){
        Serial.println("It has been Queed by sensor or Get from Server");
        i=0;
      }
      yield();
    }
    i=1;
    Serial.println("It is in Emergency now");
    after =1;
    Serial.println("Button is working : "+ String(buttonRead));
    message= "1, Emergency for car Plate Number: B31287 A.A 03 \n\r"; // Message content
    //file = "\"starteng.txt\""; // Temporarly changed to Starteng due to testing, the change to emergency.txt
    delay(100);
    setupFTP(url, username, password);
    sendPUTFtp(file);
    delay(500);// This is to waint until you lift up your finger.
    //file = "\"starteng.txt\"";
    //Serial.println("File : "+ file);
    after =0;
    Serial.println("It get out of Emergency now");
  }
  emerg =0;
  yield();
  delay(1500);
  emergecyabort= 0;
  
}

void loop4(){
  while(net)yield();
  Serial.println("it is on loop4");
  delay(500);
  setupFTP(url, username, password);
  sendPUTFtp(file);
  delay(500);
  after = 0;
  net=1;
  yield();
}

void updateSerialSmsRecive()
{
  while (Serial.available())
  {
    Serial3.write(Serial.read()); // Forward data to Serial3 port
  }
  while (Serial3.available())
  {
    char c = Serial3.read();
    Serial.write(c); // Forward data to Serial port

    incomingMessage += c;
    
    // Check for new message indicator +CMT:
    if (incomingMessage.indexOf("+FTPGET:2") != -1)
    {
      messageReceived = true;
      //Serial.println("Match found: +FTPGET: ");
    }
    
    // Check if the message content has started
    if (messageReceived && incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+FTPGET:2")) != -1)
    {
      int messageStart = incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+FTPGET:2")) + 15;
      
      if (incomingMessage.indexOf("stop", messageStart) != -1)
      {
        String messageContent = incomingMessage.substring(messageStart, incomingMessage.indexOf("stop", messageStart));
        
        messageContent.trim();
        //Serial.println("Match found:"+messageContent+" :\n ");
        //Serial.println("Match found income start:"+incomingMessage+" : income is done\n ");
        compareValue = messageContent;
        
        compareValue.toLowerCase();

        //Serial.println(" This is it:" + compareValue);
        // Process the message
        processMessage(messageContent);

        

        // Clear the buffers
        incomingMessage = "";
        messageReceived = false;
      }
    }
  }
}

void processMessage(String message)
{
  
  // Convert message to lowercase for case-insensitive comparison
  message.toLowerCase();
  
  // Check if the message contains "enginecut"
  if (message.indexOf("enginecut") != -1)
  {
    //digitalWrite(8, HIGH); // Set pin 2 HIGH
    emergecyabort =1;
    digitalWrite(ENGINECUT, LOW);
    //delay(3500);
    //digitalWrite(8, HIGH);
    cutted=1;
    on = 1;
    message = "Engine is Stoped!. \n If you want give access grand send \"engineon\" "; // Message content
    Serial.println(message);
    delay(100);
    

  }else if (message.indexOf("engineon") != -1){
    emergecyabort =1;
    digitalWrite(ENGINECUT, HIGH);
    cutted=0;
    on = 1;
    message ="Engine is on!."; // Message content
    Serial.println(message);
    delay(100);
     
  }else if (message.indexOf("startengine") != -1){
    //Serial3.print("Engine is started!");
    emergecyabort =1;
    digitalWrite(STARTENGINE, LOW);
    delay(800);
    digitalWrite(STARTENGINE, HIGH);
    //fingerlight=0;
    on = 0;
    message ="Engine is on and started!."; // Message content
    Serial.println(message);
    delay(100);
  }
  //Serial.println("It is proccessing banned: " + message + "");
  //compareValue =message;7
}
void checkRFID() {
  byte mainStatus;
  byte mainData[MAX_LEN];
  byte mainSerial[5];
  mainStatus = nfc.requestTag(MF1_REQIDL, mainData);
  if (mainStatus == MI_OK) {
    mainStatus = nfc.antiCollision(mainData);
    memcpy(mainSerial, mainData, 5);
    nfc.selectTag(mainSerial);
    nfc.haltTag();
    for(int i=0; i<5; i++)
      serial[i] = mainSerial[i];
    if(rfidUserCheck(serial,samuelTag,"Samuel"));
    else if(rfidUserCheck(serial,naolTag,"Naol"));
    else if(rfidUserCheck(serial,testTag,"Test"));
    else {
      Serial.println("Unknown RFID.");
      rfidVerified = false;
      digitalWrite(3, HIGH);
      delay(100);
      digitalWrite(3, LOW);
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        return;
      }
      m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      delay(100);
  

      message= "1, Unknown RFID. "+String(serial[0],HEX)+" "+ String(serial[1],HEX)+" "+String(serial[2],HEX)+" "+String(serial[3],HEX)+" Tries to start the car. \n\r"; // Message content
  
      net = 0;
      Serial.println("It has finished the sensor");
      delay(1000); 
    }
  }
}

void checkFingerprint() {
  int id = getFingerprintIDez();
  if (id != -1) {
    fingerprintVerified = true;
  }
  else {
    fingerprintVerified = false;
  }
}

// Returns -1 if failed, otherwise returns ID #

int getFingerprintIDez() {
  uint8_t p;

  p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  
  p = finger.fingerFastSearch();

  if (p != FINGERPRINT_OK) {
    Serial.println("Fingerprint do not match!");
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    
    while(after || emerg){
      if(m){
        Serial.println("It may be in Emergency or Get from the Server");
        m = 0;
      }
      yield();
      return -1;
    }
    m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    delay(100);
    message ="1, Unknown person with fingerprint Tries to start the car.\n \r";
    net = 0;
    Serial.println("It has finished the sensor");
    delay(1000); 
    return -1;
  } 
  else {

    if(fingerprintUserCheck(finger.fingerID, naolid, "Naol", "5d e5 a2 82",serial,naolTag));
    else if(fingerprintUserCheck(finger.fingerID, mohamedid, "Mohammed", "13 c1 90 fc",serial,naolTag));

    delay(1000); 
    Serial.print("Found ID #");
    Serial.print(finger.fingerID); 
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID; 
  }
}




// RFID Checking Mechanisim -------------------------------------------------------------------------------
int rfidUserCheck(byte serial[], byte mohamedTag[], String name){
  if(memcmp(serial, mohamedTag, 4) == 0 ){
    Serial.println("Tag verified as "+name+"'s RFID.");
    rfidVerified = true;
    convertedValue="";
    for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
      if (i > 0) {
        convertedValue += " ";
      }
      convertedValue += String(mohamedTag[i], HEX);
    }
    // Compare the converted value with the compareValue
    if (compareValue.indexOf(convertedValue) != -1) {
      Serial.println("Match found");
      Serial.println("This Driver is banned!");
      message ="1, "+name+" with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
    }
    else {
      Serial.println("No Banned Driver match found \n "+convertedValue+"\n");
      if(on){
        message ="1, "+name+" with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
        emergecyabort =1;
        digitalWrite(STARTENGINE, LOW);
        delay(800);
        digitalWrite(STARTENGINE, HIGH);
        on = 0;
        delay(100);
        //fingerlight=0;
      }
      else{
        if(!cutted){
          emergecyabort =1;
          digitalWrite(ENGINECUT, LOW);
          delay(1000);
          digitalWrite(ENGINECUT, HIGH);
          on = 1;
          message ="1, "+name+" with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
          delay(100);
        }
      }

    }
    while(after || emerg){
      status = nfc.requestTag(MF1_REQIDL, data);
      if (status == MI_OK) {
        status = nfc.antiCollision(data);
        memcpy(serial, data, 5);
        nfc.selectTag(serial);
        nfc.haltTag();
        Serial.println("It may be in Emergency or Get from the Server");
        //
        if(memcmp(serial, mohamedTag, 4) == 0) {
          Serial.println("Tag verified as "+name+"'s RFID.");
          rfidVerified = true;
          convertedValue="";
          for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
            if (i > 0) {
              convertedValue += " ";
            }
            convertedValue += String(mohamedTag[i], HEX);
          }
          // Compare the converted value with the compareValue
          if (compareValue.indexOf(convertedValue) != -1) {
            Serial.println("Match found");
            Serial.println("This Driver is banned!");
            //message ="1, Naol with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
          }
          else {
            Serial.println("No match found \n "+convertedValue+"\n");
            if(on){
              //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
              emergecyabort =1;
              digitalWrite(STARTENGINE, LOW);
              delay(800);
              digitalWrite(STARTENGINE, HIGH);
              on = 0;
              delay(100);
              //fingerlight=0;
            }
            else{
              if(!cutted){
                emergecyabort =1;
                digitalWrite(ENGINECUT, LOW);
                delay(1000);
                digitalWrite(ENGINECUT, HIGH);
                on = 1;
                //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
                delay(100);
              }
            }
          }
        }
      }        
      yield();
      //return;
    }
    //m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    delay(100);
    net = 0;
    Serial.println("It has finished the sensor");
    delay(1000); 
    return 1;
  }
  else{
    return 0;
  }
  
}

// Finger Print Checking Mechanisim------------------------------------------------------------------------
int fingerprintUserCheck(uint16_t fingerId, uint16_t id[], String name, String rfid, byte serial[], byte mohamedTag[]){
  if(fingerId == id[0] || fingerId == id[1] ){
    if((compareValue.indexOf(rfid) != -1)){
      message ="1, "+name+" with fingerprint Tries to start the car. \n\r"; // Message content
    }
    else{
      message = "1, "+name+" with fingerprint started the car. \n\r"; 
      emergecyabort =1;
      digitalWrite(STARTENGINE, LOW);
      delay(800);
      digitalWrite(STARTENGINE, HIGH);
      //fingerlight = 0;
      on = 0;
      delay(100);
    }
    while(after || emerg){
      status = nfc.requestTag(MF1_REQIDL, data);
      if (status == MI_OK) {
        status = nfc.antiCollision(data);
        memcpy(serial, data, 5);
        nfc.selectTag(serial);
        nfc.haltTag();
        Serial.println("It may be in Emergency or Get from the Server");
        //
        if(memcmp(serial, mohamedTag, 4) == 0) {
          Serial.println("Tag verified as "+name+"'s RFID.");
          rfidVerified = true;
          convertedValue="";
          for (int i = 0; i < sizeof(mohamedTag) -1; i++) {
            if (i > 0) {
              convertedValue += " ";
            }
            convertedValue += String(mohamedTag[i], HEX);
          }
          // Compare the converted value with the compareValue
          if (compareValue.indexOf(convertedValue) != -1) {
            Serial.println("Match found");
            Serial.println("This Driver is banned!");
            //message ="1, Naol with RFID :  "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Tries to Start the car. \n\r"; // Message content
          }
          else {
            Serial.println("No match found \n "+convertedValue+"\n");
            if(on){
              //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Started the car. \n\r"; // Message content
              emergecyabort =1;
              digitalWrite(STARTENGINE, LOW);
              delay(800);
              digitalWrite(STARTENGINE, HIGH);
              on = 0;
              delay(100);
              //fingerlight=0;
            }
            else{
              if(!cutted){
                emergecyabort =1;
                digitalWrite(ENGINECUT, LOW);
                delay(1000);
                digitalWrite(ENGINECUT, HIGH);
                on = 1;
                //message ="1, Naol with RFID : "+String(mohamedTag[0],HEX)+" "+ String(mohamedTag[1],HEX)+" "+String(mohamedTag[2],HEX)+" "+String(mohamedTag[3],HEX)+" Stoped the car. \n\r"; // Message content
                delay(100);
              }
            }
          }
        }
      }        
      yield();
      //return;
    }
    //m = 1;
    Serial.println("It is processing Sensor");
    after=1;
    net = 0;
    Serial.println("It has finished the sensor");
    return 1;
  }

  else{
    return 0;
  }
}

void sendGETFtp(String file)
{
  sendATCommand("AT+FTPGETNAME="+file+"");
  delay(5000);
  sendATCommand("AT+FTPGETPATH=\"/home/fleetstore/\"");
  delay(5000);
  sendATCommand("AT+FTPGET=1");
  delay(5000);
  sendATCommandCheck("AT+FTPGET=2,1024");
  delay(5000);
  
  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);
  
  
}
void sendPUTFtp(String file)
{
  sendATCommand("AT+FTPPUTNAME="+file+"");
  delay(5000);
  sendATCommand("AT+FTPPUTPATH=\"/home/fleetstore/\"");
  delay(5000);
  sendATCommand("AT+FTPPUTOPT=\"APPE\""); // This should be on user specification.
  delay(5000);
  sendATCommand("AT+FTPPUT=1");
  delay(5000);
  sendUAT("AT+FTPPUT=2,"+String(message.length()-1)+"\r", message);
  delay(5000);
  sendATCommand("AT+FTPPUT=2,0");
  delay(5000);
  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);
}
void setupFTP(String url, String username, String password){
  // THIS IS FTP
  //START  CONFIGURATION AND COMMUNICATION
  //START GPRS BARRIER
  delay(10000);
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  
  delay(3000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"etc.com\""); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=1,1"); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=2,1"); // GPRS connectivity.
  delay(1000);
  sendATCommand("AT+FTPCID=1");  
  delay(3000);
  sendATCommand("AT+FTPSERV="+url+"");
  delay(3000);
  sendATCommand("AT+FTPUN="+username+"");
  delay(3000);
  sendATCommand("AT+FTPPW="+password+"");
  delay(3000);
  
  
}
void sendUAT(String cmd, String data){
  int i =0;
  while(cmd.charAt(i)){
    Serial3.write(cmd.charAt(i++));
  }
  i=0;
  delay(5000);
  while(data.charAt(i)){
    Serial3.write(data.charAt(i++));
  }  
  delay(1000);
  updateSerial();
}

void sendATCommand(String cmd)
{
  Serial3.println(cmd);
  delay(1000); // Wait for the command to be processed
  updateSerial();
}
void sendATCommandCheck(String cmd)
{
  Serial3.println(cmd);
  delay(1000); // Wait for the command to be processed
  updateSerialSmsRecive();
}

void updateSerial()
{
  while (Serial.available())
  {
    Serial3.write(Serial.read()); // Forward data to Serial3 port
  }
  while (Serial3.available())
  {
    Serial.write(Serial3.read()); // Forward data to Serial port
  }
}

