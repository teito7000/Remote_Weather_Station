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

struct sensorData {
  float temp;
  float relHumid;
  float dewPoint;
  float pressure;
}; sensorData dataRX;

void dateTime(uint16_t* date, uint16_t* time);
void print2digitsSD(int number, File fileName);
void print2digitsSerial(int number, File fileName);

void setup(){
  Serial.begin(9600);
  while(!Serial){
  }

  if(!SD.begin(CS_PIN)){
    Serial.println("Card failed, or not present");
    while(1);
  }
  if(SD.exists("datalog.txt")){
    SD.remove("datalog.txt");
  }
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
  if(nrf24.available()){
    uint8_t len = sizeof(dataRX);
    if(nrf24.recv((uint8_t *)&dataRX, &len)){
      File dataFile = SD.open("datalog.txt", FILE_WRITE);

      if(dataFile){
        DateTime now = rtc.now();
        dataFile.print(now.year(), DEC);
        dataFile.print("-");
        print2digitsSD(now.month(), dataFile);
        dataFile.print("-");
        print2digitsSD(now.day(), dataFile);
        dataFile.print(", ");
        print2digitsSD(now.hour(), dataFile);
        dataFile.print(":");
        print2digitsSD(now.minute(), dataFile);
        dataFile.print(":");
        print2digitsSD(now.second(), dataFile);
        dataFile.print(", ");

        dataFile.print(dataRX.temp);
        dataFile.print(", ");
        dataFile.print(dataRX.relHumid);
        dataFile.print(", ");
        dataFile.print(dataRX.dewPoint);
        dataFile.print(", ");
        dataFile.println(dataRX.pressure);

        /*
        Serial.print(now.year(), DEC);
        Serial.print("-");
        print2digitsSerial(now.month(), dataFile);
        Serial.print("-");
        print2digitsSerial(now.day(), dataFile);
        Serial.print(", ");
        print2digitsSerial(now.hour(), dataFile);
        Serial.print(":");
        print2digitsSerial(now.minute(), dataFile);
        Serial.print(":");
        print2digitsSerial(now.second(), dataFile);
        Serial.print(", ");
        */

        Serial.print(dataRX.temp);
        Serial.print(", ");
        Serial.print(dataRX.relHumid);
        Serial.print(", ");
        Serial.print(dataRX.dewPoint);
        Serial.print(", ");
        Serial.println(dataRX.pressure);

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

void print2digitsSD(int number, File fileName){
  if (number >= 0 && number < 10){
    fileName.print("0");
  }
  fileName.print(number, DEC);
}

void print2digitsSerial(int number, File fileName) {
  if (number >= 0 && number < 10) {
    Serial.print("0");
  }
  Serial.print(number, DEC);
}
