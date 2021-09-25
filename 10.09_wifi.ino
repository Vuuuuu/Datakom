/////////////////////////////////////////////
// Libraries
/////////////////////////////////////////////
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

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
const char* humidityTopic = "home/livingroom/humidity";
const char* mqttUsername = "johan"; // MQTT username
const char* mqttPassword = "test123"; // MQTT password
const char* clientID = "livingroom"; // MQTT client ID

// Constants
const int potPin = 27;
int potValue;
#define SEALEVELPRESSURE_HPA (1013.25)
float bmeTemperature, bmeHumidity, bmePressure, bmeAltitude;


// Objects
WiFiClient wifiClient;
PubSubClient client(mqttServer, mqttPort, wifiClient); 
Adafruit_BME280 bme;

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

void readDataPotmeter(){
  potValue = analogRead(potPin);
  Serial.println("PotValue:");
  Serial.print(potValue);
  Serial.println(" ");
}

void readDataBME() {
  Serial.print("Temperature = ");
  bmeTemperature = bme.readTemperature();
  Serial.print(bmeTemperature);
  Serial.println(" *C");
    
  Serial.print("Pressure = ");
  bmePressure = bme.readPressure()/100.0F;
  Serial.print(bmePressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  bmeAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print(bmeAltitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  bmeHumidity = bme.readHumidity();

  Serial.print(bmeHumidity);
  Serial.println(" %");

  Serial.println();
}

void sendData(){
  if (client.publish(humidityTopic, String(bmeHumidity).c_str())) {
    Serial.println("Humidity sent!");
  } else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqttUsername, mqttPassword);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(humidityTopic, String(bmeHumidity).c_str());
  }

  if (client.publish(temperatureTopic, String(bmeTemperature).c_str())) {
    Serial.println("Temperature sent!");
  } else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqttUsername, mqttPassword);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperatureTopic, String(bmeTemperature).c_str());
  }

}

/////////////////////////////////////////////
// setup and loop
/////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(potPin, INPUT);

  bool statusBME;

  statusBME = bme.begin(0x77);
  if (!statusBME) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  //Serial.setTimeout(2000);
  //readDataPotmeter();
  readDataBME();
  connectToWifi();
  connectToBroker();
  sendData();
  WiFi.disconnect(); // disconnect from Wifi
  client.disconnect();  // disconnect from the MQTT broker
  delay(5000);
}
