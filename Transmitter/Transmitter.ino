#include <DHT.h>
#include <SPI.h>
#include <RH_NRF24.h>
#include <Adafruit_BMP280.h>

const uint8_t dhtPin = 2;
const uint8_t dhtType = DHT22;
const int CSN_PIN = 7;
const int CE_PIN = 8;

DHT dht(dhtPin, dhtType);
RH_NRF24 nrf24(CE_PIN, CSN_PIN);
Adafruit_BMP280 bmp;

struct sensorData{
  float temp;
  float relHumid;
  float dewPoint;
  float pressure;
}; sensorData dataTX;

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

  if(!nrf24.init()){
    Serial.println("NRF24 init. failed");
  }
  if(!nrf24.setChannel(1)){
    Serial.println("setChannel failed");
  }
  if(!nrf24.setRF(RH_NRF24::DataRate1Mbps, RH_NRF24::TransmitPower0dBm)){
    Serial.println("setRF failed");
  }

  if(!bmp.begin()){
    Serial.println("BMP280 initialization failed");
    // Don't do anything more
    while(1);
  }

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
  
  dataTX.temp = dht.readTemperature();            
  dataTX.relHumid = dht.readHumidity();           
  dataTX.dewPoint = getDewPoint(dataTX.temp, dataTX.relHumid);  
  dataTX.pressure = bmp.readPressure();

  //Serial.print("Temp: ");           Serial.print(dataTX.temp);
  //Serial.print(" C, Rel Humid: ");  Serial.print(dataTX.relHumid);
  //Serial.print(" %, Dew Point: ");  Serial.print(dataTX.dewPoint);
  //Serial.print(" C, Pressure: ");   Serial.print(dataTX.pressure); Serial.println(" kPa");

  nrf24.send((uint8_t*)&dataTX, sizeof(dataTX));
  nrf24.waitPacketSent();

  // DHT22 can only update readings every 2sec, so wait at least 2sec before running again
  delay(10000);
}
