/*
    PISCUIM Server with ESP8266
    based on "WiFiWebServer"
    (https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples/WiFiWebServer)

    written by T.Maki (nichiden 27)
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

IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);

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
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("");
  //Serial.println("WiFi connected");

  //get static ip
  WiFi.config(ip, gateway, subnet);

  // Start the server
  server.begin();
  //Serial.println("Server started");

  // Print the IP address
  //Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  //Serial.println("new client");
  unsigned int timeCount = 0;
  while (!client.available()){
    delay(1);
    timeCount++;
    if (timeCount >= 500){
      Serial.println("Connection Timeout");
      client.stop();
      return;
    }
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(req);
  client.flush();

  // Match the request
  if (req.indexOf("/setPin/status.json?") != -1) {
    int i = req.indexOf("/setPin/status.json") + static_cast<String>("/setPin/status.json?").length() + 1;
    while (req[i] != ' ') {
      String tmp;
      while ( req[i] != '=') {
        tmp += req[i];
        i++;
      }
      if ( req[i + 1] == 0 ) {
        if (nsprotocol.clearPort(tmp) != true) {
          client.stop();
          return;
        }
      } else if ( req[i + 1] == 1 ) {
        if (nsprotocol.setPort(tmp) != true) {
          client.stop();
          return;
        }
      } else {
        client.stop();
        return;
      }
      i++;
      if (req[i] != ' ' || req[i] != '&') {
        client.stop();
        return;
      }
    }
  } else if (req.indexOf("/setConstellationName/status.json?") != -1) {
    int i = req.indexOf("/setPin/status.json") + static_cast<String>("/setPin/status.json?").length() + 1;
    while (req[i] != ' ') {
      int pinNo;
      if ( req[i + 4] == '=' && req[i] == 'p') {
        pinNo = (req.charAt(i + 1) * 10 - '0') + (req.charAt(i + 2) - '0');
        i += 5;
        String tmp;
        while ( req[i] != '&' || req[i] != ' ') {
          tmp += req[i];
          i++;
        }
        if (nsprotocol.setConstellationName(pinNo, tmp) == false ){
          client.stop();
          return;
        }
      } else {
        client.stop();
        return;
      }
    }
  } else {
    //Serial.println("invalid request");
    client.stop();
    return;
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

  char packet[5];
  nsprotocol.getPacket(packet);
  int i;
  for (i=0;i<5;i++){
    Serial.write(packet[i]);
  }
  
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

