#include <WiFi.h>
#include <PubSubClient.h>
#define LIGHT_PIN 32
 
const char* device_id = "Jan/Thomas-Light-Sensor-right";
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "10.10.2.127";
const char* topic3 = "zuerich/fahrstuhl/LightLeft/in";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(LIGHT_PIN, INPUT);
}
 
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("done!");
}
 
void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while (!client.connected()) {
    if (client.connect(device_id)) {
      Serial.println("done!");
      client.subscribe(topic3);
 
    } else {
      delay(500);
      Serial.print(".");
    }
  }
}
 
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  int light = analogRead(LIGHT_PIN);
  char tempBuffer1[10];
  
  Serial.println(light);
  
  sprintf(tempBuffer1, "%d", light);
  
  client.publish(topic3, tempBuffer1);

  Serial.print("Light intensity sent: ");
  Serial.println(light);
  
  delay(1000);
  client.loop();
}