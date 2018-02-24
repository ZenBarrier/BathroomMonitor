/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

const char* ssid = "appfigures";
const char* password = "polarbear";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(730);

int pinSquare = 3;
int pinWindow = 4;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(pinSquare, INPUT);
  pinMode(pinWindow, INPUT);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

WiFiClient client;
void loop() {
  // Check if a client has connected
  client = server.available();

  while(client.connected()){
    while(client.available()){
      Serial.println("\nnew client");
      doStuff();
      return;
    }
  }
  
  
}

void doStuff(){
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    gpioRoute(0);
  else if (req.indexOf("/gpio/1") != -1)
    gpioRoute(1);
  else if(req.indexOf("/status") != -1){
    statusRoute();
  }
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void gpioRoute(int val){
  // Set GPIO2 according to the request
  digitalWrite(2, val);

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  writeMessageToClient(s);
}

void statusRoute(){
  int valSquare = digitalRead(pinSquare);
  int valWindow = digitalRead(pinWindow);
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "Square bathroom is ";
  s += (valSquare)?"<span id='square' value=1>Busy</span><br>":"<span id='square' value=0>Free</span><br>";
  s += "Window bathroom is ";
  s += (valWindow)?"<span id='window' value=1>Busy</span><br>":"<span id='window' value=0>Free</span><br>";
  s += "</html>\n";
  
  writeMessageToClient(s);
}

void writeMessageToClient(String s){
  client.flush();
  client.print(s);
}

