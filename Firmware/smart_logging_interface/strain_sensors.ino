#include <Wire.h>

struct Strain_sensors {
  float S1;
  float S2;
  float S3;
};
Strain_sensors strain;

void get_StrainSensorData() {
  Wire.beginTransmission(0x6C);
  Wire.requestFrom(0x6C, 3*sizeof(float));
  Wire.readBytes((byte*) &strain, sizeof(strain));
  Wire.endTransmission();
  //Serial.print("S1: "+String(strain.S1,3));
  //Serial.print(" S2: "+String(strain.S2,3));
  //Serial.println(" S3: "+String(strain.S3,3));
  doc_sensors["strain"]["S1"] = strain.S1;
  doc_sensors["strain"]["S2"] = strain.S2;
  doc_sensors["strain"]["S3"] = strain.S3;
}


  