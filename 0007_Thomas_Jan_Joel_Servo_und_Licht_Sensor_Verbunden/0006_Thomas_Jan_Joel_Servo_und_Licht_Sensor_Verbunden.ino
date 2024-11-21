#include <ESP32Servo.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define LIGHT_PIN 32
#define PIN_SG90 22

const char* device_id = "Jan/Thomas-Elevator";
const char* ssid = "GuestWLANPortal";
const char* mqtt_server = "10.10.2.127";
const char* topic1 = "zuerich/fahrstuhl/Light/in";
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
  pinMode(LIGHT_PIN, INPUT);

  sg90.setPeriodHertz(50);
  sg90.attach(PIN_SG90, 500, 2400);
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

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  Serial.print("Received Topic: ");
  Serial.println(topic);
  Serial.print("Received Message: ");
  Serial.println(message);

  if (strcmp(message, "1") == 0) {
    click = 1;
  } else if (strcmp(message, "2") == 0) {
    click = 2;
  }
}

void reconnect() {
  Serial.print("Attempting MQTT connection...");
  while (!client.connected()) {
    if (client.connect(device_id)) {
      Serial.println("done!");
      client.subscribe(topic2);
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
  client.publish(topic1, tempBuffer1);

  Serial.print("Light intensity sent: ");
  Serial.println(light);

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
  click = 0;

  delay(1000);
  client.loop();
}