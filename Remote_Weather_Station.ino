/*
 * Wiring (L->R):
 * 
 * DHT22:
 ** VCC
 ** Data - Pin 2 (and 10k pullup to VCC)
 ** NC
 ** GND
 * 
 * SD Card Module:
 ** GND
 ** VCC
 ** MISO - Pin 12
 ** MOSI - Pin 11
 ** SCK - Pin 13
 ** CS - Pin 4
 */

#include <DHT.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>

const int chipSelectPin = 4;
const uint8_t dhtPin = 2;
const uint8_t dhtType = DHT22;
 
DHT dht(dhtPin, dhtType);
Adafruit_BMP280 bmp;

float getDewPoint(float tempData, float relHumidData){
  // Equation from https://www.omnicalculator.com/physics/dew-point
  
  // Magnus coefficients
  const float a = 17.625;
  const float b = 243.04;

  // Calculate intermediary value "alpha" which is used to calculate dew point
  float alpha = log(relHumidData / 100) + (a * tempData) / (b + tempData);
  float dewPoint = (b * alpha) / (a - alpha);

  return dewPoint;
}

void setup() {
  Serial.begin(9600);
  while(!Serial){
  }
  dht.begin();

  /*
  Serial.print("Initializing BMP280... ");
  if(!bmp.begin()){
    Serial.println("BMP280 initialization failed");
    while(1);
  }
  Serial.println("BMP280 successfully initialized");

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
  /*                Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
  /*                Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
  /*                Adafruit_BMP280::FILTER_X16,      /* Filtering. */
  /*                Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  
  Serial.print("Initializing SD card... ");
  if(!SD.begin(chipSelectPin)){
    Serial.println("Card failed, or not present");
    // Don't do anything more
    while(1);
  }
  Serial.println("SD card successfully initialized");

  if(SD.exists("datalog.txt")){
    SD.remove("datalog.txt");
  }
}

void loop() {
  //const int arrayLength = 4;
  const int arrayLength = 3;
  float sensorData[arrayLength] = {};
  
  float temp = dht.readTemperature();           // Serial.print("Temp: "); Serial.print(temp); Serial.print(", ");
  float relHumid = dht.readHumidity();          // Serial.print(" C, Rel Humid: "); Serial.print(relHumid); Serial.print(", ");
  float dewPoint = getDewPoint(temp, relHumid); // Serial.print(" %, Dew Point: "); Serial.print(dewPoint); Serial.print(", ");
  //float pressure = bmp.readPressure();        // Serial.print(" C, Pressure: "); Serial.print(pressure); Serial.println();

  sensorData[0] = temp;
  sensorData[1] = relHumid;
  sensorData[2] = dewPoint;
  //sensorData[3] = pressure;
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if(dataFile){
    for(int i = 0; i < arrayLength; i++){
      dataFile.print(sensorData[i]);
      Serial.print(sensorData[i]);
      
      if(i < (arrayLength - 1)){
        dataFile.print(",");
        Serial.print(",");
      }
    }
    dataFile.println();
    Serial.println();
    dataFile.close();
  }
  else{
    Serial.println("Error opening datalog.txt");
  }

  // DHT22 can only update readings every 2sec, so wait at least 2sec before running again
  delay(5000);
}
