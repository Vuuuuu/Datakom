/////////////////////////////////////////////
// Libraries
/////////////////////////////////////////////
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

/////////////////////////////////////////////
// Constants, variables and objects
/////////////////////////////////////////////

// Wifi 
const char *SSID = "JohanVu";
const char *PWD = "datakom123";

// MQTT 
const int mqttPort = 1883;
const char* mqttServer = "172.20.10.12";  // IP of the MQTT broker
const char* temperatureTopic = "home/livingroom/temperature";
const char* mqttUsername = "johan"; // MQTT username
const char* mqttPassword = "test123"; // MQTT password
const char* clientID = "livingroom"; // MQTT client ID

// Constants
const int potPin = 32;
int potValue;

// Objects
WiFiClient wifiClient;
PubSubClient client(mqttServer, mqttPort, wifiClient); 

/////////////////////////////////////////////
// Functions
/////////////////////////////////////////////

// Connects the node (ESP32) to the WIFI
void connectToWifi() {
  Serial.print("Connectiog to Wifi ");
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wifi range:");
  Serial.println(WiFi.RSSI());
}

void connectToBroker() {
  Serial.print("Connecting to the MQTT Broker ");
  while (client.connect(clientID, mqttUsername, mqttPassword) != true) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("MQTT broker connected");
}

void readData(){
  potValue = analogRead(potPin);
  Serial.println("PotValue:");
  Serial.println(analogRead(potPin));
  Serial.print(potValue);
  Serial.println(" ");
}
void sendData(){
  if (client.publish(temperatureTopic, String(potValue).c_str())) {
    Serial.println("Temperature sent!");
  } else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqttUsername, mqttPassword);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperatureTopic, String(potValue).c_str());
  }
}

/////////////////////////////////////////////
// setup and loop
/////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(potPin, INPUT);
}

void loop() {
  //Serial.setTimeout(2000);
  readData();
  connectToWifi();
  connectToBroker();
  sendData();
  WiFi.disconnect(); // disconnect from Wifi
  client.disconnect();  // disconnect from the MQTT broker
  delay(10000);
}
