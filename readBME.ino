/////////////////////////////////////////////
// Declarations
/////////////////////////////////////////////

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

float bmeTemperature, bmeHumidity, bmePressure, bmeAltitude;
Adafruit_BME280 bme;

/////////////////////////////////////////////
// functions
/////////////////////////////////////////////

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

/////////////////////////////////////////////
// setup and loop
/////////////////////////////////////////////

void setup(){
  Serial.begin(115200);

  bool statusBME;
  statusBME = bme.begin(0x78);
  if (!statusBME) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

void loop(){
  readDataBME(); 
  delay(5000);
}
