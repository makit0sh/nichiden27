/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>
#include "NSProtocol.h"
#include "personal_wifi_setting.h"

/** 
 *  define NORTH_HEMISPHERE or SOUTH_HEMISPHER
 *  use for static IP
 */
#define NORTH_HEMISPHERE
#define ACRAB_DEBUG

#ifdef NORTH_HEMISPHERE
IPAddress ip(192, 168, 11, 100);
#endif
#ifdef SOUTH_HEMISPHERE
IPAddress ip(192, 168, 11, 101);
#endif

IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);

/**
 * set your ssid & password
 */
const char* ssid = SSID;
const char* password = PASS;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);
#ifdef ACRAB_DEBUG
const int led = 13;
#endif

NSProtocol nsprotocol;

void setup() {
  Serial.begin(9600);
  delay(10);

  #ifdef ACRAB_DEBUG
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  #endif

  // Connect to WiFi network
  //Serial.println();
  //Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  //get static ip
  //WiFi.config(ip, gateway, subnet); // itoの環境で接続できず
  
  // Start the server
  server.begin();
  //Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  //Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  client.flush();
  
  // Match the request
  if (req.indexOf("/setPin/status.json?") != -1){
    int i = req.indexOf("/setPin/stats.json") + static_cast<String>("/setPin/status.json?").length();
    while (req[i+1] != ' '){
      
    }
  }
  #ifdef ACRAB_DEBUG
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    val = 1;
  #endif
  else {
    //Serial.println("invalid request");
    client.stop();
    return;
  }

  #ifdef ACRAB_DEBUG
  digitalWrite(led, val);
  #endif
  
  client.flush();

  // Prepare the response
  String json;
  String s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
  #ifdef ACRAB_DEBUG
  s += "{\"And\": ";
  s += String(val);
  s += ", \"Aql\": -1}";
  #endif
  s += nsprotocol.getJsonStatus();
  s += "\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  //Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

