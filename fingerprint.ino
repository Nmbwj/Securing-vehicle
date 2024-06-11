/*************************************************** 
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to 
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include "Adafruit_Fingerprint_Due.h"

int getFingerprintIDez();


// Red connects to +3.3V !!!
// Black connects to Ground
// RX1 (pin 19) is IN from sensor (GREEN wire)
// TX1 (pin 18) is OUT from arduino  (WHITE wire)

Adafruit_Fingerprint_Due finger = Adafruit_Fingerprint_Due();

void setup()  
{
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);


  // set the data rate for the sensor serial port
  finger.begin(57600);
  
}

void loop()                     // run over and over again
{
  getFingerprintIDez();
}


// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)
  {
    Serial.println("Finger Print do not match!");
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    return -1;
  } 
  else {
    Serial.println("Found a print match!");
    digitalWrite(2, HIGH);
    delay(100);
    digitalWrite(2, LOW);
 
  }
  // found a match!
    Serial.print("Found ID #"); Serial.print(finger.fingerID); 
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
  
    return finger.fingerID; 
}

