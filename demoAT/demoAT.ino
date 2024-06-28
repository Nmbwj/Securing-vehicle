String url = "\"www.sim.com\"";
String mozila= "\"www.mozilla.org/en-US\"";
String testurl = "\"28b0-196-190-62-68.ngrok-free.app/index.html?inputText=naolisgoing\"";
String urlInternet ="\"www.toptal.com/developers/postbin/1719491840801-6199923341628?hello=world \"";
void setup() {
  // Setup for Serial communication
  Serial.begin(115200);
  Serial3.begin(9600);
  
  // dnsCheckUp(url);
  //dnsCheckUp(url);
  sendHttps(urlInternet);

}

void loop(){}
void sendATCommand(String cmd)
{
  Serial3.println(cmd);
  delay(1000); // Wait for the command to be processed
  updateSerial();
}
void dnsCheckUp(String url)
{

  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  
  delay(3000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"etc.com\""); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=1,1"); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=2,1"); // GPRS connectivity.
  delay(1000);
  sendATCommand("AT+CGATT?");
  delay(10000);
  sendATCommand("AT+CDNSGIP="+url +"");
  delay(10000);
  sendATCommand("AT+CIPSTART=\"TCP\","+url+",443");
  delay(10000);
  

  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);

}
void sendHttp(String url){
  
  // THIS IS HTTP
  //START GPRS BARRIER
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  
  delay(3000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"etc.com\""); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=1,1"); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=2,1"); // GPRS connectivity.
  delay(1000);
  sendATCommand("AT+CGATT?");
  delay(1000);
  // END OF BARRIER

  //START OF HTTP
  sendATCommand("AT+HTTPINIT");
  delay(1000);
  sendATCommand("AT+HTTPPARA =\"CID\",1");
  delay(10000);
  sendATCommand("AT+HTTPPARA=\"URL\","+ url +"");
  delay(10000);
  sendATCommand("AT+HTTPACTION=0");
  delay(10000);
  sendATCommand("AT+HTTPREAD");
  delay(10000);
  sendATCommand("AT+HTTPTERM");
  delay(1000);

  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);

}

void sendPostHttp(String url){
  
  // THIS IS HTTP
  //START GPRS BARRIER
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  
  delay(3000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"etc.com\""); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=1,1"); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=2,1"); // GPRS connectivity.
  delay(1000);
  sendATCommand("AT+CGATT?");
  delay(1000);
  // END OF BARRIER

  //START OF HTTP
  sendATCommand("AT+HTTPINIT");
  delay(1000);
  sendATCommand("AT+HTTPPARA =\"CID\",1");
  delay(10000);
  sendATCommand("AT+HTTPPARA=\"URL\","+ url +"");
  delay(10000);
  sendATCommand("AT+HTTPDATA=100,10000");
  delay(10000);
  sendATCommand("AT+HTTPACTION=1");
  delay(10000);
  
  sendATCommand("AT+HTTPTERM");
  delay(1000);

  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);

}

void sendHttps(String url){

  // THIS IS HTTPS
  //START GPRS BARRIER
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");  
  delay(3000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"etc.com\""); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=1,1"); // APN configuration
  delay(1000);
  sendATCommand("AT+SAPBR=2,1"); // GPRS connectivity.
  delay(1000);
  sendATCommand("AT+CGATT?");
  delay(10000);
  // END OF BARRIER

  //START OF HTTPS
  sendATCommand("AT+HTTPSSL=1");
  delay(10000);
  sendATCommand("AT+HTTPPARA =\"CID\",1");
  delay(10000);
  sendATCommand("AT+HTTPPARA=\"URL\","+ url +"");
  delay(10000);
  sendATCommand("AT+HTTPACTION=0");
  delay(10000);
  sendATCommand("AT+HTTPREAD");
  delay(10000);
  sendATCommand("AT+HTTPTERM");
  delay(10000);

  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);
  

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

