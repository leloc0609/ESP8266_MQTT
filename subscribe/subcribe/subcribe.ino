#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "xxxx"
#define AIO_KEY         "xxxx"
WiFiClient client;
const char* ssid     = "Huong";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "0989025749";     // The password of the Wi-Fi network
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bbc-led");

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    digitalWrite(LED_BUILTIN, LOW); 
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  mqtt.subscribe(&button);
  digitalWrite(LED_BUILTIN, HIGH);
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}

void loop() { 
  MQTT_connect();
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
      if (subscription == &button) {
        Serial.print(F("Got: "));
        Serial.println((char *)button.lastread);
        if (strcmp((char *)button.lastread, "1") == 0) {
          digitalWrite(LED_BUILTIN, LOW); 
        }
        if (strcmp((char *)button.lastread, "0") == 0) {
          digitalWrite(LED_BUILTIN, HIGH); 
        }
      }
      
  }
}
