//This file is for connecting to the PC using wifi

//some code stolen from https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
#include "PC_Connection.h"
#include <WiFi.h>

#include "VL53L1X_Interface.h"
#include "config.h"

//uint8_t

WiFiServer server(PORT_USED);

void wifi_init(void){

  const char* ssid = SSID_STR;
  const char* password = PASSWORD_STR;

  String hostname = HOSTNAME;


  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }

  // Set your Static IP address
  IPAddress local_IP(IP_3, IP_2, IP_1, IP_0);
  // Set your Gateway IP address
  IPAddress gateway(GATE_3, GATE_2, GATE_1, GATE_0);
  
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  //WiFi.setHostname(hostname.c_str()); //define hostname
WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);  // This is a MUST!
    if(WiFi.setHostname(HOSTNAME)) {
        Serial.println(WiFi.getHostname());
        Serial.printf("\nHostname set!\n");
          
    } else {
        Serial.printf("\nHostname NOT set!\n");
    }


  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  //start wifi server on specifed port
  server.begin();

}

void check_for_connection_process(){

  static uint32_t previousMillis = 0;

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

    if (client.connected()) {
      Serial.println("Connected to client");
    }

    // close the connection:
    client.stop();
  }

  //incase the device disconnects.
  uint32_t currentMillis = millis();
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=TIMEOUT)) {
    Serial.print(millis());
    Serial.println(" : Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

}

