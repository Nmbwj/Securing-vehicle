
String message= "welcome naol\r";
void setup() {
  // Setup for Serial communication
  Serial.begin(115200);
  Serial3.begin(9600);
  /*String url = "\"eu-central-1.sftpcloud.io\"";
  String username = "\"4c9f4fb87fbb4c599e13cd6f637985ea\"";
  String password = "\"9QajNJs9xa67guInVk75HqSl1bTtni57\"";
  */
  String url = "\"49.13.205.144\"";
  String username = "\"fleet\"";
  String password = "\"fleet\"";
  
  setupFTP(url, username, password);
  sendGETFtp();
  delay(15000);
  setupFTP(url, username, password);
  sendPUTFtp();
  

}

void loop(){
  

}

void sendGETFtp()
{
  sendATCommand("AT+FTPGETNAME=\"enginecut.txt\"");
  delay(5000);
  sendATCommand("AT+FTPGETPATH=\"/home/fleet/work/\"");
  delay(5000);
  sendATCommand("AT+FTPGET=1");
  delay(5000);
  sendATCommand("AT+FTPGET=2,1024");
  delay(5000);
  //CLOSE GPRS CONTEX
  sendATCommand("AT+SAPBR =0,1");
  delay(1000);
  
  
}
void sendPUTFtp()
{
  sendATCommand("AT+FTPPUTNAME=\"starteng.txt\"");
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
