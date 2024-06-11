#include <MFRC522.h>
#include <SPI.h>
#include "Adafruit_Fingerprint_Due.h"

#define SAD 10
#define RST 5

MFRC522 nfc(SAD, RST);
Adafruit_Fingerprint_Due finger = Adafruit_Fingerprint_Due();

byte samuelTag[5] = {0x33, 0x8B, 0x91, 0xFC};
bool rfidVerified = false;
bool fingerprintVerified = false;
String senderNumber = "";
bool messageReceived = false;
String incomingMessage = "";

void setup() {
  // Setup for Serial communication
  Serial.begin(115200);
  Serial3.begin(9600);
  delay(1000);
  sendATCommand("AT+CMGF=1"); // Set SMS mode to text

  sendATCommand("AT+CNMI=1,2,0,0,0"); // Configure the module to receive SMS messages automatically

  Serial.println("Ready to receive messages.");
  // Setup for RFID
  SPI.begin();
  nfc.begin();
  byte version = nfc.getFirmwareVersion();
  if (!version) {
    Serial.print("Didn't find MFRC522 board.");
    while (1); // halt
  }


  // Setup for Fingerprint sensor
  finger.begin(57600);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
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
  updateSerialSmsRecive();
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
    if (incomingMessage.indexOf("+CMT:") != -1)
    {
      messageReceived = true;

      // Extract the sender number
      int firstQuote = incomingMessage.indexOf('"', incomingMessage.indexOf("+CMT:") + 5);
      int secondQuote = incomingMessage.indexOf('"', firstQuote + 1);
      senderNumber = incomingMessage.substring(firstQuote + 1, secondQuote);
    }

    // Check if the message content has started
    if (messageReceived && incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+CMT:")) != -1)
    {
      int messageStart = incomingMessage.indexOf("\r\n", incomingMessage.indexOf("+CMT:")) + 2;
      if (incomingMessage.indexOf("\r\n", messageStart) != -1)
      {
        String messageContent = incomingMessage.substring(messageStart, incomingMessage.indexOf("\r\n", messageStart));
        messageContent.trim();

        // Process the message
        processMessage(messageContent, senderNumber);

        // Clear the buffers
        incomingMessage = "";
        senderNumber = "";
        messageReceived = false;
      }
    }
  }
}

void processMessage(String message, String sender)
{
  // Check if the sender is the authorized number
  if (sender == "+251933660705")
  {
    // Convert message to lowercase for case-insensitive comparison
    message.toLowerCase();

    // Check if the message contains "enginecut"
    if (message.indexOf("enginecut") != -1)
    {
      //digitalWrite(8, HIGH); // Set pin 2 HIGH
      digitalWrite(8, LOW);
      delay(3500);
      digitalWrite(8, HIGH);
      sendATCommand("AT+CMGF=1"); 
      sendATCommand("AT+CMGS=\"+251933660705\""); // Replace with recipient's phone number
      Serial3.print("Engine is Stoped!."); // Message content
      delay(100);
      Serial3.write(26);

    }
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
      digitalWrite(7, LOW);
      delay(3500);
      digitalWrite(7, HIGH);
    
    sendATCommand("AT+CMGF=1"); 
    sendATCommand("AT+CMGS=\"+251933660705\""); // Replace with recipient's phone number
  Serial3.print("Samuel with RFID : 33 8B 91 FC Started the car."); // Message content
  delay(100);
  Serial3.write(26); // ASCII code of CTRL+Z to send the SMS
  delay(1000); 
    } else {
      Serial.println("Unknown RFID.");
      rfidVerified = false;
      digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    sendATCommand("AT+CMGF=1"); 
    sendATCommand("AT+CMGS=\"+251933660705\""); // Replace with recipient's phone number
  Serial3.print("unknown person with RFID trys to start the car."); // Message content
  delay(100);
  Serial3.write(26); // ASCII code of CTRL+Z to send the SMS
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
    sendATCommand("AT+CMGF=1"); 
    sendATCommand("AT+CMGS=\"+251933660705\""); // Replace with recipient's phone number
    Serial3.print("unknown person with fingerprint trys to start the car."); // Message content
    delay(100);
    Serial3.write(26); // ASCII code of CTRL+Z to send the SMS
    delay(1000); 
    return -1;
  } else {
    Serial.println("Found a print match!");
    digitalWrite(7, LOW);
    delay(3500);
    digitalWrite(7, HIGH);
    sendATCommand("AT+CMGF=1"); 
    sendATCommand("AT+CMGS=\"+251933660705\""); // Replace with recipient's phone number
    Serial3.print("samuel with fingerprint started the car."); // Message content
    delay(100);
    Serial3.write(26); // ASCII code of CTRL+Z to send the SMS
    delay(1000); 
    Serial.print("Found ID #");
    Serial.print(finger.fingerID); 
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID; 
  }
}
void sendATCommand(String cmd)
{
  Serial3.println(cmd);
  delay(1000); // Wait for the command to be processed
  updateSerial();
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
