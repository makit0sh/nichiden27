/*
    PISCIUM Server with ESP8266
    based on "WiFiWebServer"
    (https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples/WiFiWebServer)

    written by T.Maki (nichiden 27)
*/

#include <ESP8266WiFi.h>
#include "NSProtocol.h"
#include "personal_wifi_setting.h"

/*
   for debug
*/
//#define ACRAB_DEBUG
//#define PISCUIM_DEBUG


/**
    define NORTH_HEMISPHERE or SOUTH_HEMISPHER
    use for static IP
*/
#define NORTH_HEMISPHERE

#ifdef NORTH_HEMISPHERE
IPAddress ip(192, 168, 11, 100);
#endif
#ifdef SOUTH_HEMISPHERE
IPAddress ip(192, 168, 11, 101);
#endif

IPAddress gateway(192, 168, 11, 1);
IPAddress subnet(255, 255, 255, 0);

/**
   set your ssid & password
*/
const char* ssid = MY_SSID;
const char* password = MY_PASS;

#ifdef ACRAB_DEBUG
const int led = 13;
#endif

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

NSProtocol nsprotocol;

void setup() {
  Serial.begin(9600);
  delay(10);

#ifdef ACRAB_DEBUG
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
#endif

  // Connect to WiFi network
#ifdef PISCUIM_DEBUG
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef PISCUIM_DEBUG
    Serial.print(".");
#endif
  }
#ifdef PISCUIM_DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
#endif

  //get static ip
  WiFi.config(ip, gateway, subnet);

  // Start the server
  server.begin();
#ifdef PISCUIM_DEBUG
  Serial.println("Server started");
#endif

  // Print the IP address
#ifdef PISCUIM_DEBUG
  Serial.println(WiFi.localIP());
#endif
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
  while (!client.available()) {
    delay(1);
    timeCount++;
    if (timeCount >= 500) {
#ifdef PISCUIM_DEBUG
      Serial.println("Connection Timeout");
#endif
      client.stop();
      return;
    }
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
#ifdef PISCUIM_DEBUG
  Serial.println(req);
#endif
  client.flush();

#ifdef ACRAB_DEBUG
  int val;
#endif

  // Match the request


  if (req.indexOf("/refresh_confirm/status.json") != -1) {
  } else if (req.indexOf("/setPort/status.json?") != -1) {
    int i = req.indexOf("/setPort/status.json?") + 21;
    int j;
    if (req[i] != ' ') {
      do {
        j = i;
        String tmp;
        while ( req[i] != '=' && req[i] != ' ') {
          i++;
        }
        tmp = req.substring(j, i);
#ifdef PISCUIM_DEBUG
        Serial.println(tmp);
#endif
        i++;
        if ( req[i] == '0' ) {
          if (nsprotocol.clearPort(tmp) != true) {
            client.stop();
            return;
          }
        } else if ( req[i] == '1' ) {
          if (nsprotocol.setPort(tmp) != true) {
            client.stop();
            return;
          }
        } else {
          client.stop();
          return;
        }
        i++;
        if (req[i] == ' ') {
          break;
        }
        i++;
      } while (req[i - 1] == '&');
    }
  } else if (req.indexOf("/setConstellationName/status.json?") != -1) {
    int i = req.indexOf("/setConstellationName/status.json?") + 34;
    int j;
    if (req[i] != ' ') {
      do {
        int pinNo;
        if ( req[i + 3] == '=' && req[i] == 'p') {
          pinNo = ((req.charAt(i + 1) - '0') * 10 ) + (req.charAt(i + 2) - '0');
          i += 4;
          j = i;
          while ( req[i] != '&' && req[i] != ' ') {
            i++;
          }
          if (nsprotocol.setConstellationName(pinNo, req.substring(j, i)) != true ) {
            client.stop();
            return;
          }
        } else {
          client.stop();
          return;
        }
        i++;
      } while (req[i - 1] == '&');
    }
  } else if (req.indexOf("/allSet/status.json") != -1) {
    nsprotocol.allSet();
  } else if (req.indexOf("/allClear/status.json") != -1) {
    nsprotocol.allClear();
  } else

#ifdef ACRAB_DEBUG
    if (req.indexOf("/gpio/0") != -1)
      val = 0;
    else if (req.indexOf("/gpio/1") != -1)
      val = 1;
#endif

  {
#ifdef PISCUIM_DEBUG
    Serial.println("invalid request");
#endif
    client.stop();
    return;
  }

#ifdef ACRAB_DEBUG
  digitalWrite(led, val);
#endif

  client.flush();

  char packet[23];
  nsprotocol.getPacket(packet);
  int i;
  for (i = 0; i < 23; i++) {
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
#ifdef PISCUIM_DEBUG
  Serial.println("Client disonnected");
#endif

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

