#include <SPI.h>
#include <RH_NRF24.h>

const int CSN_PIN = 7;
const int CE_PIN = 8;

RH_NRF24 nrf24(CE_PIN, CSN_PIN);

struct sensorData{
  float temp;
  float relHumid;
  float dewPoint;
  float pressure;
}; sensorData dataRX;

void setup() {
  Serial.begin(9600);
  while(!Serial){
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

void loop() {
  // If NRF24 receives data, save to struct
  if(nrf24.available()){
    uint8_t len = sizeof(dataRX);
    if(nrf24.recv((uint8_t *)&dataRX, &len)){
      Serial.print(dataRX.temp);
      Serial.print(", ");
      Serial.print(dataRX.relHumid);
      Serial.print(", ");
      Serial.print(dataRX.dewPoint);
      Serial.print(", ");
      Serial.println(dataRX.pressure);
    }
    else{
      Serial.println("RX failed");
    }
  }
}
