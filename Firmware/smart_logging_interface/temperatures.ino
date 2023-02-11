#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define THERMO_MISO 35
#define THERMO_SCK 33
#define THERMO_CS1 2
#define THERMO_CS2 12
#define THERMO_CS3 13
#define THERMO_CS4 14
#define THERMO_CS5 15
#define THERMO_CS6 16
#define THERMO_CS7 17
#define THERMO_CS8 25
#define THERMO_CS9 26
#define THERMO_CS10 27
#define THERMO_CS11 32

Adafruit_MAX31855 TC1(THERMO_SCK, THERMO_CS1, THERMO_MISO);
Adafruit_MAX31855 TC2(THERMO_SCK, THERMO_CS2, THERMO_MISO);
Adafruit_MAX31855 TC3(THERMO_SCK, THERMO_CS3, THERMO_MISO);
Adafruit_MAX31855 TC4(THERMO_SCK, THERMO_CS4, THERMO_MISO);
Adafruit_MAX31855 TC5(THERMO_SCK, THERMO_CS5, THERMO_MISO);
Adafruit_MAX31855 TC6(THERMO_SCK, THERMO_CS6, THERMO_MISO);
Adafruit_MAX31855 TC7(THERMO_SCK, THERMO_CS7, THERMO_MISO);
Adafruit_MAX31855 TC8(THERMO_SCK, THERMO_CS8, THERMO_MISO);
Adafruit_MAX31855 TC9(THERMO_SCK, THERMO_CS9, THERMO_MISO);
Adafruit_MAX31855 TC10(THERMO_SCK, THERMO_CS10, THERMO_MISO);
Adafruit_MAX31855 TC11(THERMO_SCK, THERMO_CS11, THERMO_MISO);

int boolean_to_decimal(bool bol[]) {
  int somme = 0;
  for (int i = 0; i < 11; i++) {
    somme += bol[i] * (1 << (10 - i));
  }
  return somme;
}

void get_temperatures() {
  double c1 = TC1.readCelsius();
  double c2 = TC2.readCelsius();
  double c3 = TC3.readCelsius();
  double c4 = TC4.readCelsius();
  double c5 = TC5.readCelsius();
  double c6 = TC6.readCelsius();
  double c7 = TC7.readCelsius();
  double c8 = TC8.readCelsius();
  double c9 = TC9.readCelsius();
  double c10 = TC10.readCelsius();
  double c11 = TC11.readCelsius();
  bool NC[11];
  bool SCP[11];
  bool SCG[11];
#ifdef SIMULATION
  doc_sensors["temperatures"]["TC1"] = random(-50, 150);
  doc_sensors["temperatures"]["TC2"] = random(-50, 150);
  doc_sensors["temperatures"]["TC3"] = random(-50, 150);
  doc_sensors["temperatures"]["TC4"] = random(-50, 150);
  doc_sensors["temperatures"]["TC5"] = random(-50, 150);
  doc_sensors["temperatures"]["TC6"] = random(-50, 150);
  doc_sensors["temperatures"]["TC7"] = random(-50, 150);
  doc_sensors["temperatures"]["TC8"] = random(-50, 150);
  doc_sensors["temperatures"]["TC9"] = random(-50, 150);
  doc_sensors["temperatures"]["TC10"] = random(-50, 150);
  doc_sensors["temperatures"]["TC11"] = random(-50, 150);
  for (int i = 0; i < 11; i++) NC[i] = random(2);
  for (int i = 0; i < 11; i++) SCG[i] = random(2);
  for (int i = 0; i < 11; i++) SCP[i] = random(2);
#else
  if (isnan(c1)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC1.readError();
    if (e & MAX31855_FAULT_OPEN) NC[0] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[0] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[0] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc1) doc_sensors["temperatures"]["TC1"] = c1;
    NC[0] = 0;
    SCG[0] = 0;
    SCP[0] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c2)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC2.readError();
    if (e & MAX31855_FAULT_OPEN) NC[1] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[1] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[1] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc2) doc_sensors["temperatures"]["TC2"] = c2;
    NC[1] = 0;
    SCG[1] = 0;
    SCP[1] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c3)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC3.readError();
    if (e & MAX31855_FAULT_OPEN) NC[2] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[2] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[2] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc3) doc_sensors["temperatures"]["TC3"] = c3;
    NC[2] = 0;
    SCG[2] = 0;
    SCP[2] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c4)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC4.readError();
    if (e & MAX31855_FAULT_OPEN) NC[3] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[3] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[3] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc4) doc_sensors["temperatures"]["TC4"] = c4;
    NC[3] = 0;
    SCG[3] = 0;
    SCP[3] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c5)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC5.readError();
    if (e & MAX31855_FAULT_OPEN) NC[4] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[4] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[4] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc5) doc_sensors["temperatures"]["TC5"] = c5;
    NC[4] = 0;
    SCG[4] = 0;
    SCP[4] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c6)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC6.readError();
    if (e & MAX31855_FAULT_OPEN) NC[5] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[5] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[5] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc6) doc_sensors["temperatures"]["TC6"] = c6;
    NC[5] = 0;
    SCG[5] = 0;
    SCP[5] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c7)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC7.readError();
    if (e & MAX31855_FAULT_OPEN) NC[6] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[6] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[6] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc7) doc_sensors["temperatures"]["TC7"] = c7;
    NC[6] = 0;
    SCG[6] = 0;
    SCP[6] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c8)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC8.readError();
    if (e & MAX31855_FAULT_OPEN) NC[7] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[7] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[7] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc8) doc_sensors["temperatures"]["TC8"] = c8;
    NC[7] = 0;
    SCG[7] = 0;
    SCP[7] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c9)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC9.readError();
    if (e & MAX31855_FAULT_OPEN) NC[8] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[8] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[8] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc9) doc_sensors["temperatures"]["TC9"] = c9;
    NC[8] = 0;
    SCG[8] = 0;
    SCP[8] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c10)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC10.readError();
    if (e & MAX31855_FAULT_OPEN) NC[9] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[9] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[9] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc10) doc_sensors["temperatures"]["TC10"] = c10;
    NC[9] = 0;
    SCG[9] = 0;
    SCP[9] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
  if (isnan(c11)) {
    //Serial.println("Thermocouple fault(s) detected!");
    uint8_t e = TC11.readError();
    if (e & MAX31855_FAULT_OPEN) NC[10] = 1;        //Serial.println("FAULT: Thermocouple is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) SCG[10] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) SCP[10] = 1;  // Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
  } else {
    if (tc11) doc_sensors["temperatures"]["TC11"] = c11;
    NC[10] = 0;
    SCG[10] = 0;
    SCP[10] = 0;
    //Serial.print("C = ");
    //Serial.println(c);
  }
#endif


  if (tc1 || tc2 || tc3 || tc4 || tc5 || tc6 || tc7 || tc8 || tc9 || tc10 || tc11) {
    doc_sensors["temperatures"]["TC_status"]["NC"] = boolean_to_decimal(NC);
    doc_sensors["temperatures"]["TC_status"]["SCP"] = boolean_to_decimal(SCP);
    doc_sensors["temperatures"]["TC_status"]["SCG"] = boolean_to_decimal(SCG);
  }
}