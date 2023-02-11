struct i2c_device {
  String name;
  uint8_t adr;
  bool selected;
};

struct i2c_device i2c_devices[] = {
  { "Ultrasonic UNIT",	 		0x57 },
{ "DLight UNIT(BH1750FVI)",		0x23 },
{ "4-Relay UNIT(STM32F030)",		0x26 },
{ "EXT.IO UNIT(PCA9554PW)",		0x27 },
{ "RFID UNIT(MFRC522)",	 		0x28 },
{ "Color UNIT(TCS3472)",		0x29 },
{ "ToF UNIT(VL53L0X)",	 		0x29 },
{ "Digi Clock(TM1637)",	 		0x30 },
{ "DDS UNIT",	 			0x31 },
{ "THERMAL UNIT(MLX90640)",		0x33 },
{ "Themal UNIT(MLX90640)",		0x33 },
{ "ID UNIT(ATECC608B-TNGTLSU-G)",	0x35 },
{ "OLED (SSD1306)",	 		0x3C },
{ "LCD UNIT",	 			0x3E },
{ "INA3221_1",	 			0x40 },
{ "INA3221_2",	 			0x41 },
{ "INA3221_3",	 			0x42 },
{ "INA3221_4",	 			0x43 },
{ "SHT30",	 			0x44 },
{ "ADC UNIT(ADS1100)",	 		0x48 },
{ "A/D(ADS1115)_1",	 		0x49 },
{ "A/D(ADS1115)_2",	 		0x4B },
{ "ACSSR UNIT", 			0x50 },
{ "RTC UNIT(BM8563)",	 		0x51 },
{ "ACCEL UNIT(ADXL345)",	 	0x53 },
{ "SONIC UNIT(RCWL-9600)",	 	0x57 },
{ "Heart UNIT(MAX30100)",	 	0x57 },
{ "TVOC UNIT(SGP30)",	 		0x58 },
{ "NCIR UNIT(MLX90614)",	 	0x5A },
{ "Trace UNIT",	 			0x5A },
{ "DHT12",	 			0x5C },
{ "CardKB UNIT",	 		0x5F },
{ "DAC UNIT(MCP4725)",	 		0x60 },
{ "Kmeter UNIT",	 		0x66 },
{ "IMU UNIT(MPU6886)",	 		0x68 },
{ "HX711(mega328p)",	 		0x6C },
{ "QMP6988",				0x70 },
{ "PaHUB UNIT(TCA9548A)",	 	0x70 },
{ "Gesture UNIT(PAJ7620U2)",	 	0x73 },
{ "BMP280/DPS310",	 		0x77 }

};

