/////////////////////////////////////////////
// Libraries
/////////////////////////////////////////////

#include <WiFi.h>
#include <PubSubClient.h>

// BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/////////////////////////////////////////////
// Constants, variables and objects
/////////////////////////////////////////////

#define relay 26 // Set the GPIO26 as RELAY
#define SEALEVELPRESSURE_HPA (1013.25)

// For WIFI and MQTT connection
char WIFISSID[] = "JohanVu"; // Put your WifiSSID here
char PASSWORD[] = "datakom123"; // Put your wifi password here
char TOKEN[] = "BBFF-ef9gSVjLj9zDBkqHnFYitIod0M7N9U"; // Put your Ubidots' TOKEN
char mqttBroker[]  = "things.ubidots.com";
char MQTT_CLIENT_NAME[] = "esp1"; 
char topicTemperature[] = "livingroom-temp"; // Assing the variable label
char topicHumidity[] = "livingroom-humidity"; // Assing the variable label
char VARIABLE_LABEL_SUBSCRIBE[] = "relay"; // Assing the variable label
char deviceLabel[] = "room1"; // Assig the device label

float bmeTemperature, bmeHumidity, bmePressure, bmeAltitude;

char payload[100];
char topic[150];
char topicSubscribe[100];
char str_sensor[10];

WiFiClient ubidots;
PubSubClient client(ubidots);
Adafruit_BME280 bme;

/////////////////////////////////////////////
// Functions
/////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  if (message == "0") {
    digitalWrite(relay, LOW);
  } else {
    digitalWrite(relay, HIGH);
  }
  Serial.write(payload, length);
  Serial.println();
}

void connectToWifi(){
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as INPUT 
  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
}

void connectToBroker() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

void readDataBME() {
  Serial.print("Temperature = ");
  bmeTemperature = bme.readTemperature();
  Serial.print(bmeTemperature);
  Serial.println(" *C");
  /*
  Serial.print("Pressure = ");
  bmePressure = bme.readPressure()/100.0F;
  Serial.print(bmePressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  bmeAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print(bmeAltitude);
  Serial.println(" m");
  */
  Serial.print("Humidity = ");
  bmeHumidity = bme.readHumidity();
  Serial.print(bmeHumidity);
  Serial.println(" %");

  Serial.println();
}

void sendData(char deviceLabel[], char variableLabel[], float dataValue){
  sprintf(topic, "%s%s", "/v1.6/devices/", deviceLabel);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", variableLabel); // Adds the variable label

  dtostrf(dataValue, 4, 2, str_sensor);
  
  sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
  Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  client.loop();
  delay(1000);
}

/////////////////////////////////////////////
// setup and loop
/////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);

  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", deviceLabel, VARIABLE_LABEL_SUBSCRIBE);
  client.subscribe(topicSubscribe);

  bool statusBME;
  statusBME = bme.begin(0x77);
  if (!statusBME) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop() {  
  readDataBME(); 
  
  connectToWifi();
  connectToBroker();
  sendData(deviceLabel, topicTemperature, bmeTemperature);
  sendData(deviceLabel, topicHumidity, bmeHumidity);
  WiFi.disconnect(); // disconnect from Wifi
  client.disconnect();  // disconnect from the MQTT broker
  
  delay(5000);
}
