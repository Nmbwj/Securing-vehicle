#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_Fingerprint_Due.h"
#include <Scheduler.h>
#define SAD 10
#define RST 5

MFRC522 nfc(SAD, RST);
Adafruit_Fingerprint_Due finger = Adafruit_Fingerprint_Due();

byte samuelTag[5] = {0x88, 0x4, 0x16, 0xDA};
byte mohamedTag[5] = {0x5D, 0xE5, 0xA2, 0x82};
byte testTag[5] = {0x13, 0xC1, 0x90, 0xFC};
bool rfidVerified = false;
bool fingerprintVerified = false;

bool messageReceived = false;
String incomingMessage = "";
String message;
/*
String url = "\"eu-central-1.sftpcloud.io\"";
String username = "\"4c9f4fb87fbb4c599e13cd6f637985ea\"";
String password = "\"9QajNJs9xa67guInVk75HqSl1bTtni57\"";
*/
  String url = "\"49.13.205.144\"";
  String username = "\"fleet\"";
  String password = "\"fleet\"";

String file = "\"starteng.txt\"";
int buttonPin =9;
int buttonRead;
int after =0, emerg=0, net=1, i=1, j=1, m=1;


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
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  pinMode(buttonPin,INPUT);
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
  Scheduler.startLoop(loop4);
}

void loop() {
  //digitalWrite(22, LOW);
  checkRFID();
  checkFingerprint();

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
  delay(300000);
  if(after || emerg){
    Serial.println("It has been Queed by sensor or Emergency come on!.");
    return;
  }
  after = 1;
  setupFTP(url, username, password);
  sendGETFtp();
  after = 0;
  yield(); 
}

void loop3(){
  buttonRead= digitalRead(buttonPin);
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
    message= "Emergency for car Plate Number: 5D E5 A2 82\r"; // Message content
    file = "\"starteng.txt\""; // Temporarly changed to Starteng due to testing, the change to emergency.txt
    delay(100);
    setupFTP(url, username, password);
    sendPUTFtp(file);
    delay(500);// This is to waint until you lift up your finger.
    file = "\"starteng.txt\"";
    //Serial.println("File : "+ file);
    after =0;
    Serial.println("It get out of Emergency now");
  }
  emerg =0;
  
  yield();
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
    if (incomingMessage.indexOf("+FTPGET:") != -1)
    {
      messageReceived = true;
    }
    
    // Check if the message content has started
    if (messageReceived && incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+FTPGET:")) != -1)
    {
      int messageStart = incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+FTPGET:")) + 2;
      if (incomingMessage.indexOf("\r\n", messageStart) != -1)
      {
        String messageContent = incomingMessage.substring(messageStart, incomingMessage.indexOf("\r\n", messageStart));
        messageContent.trim();

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
      digitalWrite(8, LOW);
      //delay(3500);
      //digitalWrite(8, HIGH);
      message = "Engine is Stoped!. \n If you want give access grand send \"engineon\" "; // Message content
      Serial.println(message);
      delay(100);
    

    }else if (message.indexOf("engineon") != -1){
      digitalWrite(8, HIGH);
      

      message ="Engine is on!."; // Message content
      Serial.println(message);
      delay(100);
      
    }
  
}
void checkRFID() {
  byte status;
  byte data[MAX_LEN];
  byte serial[5];

  status = nfc.requestTag(MF1_REQIDL, data);
  if (status == MI_OK) {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    nfc.selectTag(serial);
    nfc.haltTag();

    if (memcmp(serial, samuelTag, 4) == 0) {
      Serial.println("Tag verified as Samuel's RFID.");
      rfidVerified = true;
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      message ="Samuel with RFID : 88 4 16 DA Started the car."; // Message content
      delay(100);
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return;
        }
        m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      /*setupFTP(url, username, password);
      sendPUTFtp(file);
      */
      net = 0;
      //after=0;
      Serial.println("It has finished the sensor");
      delay(1000); 
      }else if(memcmp(serial, mohamedTag, 4) == 0) {
      Serial.println("Tag verified as Mohamed's RFID.");
      rfidVerified = true;
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
      message= "Mohamed with RFID : 5D E5 A2 82 Started the car.\r"; // Message content
      
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return;
        }
        m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      delay(100);
      /*setupFTP(url, username, password);
      sendPUTFtp(file);
      after=0;*/
      net = 0;
      Serial.println("It has finished the sensor");
  delay(1000); 
    }else if(memcmp(serial, testTag, 4) == 0) {
      Serial.println("Tag verified as Test Card's RFID.");
      rfidVerified = true;
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);

  message= "Test with RFID : 13 C1 90 FC Started the car.\r"; // Message content
  
  while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return;
        }
        m = 1;
  Serial.println("It is processing Sensor");  
  after=1;
  delay(100);
  /*
  setupFTP(url, username, password);
  sendPUTFtp(file);
  after=0;
  */
  net = 0;
  Serial.println("It has finished the sensor");
  delay(1000); 
    }else {
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
  /*
  setupFTP(url, username, password);
  sendPUTFtp(file);
  after=0;
  */
  message= "unknown person with RFID trys to start the car.\r"; // Message content
  
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
  } else {
    fingerprintVerified = false;
  }
}

// Returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
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
    /*
    setupFTP(url, username, password);
    sendPUTFtp(file);
    after=0;
    */
    message = "unknown person with fingerprint trys to start the car.\r"; // Message content
    
    net = 0;
    Serial.println("It has finished the sensor");
    delay(1000); 
    return -1;
  } else {
    Serial.println("Found a print match!");
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    if(finger.fingerID == 1 || finger.fingerID == 2){
      message ="Naol with fingerprint started the car.\r"; // Message content
      
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return finger.fingerID;
        }
        m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      /*
    setupFTP(url, username, password);
    sendPUTFtp(file);
    after=0;
    */
    net =0;
      Serial.println("It has finished the sensor");
    }else if(finger.fingerID == 3 || finger.fingerID == 4){
      message = "Mohammed with fingerprint started the car.\r"; 
      
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return finger.fingerID;
        }
        m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      delay(100);
      /*
    setupFTP(url, username, password);
    sendPUTFtp(file);
    after=0;
    */
    net = 0;
      Serial.println("It has finished the sensor");
    }else if(finger.fingerID == 5 || finger.fingerID == 6){
      message = "Nati with fingerprint started the car.\r";
      
      while(after || emerg){
        if(m){
          Serial.println("It may be in Emergency or Get from the Server");
          m = 0;
        }
        yield();
        //return finger.fingerID;
        }
        m = 1;
      Serial.println("It is processing Sensor");
      after=1;
      delay(100);
       /*
    setupFTP(url, username, password);
    sendPUTFtp(file);
    after=0;
    */
    net = 0;
      Serial.println("It has finished the sensor");
    }
    delay(1000); 
    Serial.print("Found ID #");
    Serial.print(finger.fingerID); 
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID; 
  }
}

void sendGETFtp()
{
  sendATCommand("AT+FTPGETNAME=\"enginecut.txt\"");
  delay(5000);
  sendATCommand("AT+FTPGETPATH=\"/home/fleet/work/\"");
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
  sendATCommand("AT+FTPPUTPATH=\"/home/fleet/work/\"");
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

