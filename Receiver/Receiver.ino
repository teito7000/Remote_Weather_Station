#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <RTClib.h>
#include <RH_NRF24.h>

const int CSN_PIN = 7;
const int CE_PIN = 8;
const int CS_PIN = 4;
char timestamp[30];

RTC_DS3231 rtc;
RH_NRF24 nrf24(CE_PIN, CSN_PIN);

struct sensorData{
  float temp;
  float relHumid;
  float dewPoint;
  float pressure;
}; sensorData dataRX;

void dateTime(uint16_t* date, uint16_t* time);
void printTwoDigitsSD(int number, File fileName);
void printTwoDigitsSerial(int number, File fileName);

void setup(){
  Serial.begin(9600);
  while(!Serial){
  }

  if(!SD.begin(CS_PIN)){
    Serial.println("Card failed, or not present");
    while(1);
  }
  // Remove existing data file to prevent clutter
  if(SD.exists("datalog.txt")){
    SD.remove("datalog.txt");
  }
  // Used to set the time of file creation on SD card
  SdFile::dateTimeCallback(dateTime); 

  if(!rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }

  if(!nrf24.init()){
    Serial.println("init failed");
  }
  if(!nrf24.setChannel(1)){
    Serial.println("setChannel failed");
  }
  if(!nrf24.setRF(RH_NRF24::DataRate1Mbps, RH_NRF24::TransmitPower0dBm)){
    Serial.println("setRF failed");
  }
}

void loop(){
  // If NRF24 receives data, save to struct
  if(nrf24.available()){
    uint8_t len = sizeof(dataRX);
    if(nrf24.recv((uint8_t *)&dataRX, &len)){
      // Also save time and sensor data to SD card
      File dataFile = SD.open("datalog.txt", FILE_WRITE);
      
      if(dataFile){
        DateTime now = rtc.now();
        dataFile.print(now.year(), DEC);
        dataFile.print("-");
        printTwoDigitsSD(now.month(), dataFile);
        dataFile.print("-");
        printTwoDigitsSD(now.day(), dataFile);
        dataFile.print(", ");
        printTwoDigitsSD(now.hour(), dataFile);
        dataFile.print(":");
        printTwoDigitsSD(now.minute(), dataFile);
        dataFile.print(":");
        printTwoDigitsSD(now.second(), dataFile);
        dataFile.print(", ");

        dataFile.print(dataRX.temp);
        dataFile.print(", ");
        dataFile.print(dataRX.relHumid);
        dataFile.print(", ");
        dataFile.print(dataRX.dewPoint);
        dataFile.print(", ");
        dataFile.println(dataRX.pressure);
 
        // Print values for debugging
        Serial.print(now.year(), DEC);
        Serial.print("-");
        printTwoDigitsSerial(now.month(), dataFile);
        Serial.print("-");
        printTwoDigitsSerial(now.day(), dataFile);
        Serial.print(", ");
        printTwoDigitsSerial(now.hour(), dataFile);
        Serial.print(":");
        printTwoDigitsSerial(now.minute(), dataFile);
        Serial.print(":");
        printTwoDigitsSerial(now.second(), dataFile);
        Serial.print(", ");

        // Print sensor data to serial monitor
        Serial.print(dataRX.temp);
        Serial.print(", ");
        Serial.print(dataRX.relHumid);
        Serial.print(", ");
        Serial.print(dataRX.dewPoint);
        Serial.print(", ");
        Serial.println(dataRX.pressure/1000.0, 2);

        dataFile.close();
      }
      else{
        Serial.println("Error opening datalog.txt");
      }
    }
    else{
      Serial.println("RX failed");
    }
  }
  delay(250);
}

void dateTime(uint16_t* date, uint16_t* time){
  DateTime setupTime = rtc.now();
  sprintf(timestamp, "%02d:%02d:%02d %2d/%2d/%2d \n", setupTime.hour(), setupTime.minute(), setupTime.second(),
          setupTime.month(), setupTime.day(), setupTime.year() - 2000);
  *date = FAT_DATE(setupTime.year(), setupTime.month(), setupTime.day());
  *time = FAT_TIME(setupTime.hour(), setupTime.minute(), setupTime.second());
}

void printTwoDigitsSD(int number, File fileName){
  if (number >= 0 && number < 10){
    fileName.print("0");
  }
  fileName.print(number, DEC);
}

void printTwoDigitsSerial(int number, File fileName){
  if (number >= 0 && number < 10) {
    Serial.print("0");
  }
  Serial.print(number, DEC);
}
