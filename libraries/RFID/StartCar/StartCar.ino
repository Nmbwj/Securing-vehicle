#include <SPI.h>
#include <RFID.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#define SS_PIN 10
#define RST_PIN 9
RFID rfid(SS_PIN, RST_PIN);
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

//Create software serial object to communicate with SIM800L
SoftwareSerial netSerial(5, 4); //SIM800L Tx & Rx is connected to Arduino #3 & #2


#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

String rfidCard;
String incomingMessage = ""; // Variable to store the incoming message
String savedMessage = "";

void setup() {
  Serial.begin(9600);
  netSerial.begin(115200);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
  pinMode(8, OUTPUT);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  netSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  netSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  netSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  
}



void loop() {
  // RFID
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
      if (rfidCard == "93 229 162 130") {
       /*netSerial.println("AT"); //Once the handshake test is successful, it will back to OK
       updateSerial();
       netSerial.println("AT+CMGF=1"); // Configuring TEXT mode
       updateSerial();
       */
       netSerial.println("AT+CMGS=\"+251977245580\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
       updateSerial();
       netSerial.print("Driver \"A\" with RFID 93 229 162 130 started the car "); //text content
       updateSerial();
       netSerial.write(26);
       updateSerial();
       digitalWrite(8, HIGH);
        delay(100);
        digitalWrite(8, LOW);
      } else {
        /*
        netSerial.println("AT"); //Once the handshake test is successful, it will back to OK
        updateSerial();
        netSerial.println("AT+CMGF=1"); // Configuring TEXT mode
       updateSerial();
       */
       netSerial.println("AT+CMGS=\"+251977245580\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
       updateSerial();
       netSerial.print("Unknown Driver with RFID started the car "); //text content
       updateSerial();
       netSerial.write(26);
       updateSerial();
        digitalWrite(7, HIGH);
        delay(50);
        digitalWrite(7, LOW);
      }
    }
    rfid.halt();
  }
  getFingerprintID();
  delay(50); 
  // finger print
}
  uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
    
    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      Serial.println("Found a print match!");
      
      netSerial.println("AT"); //Once the handshake test is successful, it will back to OK
       updateSerial();
       netSerial.println("AT+CMGF=1"); // Configuring TEXT mode
       updateSerial();
       
       netSerial.println("AT+CMGS=\"+251977245580\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
       updateSerial();
       netSerial.print("Driver \"A\" with Fingerprint A started the car "); //text content
       updateSerial();
       netSerial.write(26);
       updateSerial();
       digitalWrite(8, HIGH);
        delay(100);
        digitalWrite(8, LOW);
      digitalWrite(8,HIGH);
      delay(50);
      digitalWrite(8,LOW);
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
      Serial.println("Did not find a match");
      
      netSerial.println("AT"); //Once the handshake test is successful, it will back to OK
        updateSerial();
        netSerial.println("AT+CMGF=1"); // Configuring TEXT mode
       updateSerial();
       netSerial.println("AT+CMGS=\"+251977245580\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
       updateSerial();
       netSerial.print("Unknown Driver with Fingerprint started the car "); //text content
       updateSerial();
       netSerial.write(26);
       updateSerial();
      digitalWrite(7,HIGH);
      delay(50);
      digitalWrite(7,LOW);
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }

    // found a match!
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);

    return finger.fingerID;
  }

  // returns -1 if failed, otherwise returns ID #
  int getFingerprintIDez() {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)  return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)  return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)  return -1;

    // found a match!
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    return finger.fingerID;
  }
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
  while (netSerial.available()) 
  {
    char c = netSerial.read();
    Serial.write(c); // Forward what Software Serial received to Serial Port
    
    incomingMessage += c; // Store the incoming message
    savedMessage = incomingMessage;
    Serial.println("Received Saved Message: " + savedMessage );
    if(savedMessage == "EngineCut"){
      digitalWrite(8, HIGH);
      delay(100);
      digitalWrite(8, LOW);
      delay(100);
      digitalWrite(8, HIGH);
      delay(2000);
      digitalWrite(8,LOW);

    }
    // Check for the end of the message
    if (c == '\n') {
      if (incomingMessage.indexOf("+CMT:") >= 0) {
        Serial.println("Received Message: " + incomingMessage );
        // Process the message if necessary
       
      }
      

      incomingMessage = ""; // Clear the variable for the next message
    }
  }


}

