#include <Wire.h>
#include <Beastdevices_INA3221.h>
#define MAX_DECIMAL_PLACES_VCP 3
// Set I2C address to 0x41 (A0 pin -> VCC)

Beastdevices_INA3221 ina3221_1(INA3221_ADDR40_GND);
Beastdevices_INA3221 ina3221_2(INA3221_ADDR41_VCC);
Beastdevices_INA3221 ina3221_3(INA3221_ADDR42_SDA);
Beastdevices_INA3221 ina3221_4(INA3221_ADDR43_SCL);

void setup_vcp_sensors() {
  ina3221_1.begin();
  ina3221_2.begin();
  ina3221_3.begin();
  ina3221_4.begin();

  ina3221_1.reset();
  ina3221_2.reset();
  ina3221_3.reset();
  ina3221_4.reset();
  // Set shunt resistors to 20 mOhm for all channels
  ina3221_1.setShuntRes(20, 20, 20);
  ina3221_2.setShuntRes(20, 20, 20);
  ina3221_3.setShuntRes(20, 20, 20);
  ina3221_4.setShuntRes(20, 20, 20);
}

void get_vcp_sensors() {
  if (vcp1) {
    float current1_1 = ina3221_1.getCurrent(INA3221_CH1);
    float voltage1_1 = ina3221_1.getVoltage(INA3221_CH1);
    float pwr1_1 = current1_1 * voltage1_1;
    doc_sensors["vcp"]["vcp1"][0] = serialized(String(pwr1_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp1"][1] = serialized(String(voltage1_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp1"][2] = serialized(String(current1_1,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp2) {
    float current1_2 = ina3221_1.getCurrent(INA3221_CH2);
    float voltage1_2 = ina3221_1.getVoltage(INA3221_CH2);
    float pwr1_2 = current1_2 * voltage1_2;
    doc_sensors["vcp"]["vcp2"][0] = serialized(String(pwr1_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp2"][1] = serialized(String(voltage1_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp2"][2] = serialized(String(current1_2,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp3) {
    float current1_3 = ina3221_1.getCurrent(INA3221_CH3);
    float voltage1_3 = ina3221_1.getVoltage(INA3221_CH3);
    float pwr1_3 = current1_3 * voltage1_3;
    doc_sensors["vcp"]["vcp3"][0] = serialized(String(pwr1_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp3"][1] = serialized(String(voltage1_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp3"][2] = serialized(String(current1_3,MAX_DECIMAL_PLACES_VCP));
  }
  
  
  
  if (vcp4) {
    float current2_1 = ina3221_2.getCurrent(INA3221_CH1);
    float voltage2_1 = ina3221_2.getVoltage(INA3221_CH1);
    float pwr2_1 = current2_1 * voltage2_1;
    doc_sensors["vcp"]["vcp4"][0] = serialized(String(pwr2_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp4"][1] = serialized(String(voltage2_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp4"][2] = serialized(String(current2_1,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp5) {
    float current2_2 = ina3221_2.getCurrent(INA3221_CH2);
    float voltage2_2 = ina3221_2.getVoltage(INA3221_CH2);
    float pwr2_2 = current2_2 * voltage2_2;
    doc_sensors["vcp"]["vcp5"][0] = serialized(String(pwr2_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp5"][1] = serialized(String(voltage2_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp5"][2] = serialized(String(current2_2,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp6) {
    float current2_3 = ina3221_2.getCurrent(INA3221_CH3);
    float voltage2_3 = ina3221_2.getVoltage(INA3221_CH3);
    float pwr2_3 = current2_3 * voltage2_3;
    doc_sensors["vcp"]["vcp6"][0] = serialized(String(pwr2_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp6"][1] = serialized(String(voltage2_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp6"][2] = serialized(String(current2_3,MAX_DECIMAL_PLACES_VCP));
  }
  
  

  if (vcp7) {
    float current3_1 = ina3221_3.getCurrent(INA3221_CH1);
    float voltage3_1 = ina3221_3.getVoltage(INA3221_CH1);
    float pwr3_1 = current3_1 * voltage3_1;
    doc_sensors["vcp"]["vcp7"][0] = serialized(String(pwr3_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp7"][1] = serialized(String(voltage3_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp7"][2] = serialized(String(current3_1,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp8) {
    float current3_2 = ina3221_3.getCurrent(INA3221_CH2);
    float voltage3_2 = ina3221_3.getVoltage(INA3221_CH2);
    float pwr3_2 = current3_2 * voltage3_2;
    doc_sensors["vcp"]["vcp8"][0] = serialized(String(pwr3_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp8"][1] = serialized(String(voltage3_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp8"][2] = serialized(String(current3_2,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp9) {
    float current3_3 = ina3221_3.getCurrent(INA3221_CH3);
    float voltage3_3 = ina3221_3.getVoltage(INA3221_CH3); 
    float pwr3_3 = current3_3 * voltage3_3;
    doc_sensors["vcp"]["vcp9"][0] = serialized(String(pwr3_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp9"][1] = serialized(String(voltage3_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp9"][2] = serialized(String(current3_3,MAX_DECIMAL_PLACES_VCP));
  }



  if (vcp10) {
    float current4_1 = ina3221_4.getCurrent(INA3221_CH1);
    float voltage4_1 = ina3221_4.getVoltage(INA3221_CH1);
    float pwr4_1 = current4_1 * voltage4_1;
    doc_sensors["vcp"]["vcp10"][0] = serialized(String(pwr4_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp10"][1] = serialized(String(voltage4_1,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp10"][2] = serialized(String(current4_1,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp11) {
    float current4_2 = ina3221_4.getCurrent(INA3221_CH2);
    float voltage4_2 = ina3221_4.getVoltage(INA3221_CH2);
    float pwr4_2 = current4_2 * voltage4_2;
    doc_sensors["vcp"]["vcp11"][0] = serialized(String(pwr4_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp11"][1] = serialized(String(voltage4_2,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp11"][2] = serialized(String(current4_2,MAX_DECIMAL_PLACES_VCP));
  }
  if (vcp12) {
    float current4_3 = ina3221_4.getCurrent(INA3221_CH3);
    float voltage4_3 = ina3221_4.getVoltage(INA3221_CH3);
    float pwr4_3 = current4_3 * voltage4_3;
    doc_sensors["vcp"]["vcp12"][0] = serialized(String(pwr4_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp12"][1] = serialized(String(voltage4_3,MAX_DECIMAL_PLACES_VCP));
    doc_sensors["vcp"]["vcp12"][2] = serialized(String(current4_3,MAX_DECIMAL_PLACES_VCP));
  }
}