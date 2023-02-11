#include "ADS1X15.h"
#include <PCA95x5.h>
#include <Wire.h>
#define MAX_DECIMAL_PLACES_ANALOG 3
// choose you sensor
// ADS1013 ADS(0x48);
// ADS1014 ADS(0x48);
// ADS1015 ADS(0x48);
// ADS1113 ADS(0x48);
// ADS1114 ADS(0x48);

ADS1115 ADS1(0x4b);
ADS1115 ADS2(0x49);
#define R1 100.0f
#define R2 5.6f
//#define f R2 / (R1 + R2)
float f = 0.053030;
PCA9555 ioport;
void setup_adc() {

  ADS1.begin();
  ADS1.setGain(0);  // 6.144 volt

  ADS2.begin();
  ADS2.setGain(0);  // 6.144 volt
}

void setup_digitals() {

  Wire.begin();
  ioport.attach(Wire);
  ioport.polarity(PCA95x5::Polarity::INVERTED_ALL);
  ioport.direction(PCA95x5::Direction::IN_ALL);
}
void get_adc() {
#ifdef SIMULATION
  doc_sensors["analog"]["A1"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A2"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A3"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A4"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A5"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A6"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A7"] = random(0, 3300) * 0.001 / f;
  doc_sensors["analog"]["A8"] = random(0, 3300) * 0.001 / f;



#else
  int16_t raw0 = ADS1.readADC(0);
  int16_t raw1 = ADS1.readADC(1);
  int16_t raw2 = ADS1.readADC(2);
  int16_t raw3 = ADS1.readADC(3);

  int16_t raw4 = ADS2.readADC(0);
  int16_t raw5 = ADS2.readADC(1);
  int16_t raw6 = ADS2.readADC(2);
  int16_t raw7 = ADS2.readADC(3);
  doc_sensors["analog"]["A1"] = serialized(String(ADS1.toVoltage(raw0) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A2"] = serialized(String(ADS1.toVoltage(raw1) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A3"] = serialized(String(ADS1.toVoltage(raw2) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A4"] = serialized(String(ADS1.toVoltage(raw3) / f, MAX_DECIMAL_PLACES_ANALOG));

  doc_sensors["analog"]["A5"] = serialized(String(ADS2.toVoltage(raw4) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A6"] = serialized(String(ADS2.toVoltage(raw5) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A7"] = serialized(String(ADS2.toVoltage(raw6) / f, MAX_DECIMAL_PLACES_ANALOG));
  doc_sensors["analog"]["A8"] = serialized(String(ADS2.toVoltage(raw7) / f, MAX_DECIMAL_PLACES_ANALOG));
#endif
}

void get_digitals() {
#ifdef SIMULATION
  doc_sensors["digital"]["D1"] = (bool)random(2);
  doc_sensors["digital"]["D2"] = (bool)random(2);
  doc_sensors["digital"]["D3"] = (bool)random(2);
  doc_sensors["digital"]["D4"] = (bool)random(2);
  doc_sensors["digital"]["D5"] = (bool)random(2);
  doc_sensors["digital"]["D6"] = (bool)random(2);
  doc_sensors["digital"]["D7"] = (bool)random(2);
  doc_sensors["digital"]["D8"] = (bool)random(2);
  doc_sensors["digital"]["D9"] = (bool)random(2);
  doc_sensors["digital"]["D10"] = (bool)random(2);
  doc_sensors["digital"]["D11"] = (bool)random(2);
  doc_sensors["digital"]["D12"] = (bool)random(2);
  doc_sensors["digital"]["D13"] = (bool)random(2);
  doc_sensors["digital"]["D14"] = (bool)random(2);
  doc_sensors["digital"]["D15"] = (bool)random(2);
  doc_sensors["digital"]["D16"] = (bool)random(2);
#else

  doc_sensors["digital"]["D1"] = ioport.read(PCA95x5::Port::P00);
  doc_sensors["digital"]["D2"] = ioport.read(PCA95x5::Port::P01);
  doc_sensors["digital"]["D3"] = ioport.read(PCA95x5::Port::P02);
  doc_sensors["digital"]["D4"] = ioport.read(PCA95x5::Port::P03);
  doc_sensors["digital"]["D5"] = ioport.read(PCA95x5::Port::P04);
  doc_sensors["digital"]["D6"] = ioport.read(PCA95x5::Port::P05);
  doc_sensors["digital"]["D7"] = ioport.read(PCA95x5::Port::P06);
  doc_sensors["digital"]["D8"] = ioport.read(PCA95x5::Port::P07);
  doc_sensors["digital"]["D9"] = ioport.read(PCA95x5::Port::P10);
  doc_sensors["digital"]["D10"] = ioport.read(PCA95x5::Port::P11);
  doc_sensors["digital"]["D11"] = ioport.read(PCA95x5::Port::P12);
  doc_sensors["digital"]["D12"] = ioport.read(PCA95x5::Port::P13);
  doc_sensors["digital"]["D13"] = ioport.read(PCA95x5::Port::P14);
  doc_sensors["digital"]["D14"] = ioport.read(PCA95x5::Port::P15);
  doc_sensors["digital"]["D15"] = ioport.read(PCA95x5::Port::P16);
  doc_sensors["digital"]["D16"] = ioport.read(PCA95x5::Port::P17);

#endif
  //doc_digitals.shrinkToFit();
}
