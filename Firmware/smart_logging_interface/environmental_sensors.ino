#include "SHT85.h"
//#include <Adafruit_DPS310.h>
#include <Dps310.h>
#include <Wire.h>
// Dps310 Opject
Dps310 Dps310PressureSensor = Dps310();
#define SHT85_ADDRESS 0x44
//Adafruit_DPS310 dps;

SHT85 sht;

int BaseLog = 2.71828;


void setup_env_sensors() {
  Wire.begin();
  sht.begin(SHT85_ADDRESS);


  Dps310PressureSensor.begin(Wire);
}

void get_env_sensors() {

  float pressure, temperature2;
  Dps310PressureSensor.measureTempOnce(temperature2, 7);
  Dps310PressureSensor.measurePressureOnce(pressure, 7);
  sht.read();
  float temperature = sht.getTemperature();
  float humidity = sht.getHumidity();
  float absh = (6.112 * pow(BaseLog, ((17.67 * temperature) / (temperature + 243.5))) * humidity * 2.1674) / (273.15 + temperature);

  ambient_p = pressure;
  ambient_absh = absh;
  ambient_rh = humidity;
  ambient_t = temperature;
  doc_sensors["ambient"]["t"] = serialized(String(temperature, 3));
   if(!isnan(temperature2))doc_sensors["ambient"]["t2"] = serialized(String(temperature2, 3));
  doc_sensors["ambient"]["rh"] = serialized(String(humidity, 1));
  doc_sensors["ambient"]["absh"] = serialized(String(absh, 1));
  if(!isnan(pressure) || pressure !=0)doc_sensors["ambient"]["p"] = serialized(String(pressure, 1));
  //doc_env.shrinkToFit();
}
