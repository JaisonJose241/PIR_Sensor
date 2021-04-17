#include <ESP8266WiFi.h>
 
const char* ssid = "bharti9198"; // ssid
const char* password = "__";// password
IPAddress ip(192, 168, 0, 210); //set static ip
IPAddress gateway(192, 168, 0, 1); //set getteway
IPAddress subnet(255, 255, 255, 0);//set subnet
const char* deviceName = "esp";
 
int pirPin = 13; //D7
int tl = 14; //D5

int pirFlag;
int tlFlag;
long unsigned int pause = 10000;  
boolean lockLow = true;
boolean takeLowTime; 
long unsigned int lowIn; 
int calibrationTime = 10;

  WiFiServer server(80);

void setup() 
{
  Serial.begin(115200);
  
  pinMode(tl, OUTPUT);
  digitalWrite(tl, HIGH);
 
  //////////////////////////////////////.....PIR.....///////////
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  /*
  for(int i = 0; i < calibrationTime; i++)
     {Serial.print(".");
      delay(500);
     }*/
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
  //////////////////////////////////////////////////////////////

  // Connection to wireless network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(deviceName);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address in serial monitor. It must be the same we entered above
  Serial.print("Type this address in URL to connect: ");
  Serial.print("http://");
  Serial.println(ip);
  Serial.println("/");
 

}
void loop() {

  WiFiClient client = server.available();
 if (client) {
  Serial.println("new client");
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  //pirFlag = 0;
  if (request.indexOf("/pir=on") != -1)  {
    pirFlag = 1;
    if(millis()<90000)
    {
      pirFlag = 0;
    }
  }
  if (request.indexOf("/pir=off") != -1)  {
    pirFlag = 0;
  }  
  //tlFlag = 0;
  if (request.indexOf("/tl=on") != -1)  {
    digitalWrite(tl, LOW);
    tlFlag = 1;
  }
  if (request.indexOf("/tl=off") != -1)  {
    digitalWrite(tl, HIGH);
    tlFlag = 0;
  }

 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<style>");
  client.println(" a {background:#F44336; color:white; padding:25px; text-decoration:none; display:inline-block; width:50%; margin: 20px; text-align:center;}");
  client.println("</style>");
  client.println("</head>");
  client.println("<html>");
  client.println("<div align=\"center\">");
 /// led1 part. GUI, etc
  client.print("<p>PIR Status :");
  if(pirFlag == 1) {
    client.print("ON");
  } else {
    client.print("OFF");
  }
  client.print("</p>");
  client.println("<br><br>");
  client.println("<a href=\"/pir=on\" > ON </a>");
  client.println("<a href=\"/pir=off\"> OFF </a><br />");
  client.println("<br><br>");

  ///led2 part. GUI, etc
  client.print("<p>TubeLight status :"); 
  //if(value == HIGH) {
    if(tlFlag == 1){
    client.print("ON");
  } else {
    client.print("OFF");
  }
  client.print("</p>");
  client.println("<br><br>");
  client.println("<a href=\"/tl=on\"> ON </a>");
  client.println("<a href=\"/tl=off\"> OFF </a><br />"); 
  client.println("<br><br>");
  
  client.print("<p>Sensor status :"); 
  
  //if(value == HIGH) {
    if(millis()< 90000){
    client.print("NOT READY");
  } else {
    client.print("READY");
  }
  client.print("</p>");
  client.println("<br><br>");
  
  client.println("</div>");
  client.println("</html>");  
 
  //delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
  if( pirFlag == 1)
  {
  if(digitalRead(pirPin) == HIGH){
          //the led visualizes the sensors output pin state
             tlFlag = 1;
             digitalWrite(tl, LOW);
       if(lockLow){
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;           
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec");
         }        
         takeLowTime = true;
       }
 
     if(digitalRead(pirPin) == LOW){      
         //the led visualizes the sensors output pin state

       if(takeLowTime){
        
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause,
       //we assume that no more motion is going to happen
       if (millis() - lowIn > pause){
           digitalWrite(tl, HIGH);
           tlFlag = 0;
       }
       if(!lockLow && millis() - lowIn > pause){ 

           //makes sure this block of code is only executed again after
           //a new motion sequence has been detected
           lockLow = true;                       
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           }
  }
   
}
}
