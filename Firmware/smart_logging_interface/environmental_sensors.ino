#include "SHT85.h"
#include <Adafruit_DPS310.h>

#define SHT85_ADDRESS 0x44
Adafruit_DPS310 dps;

SHT85 sht;

int BaseLog = 2.71828;


void setup_env_sensors() {
  Wire.begin();
  sht.begin(SHT85_ADDRESS);


  if (!dps.begin_I2C(0x77)) {

    Serial.println("Failed to find DPS");
  }

  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
}

void get_env_sensors() {
  float pressure, temperature2;
  if (dps.temperatureAvailable() || dps.pressureAvailable()) {
    sensors_event_t temp_event, pressure_event;
    dps.getEvents(&temp_event, &pressure_event);
    temperature2 = temp_event.temperature;
    pressure = pressure_event.pressure;
  }
  sht.read();
  float temperature = sht.getTemperature();
  float humidity = sht.getHumidity();
  float absh = (6.112 * pow(BaseLog, ((17.67 * temperature) / (temperature + 243.5))) * humidity * 2.1674) / (273.15 + temperature);


  doc_sensors["ambient"]["t"] = serialized(String(temperature, 3));
  doc_sensors["ambient"]["t2"] = serialized(String(temperature2, 3));
  doc_sensors["ambient"]["rh"] = serialized(String(humidity, 1));
  doc_sensors["ambient"]["absh"] = serialized(String(absh, 1));
  doc_sensors["ambient"]["p"] = serialized(String(pressure, 1));
  //doc_env.shrinkToFit();
}
