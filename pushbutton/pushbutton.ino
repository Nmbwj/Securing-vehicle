int buttonPin =9;
int buttonRead;
void setup(){
  Serial.begin(9600);
  pinMode(buttonPin,INPUT);
 }

 void loop(){
  buttonRead= digitalRead(buttonPin);
  if(buttonRead){
    //Serial.println("It is working : "+ String(buttonRead));
    delay(500);
  }
 }