#include <ESP32Servo.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define PIN_SG90 22

const char* device_id = "Servomotor_fur_lift";
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "10.10.2.127";

const char* topic2 = "zuerich/fahrstuhl/Servo/out"; 

int click = 0;

Servo sg90;
WiFiClient espClient;
PubSubClient client(espClient); 

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  sg90.setPeriodHertz(50);
  sg90.attach(PIN_SG90, 500, 2400); 
}

void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  Serial.print("Received Message: ");
  Serial.println(message);

  if (strcmp(message, "1") == 0) { 
    click = 1;
  }else if (strcmp(message, "2") == 0) { 
    click = 2;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(topic2); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect(); 
  }
  client.loop(); 

  sg90.attach(PIN_SG90, 500, 2400); 
  if (click == 1) {
    sg90.write(20);
    delay(500); 

    sg90.write(90);
    delay(500); 
  }
  if (click == 2) {
    sg90.write(170);
    delay(500); 

    sg90.write(90);
    delay(500); 
  }
  sg90.detach();
  delay(500);

  click = 0;
}