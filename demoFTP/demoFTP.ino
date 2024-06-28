String url = "\"eu-central-1.sftpcloud.io\"";

String username = "\"21bcb027c0ae4a40a055385b4f97c321\"";
String password = "\"Wm6XB8B8tFjFNYPY4I8OPDSdTgd9NqVJ\"";


void setup() {
  // Setup for Serial communication
  Serial.begin(115200);
  Serial3.begin(9600);
  
  sendFtp(url);
  

}

void loop(){

}

void sendFtp(String url)
{
  // THIS IS FTP
  //START  CONFIGURATION AND COMMUNICATION
  sendATCommand("AT+FTPCID=1");  
  delay(3000);
  //sendATCommand("AT+FTPPORT="+port+""); //port assign
  //delay(1000);
  //sendATCommand("AT+FTPPORT?"); //check port
  //delay(1000);
  //sendATCommand("AT+FTPMODE?"); // check mode of communication whether active or passive
  //delay(1000);
  sendATCommand("AT+FTPSERV="+url+"");
  delay(3000);
  sendATCommand("AT+FTPUN="+username+"");
  delay(3000);
  sendATCommand("AT+FTPPW="+password+"");
  delay(3000);
  sendATCommand("AT+FTPGETNAME=\"enginecut.txt\"");
  delay(5000);
  sendATCommand("AT+FTPGETPATH=\"/\"");
  delay(5000);
  sendATCommand("AT+FTPGET=1");
  delay(5000);
  sendATCommand("AT+FTPGET=2,1024");
  delay(5000);
  
  
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
