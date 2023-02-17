#define ARDUINOJSON_ENABLE_PROGMEM 1
//#define SIMULATION
//#include <Arduino.h>
#define WM_NODEBUG
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
// include MDNS
#ifdef ESP8266
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <ESPmDNS.h>
#endif

#include <WebServer.h>
//#include <NTPClient.h>
#include <WiFiUdp.h>

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <PubSubClient.h>
//#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <String.h>
#include "sha256.h"
#include "Base64.h"
#include <time.h>
#include "timer.h"
#include "cert.h"
#include <stdio.h>
#include <WiFi.h>
#include <SPI.h>
#include <FS.h>
#include "SD.h"
#include "oled_bitmaps.h"
#include "i2c_dev_definitions.h"


SPIClass sdSPI(VSPI);
#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCLK 18
#define SD_CS 0  //for final board

#define MAX_FILE_NAME_LEN 100
#define MAX_STR_LEN 500
#define DATABASE_NAME "/sd/data.db"
#define DATA_BUF 10
#define MAX_I2C_ADR 10
#define buffersize 3000
#define buffersize_i2c 1000
#include <ESP32CAN.h>
#include <CAN_config.h>
CAN_device_t CAN_cfg;
int recv_ct = 0;
#define CAN_ID_ARR_SIZE 30

#define TRIGGER_PIN 34  // for final board; 39 for M5stack

#include <ArduinoJson.h>

#include <Preferences.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <sqlite3.h>
//#include <ESP32CAN.h>
//#include <CAN_config.h>
#include <EasyButton.h>
String FirmwareVer = {
  "1.0"
};
#define URL_fw_Version "https://raw.githubusercontent.com/kiwi85/Smart_logging_Interface/main/Firmware/smart_logging_interface/build/esp32.esp32.esp32/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/kiwi85/Smart_logging_Interface/main/Firmware/smart_logging_interface/build/esp32.esp32.esp32/smart_logging_interface.ino.bin"
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)

#define LOGO_HEIGHT 64
#define LOGO_WIDTH 128


EasyButton button_config(TRIGGER_PIN, 35, false, false);

Timer can_timer;
Timer data_timer;
Timer status_timer;
Timer timer_update;
Preferences preferences;
RTC_DATA_ATTR int bootCount = 0;
bool ch1, c2, ch3, ch4, ch5, ch6, ch7, ch8, ch9, ch10, ch11, ch12 = false;
uint8_t i2c_device_adr[MAX_I2C_ADR];
float ambient_absh, ambient_rh, ambient_t, ambient_p;
String status;
String status_serial;
int content_length = -1;
int http_status_code = -1;
String date;
String request;
String fullSas;
WiFiClientSecure client;
WiFiManager wm;
WebServer server(80);

/* can declarations  */
//CAN_device_t CAN_cfg;
//CAN_frame_t tx_frame;
//DynamicJsonDocument doc_can_config(12288);
DynamicJsonDocument doc_can_config(16384);
DynamicJsonDocument doc_can(4400);

DynamicJsonDocument doc_sensors(5800);

//DynamicJsonDocument doc_active_i2c_devs(256);
String can_messages = "";
int can_ids[CAN_ID_ARR_SIZE];
int id_counter = 0;
//CAN_frame_t rx_frame;
bool serialize_can_msg = false;
//const char *can_json;

bool has_connected = false;
// setup some parameters
WiFiManagerParameter custom_azure_host("host", "host", "smartdata.servicebus.windows.net", 80);
WiFiManagerParameter custom_azure_endpoint("endpoint", "end point", "/device1/messages/", 80);
WiFiManagerParameter custom_azure_key("key", "key", "/+8UZvshYLw+xTusaFvAa8BYspMBnJHT8fRjFBRAzIU=", 100);
WiFiManagerParameter custom_azure_keyname("keyname", "key name", "RootManageSharedAccessKey", 50);
WiFiManagerParameter custom_logspeed("log_speed", "data logging speed", "5", 6);
WiFiManagerParameter custom_logspeed_can("log_speed_can", "can logging speed", "5", 6);
WiFiManagerParameter custom_options;
WiFiManagerParameter custom_options_i2c;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



IPAddress ip;

String mac;

int rssi;
// TEST OPTION FLAGS
bool wm_nonblocking = false;
String deviceName = "SDL-";
String json = "";
String config_json = "";
String time_stamp;
uint32_t chipId = 0;
String data_json;
String azure_host, azure_key, azure_keyname, azure_endpoint;
bool vcp1, vcp2, vcp3, vcp4, vcp5, vcp6, vcp7, vcp8, vcp9, vcp10, vcp11, vcp12;
bool tc1, tc2, tc3, tc4, tc5, tc6, tc7, tc8, tc9, tc10, tc11;
float interval = 5;
float interval_can = 5;
float upd_interval = 0;



void saveWifiCallback() {
  Serial.println("[CALLBACK] saveWifiCallback fired");
  status = "wifi settings saved";
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager) {
  server.close();
  server.stop();
  Serial.println("[CALLBACK] configModeCallback fired");
  status = "config mode";
  has_connected = true;
}
void get_parameters() {
  DynamicJsonDocument doc_config(1500);
  JsonObject i2c = doc_config.createNestedObject("i2c");
  DynamicJsonDocument doc_active_i2c_devs(256);
  preferences.begin("SLI", false);
  azure_host = preferences.getString("azure_host", "smartdata.servicebus.windows.net");
  azure_endpoint = preferences.getString("azure_endpoint", "/smartlogginginterface/messages/");
  azure_key = preferences.getString("azure_key", "/+8UZvshYLw+xTusaFvAa8BYspMBnJHT8fRjFBRAzIU=");
  azure_keyname = preferences.getString("azure_keyname", "RootManageSharedAccessKey");
  interval = preferences.getFloat("logspeed_data", 5);
  interval_can = preferences.getFloat("logspeed_can", 5);
  upd_interval = preferences.getFloat("updInterval", 0);
  FirmwareVer = preferences.getString("sw_version","1.0");
  String selected_i2c_devices = preferences.getString("i2c", "");
  DeserializationError error = deserializeJson(doc_active_i2c_devs, selected_i2c_devices);
  if (!error) i2c = doc_active_i2c_devs.to<JsonObject>();


  vcp1 = preferences.getBool("vcp1", false);
  vcp2 = preferences.getBool("vcp2", false);
  vcp3 = preferences.getBool("vcp3", false);
  vcp4 = preferences.getBool("vcp4", false);
  vcp5 = preferences.getBool("vcp5", false);
  vcp6 = preferences.getBool("vcp6", false);
  vcp7 = preferences.getBool("vcp7", false);
  vcp8 = preferences.getBool("vcp8", false);
  vcp9 = preferences.getBool("vcp9", false);
  vcp10 = preferences.getBool("vcp10", false);
  vcp11 = preferences.getBool("vcp11", false);
  vcp12 = preferences.getBool("vcp12", false);
  tc1 = preferences.getBool("tc1", true);
  tc2 = preferences.getBool("tc2", true);
  tc3 = preferences.getBool("tc3", true);
  tc4 = preferences.getBool("tc4", true);
  tc5 = preferences.getBool("tc5", true);
  tc6 = preferences.getBool("tc6", true);
  tc7 = preferences.getBool("tc7", true);
  tc8 = preferences.getBool("tc8", true);
  tc9 = preferences.getBool("tc9", true);
  tc10 = preferences.getBool("tc10", true);
  tc11 = preferences.getBool("tc11", true);

  JsonObject log_config = doc_config.createNestedObject("log_config");
  log_config["host"] = azure_host;
  log_config["endp"] = azure_endpoint;
  log_config["key"] = azure_key;
  log_config["keyn"] = azure_keyname;
  log_config["spd"] = interval;

  log_config["vcp1"] = vcp1;
  log_config["vcp2"] = vcp2;
  log_config["vcp3"] = vcp3;
  log_config["vcp4"] = vcp4;
  log_config["vcp5"] = vcp5;
  log_config["vcp6"] = vcp6;
  log_config["vcp7"] = vcp7;
  log_config["vcp8"] = vcp8;
  log_config["vcp9"] = vcp9;
  log_config["vcp10"] = vcp10;
  log_config["vcp11"] = vcp11;
  log_config["vcp12"] = vcp12;

  log_config["tc1"] = tc1;
  log_config["tc2"] = tc2;
  log_config["tc3"] = tc3;
  log_config["tc4"] = tc4;
  log_config["tc5"] = tc5;
  log_config["tc6"] = tc6;
  log_config["tc7"] = tc7;
  log_config["tc8"] = tc8;
  log_config["tc9"] = tc9;
  log_config["tc10"] = tc10;
  log_config["tc11"] = tc11;
  serializeJson(doc_config, Serial);
  Serial.println();
  doc_config.clear();
  doc_config.garbageCollect();

  preferences.end();
}

bool getParam_bool(String name) {
  //read parameter from server, for customhmtl input
  String value;

  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  if (value.equals("on")) return true;
  return false;
}

String getParam(String name) {
  //read parameter from server, for customhmtl input
  String value;

  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }

  return value;
}
String get_str_from_bool(bool var) {
  if (var == 1) return "checked='checked'";
  if (var == 0) return "";
}

void set_i2c_dev_status(uint8_t value) {
  for (int i = 0; i < sizeof(i2c_devices) / sizeof(i2c_devices[0]); i++) {
    if (i2c_devices[i].adr == value) {

      i2c_devices[i].selected = true;

    } else i2c_devices[i].selected = false;
  }
}

String get_device_by_adr(uint8_t value) {
  for (int i = 0; i < sizeof(i2c_devices) / sizeof(i2c_devices[0]); i++) {
    if (i2c_devices[i].adr == value) {
      i2c_devices[i].selected = 1;
      return i2c_devices[i].name;
    }
  }
  return "n/a";
}

void i2c_scan() {
  preferences.begin("SLI", false);
  StaticJsonDocument<256> doc_i2c;
  memset(i2c_device_adr, 0, sizeof(i2c_device_adr));
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // Receive 0 = success (ACK response)
    {
      i2c_device_adr[count] = i;
      //Serial.printf("device: %i  count: %i\n",i,count);
      count++;
    }
  }

  for (int i = 0; i < sizeof(i2c_device_adr) / sizeof(i2c_device_adr[0]); i++) {
    if (i2c_device_adr[i] != 0) {
      set_i2c_dev_status(i2c_device_adr[i]);

      String translatedValue = get_device_by_adr(i2c_device_adr[i]);
      JsonArray i2c_dev = doc_i2c.createNestedArray(String(i2c_device_adr[i]));
      i2c_dev.add(translatedValue);
      if (i2c_device_adr[i] > 0) i2c_dev.add(true);
    }
    String temp_i2c_string;
    serializeJson(doc_i2c, temp_i2c_string);
    preferences.putString("i2c", temp_i2c_string);
    preferences.end();
  }
}

char *generate_i2c_html_str() {
  preferences.begin("SLI", false);
  i2c_scan();
  StaticJsonDocument<256> doc_i2c;
  static char buffer[1000];
  int buff_len = sizeof(buffer) / sizeof(buffer[0]);
  const char *html_head = R"(
<p>I2C Devices:</p>
<hr/>
<p>
)";
  strncpy(buffer, html_head, buff_len);
  for (int i = 0; i < sizeof(i2c_device_adr) / sizeof(i2c_device_adr[0]); i++) {
    if (i2c_device_adr[i] != 0) {
      set_i2c_dev_status(i2c_device_adr[i]);

      String translatedValue = get_device_by_adr(i2c_device_adr[i]);
      JsonArray i2c_dev = doc_i2c.createNestedArray(String(i2c_device_adr[i]));
      i2c_dev.add(translatedValue);
      if (i2c_device_adr[i] > 0) i2c_dev.add(true);
      strncat(buffer, "<label> <input id='", buff_len - strlen(buffer));
      strncat(buffer, String(i2c_device_adr[i]).c_str(), buff_len - strlen(buffer));
      strncat(buffer, "'name='", buff_len - strlen(buffer));
      strncat(buffer, translatedValue.c_str(), buff_len - strlen(buffer));
      strncat(buffer, "'type='checkbox' ", buff_len - strlen(buffer));
      if (i2c_device_adr[i] > 0) strncat(buffer, "checked", buff_len - strlen(buffer));
      strncat(buffer, "/> ", buff_len - strlen(buffer));
      strncat(buffer, translatedValue.c_str(), buff_len - strlen(buffer));
      strncat(buffer, "</label>", buff_len - strlen(buffer));

      //Serial.println(translatedValue); // Outputs: "Device3"
    }
  }
  strncat(buffer, "</p>", buff_len - strlen(buffer));
  String temp_i2c_string;
  serializeJson(doc_i2c, temp_i2c_string);
  preferences.putString("i2c", temp_i2c_string);
  preferences.end();
  return buffer;
}

char *get_option_string() {

  static char buf[buffersize];


  strncpy(buf, "<p>temperatures:</p> <hr /> <p><label> <input id='tc1'", buffersize);

  strncat(buf, get_str_from_bool(tc1).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc1' type='checkbox' /> TC1 </label> <label> <input id='tc2' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc2).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc2' type='checkbox' /> TC2 </label> <label> <input id='tc3' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc3' type='checkbox' /> TC3 </label> <label> <input id='tc4' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc4).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc4' type='checkbox' /> TC4 </label> <label> <input id='tc5' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc5).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc5' type='checkbox' /> TC5 </label> <label> <input id='tc6' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc6' type='checkbox' /> TC6 </label> <label> <input id='tc7' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc7' type='checkbox' /> TC7 </label> <label> <input id='tc8' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc8' type='checkbox' /> TC8 </label> <label> <input id='tc9' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc9' type='checkbox' /> TC9 </label> <label> <input id='tc10' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc10' type='checkbox' /> TC10 </label> <label> <input id='tc11' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(tc3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='tc11' type='checkbox' /> TC11 </label></p> <p>Voltage/Current:</p> <hr /> <p><label> <input id='vcp1' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp1).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp1' type='checkbox' /> CH1 </label> <label> <input id='vcp2' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp2).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp2' type='checkbox' /> CH2 </label> <label> <input id='vcp3' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp3).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp3' type='checkbox' /> CH3 </label> <label> <input id='vcp4' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp4).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp4' type='checkbox' /> CH4 </label> <label> <input id='vcp5' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp5).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp5' type='checkbox' /> CH5 </label> <label> <input id='vcp6' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp6).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp6' type='checkbox' /> CH6 </label> <label> <input id='vcp7' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp7).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp7' type='checkbox' /> CH7 </label> <label> <input id='vcp8' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp8).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp8' type='checkbox' /> CH8 </label> <label> <input id='vcp9' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp9).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp9' type='checkbox' /> CH9 </label> <label> <input id='vcp10' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp10).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp10' type='checkbox' /> CH10 </label> <label> <input id='vcp11' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp11).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp11' type='checkbox' /> CH11 </label> <label> <input id='vcp12' ", buffersize - strlen(buf));
  strncat(buf, get_str_from_bool(vcp12).c_str(), buffersize - strlen(buf));
  strncat(buf, "' name='vcp12' type='checkbox' /> CH12 </label></p>", buffersize - strlen(buf));
  strncat(buf, "<hr /><p><label for='upd_interval'>check for update</label></p><select id='upd_interval'>", buffersize - strlen(buf));
  if (upd_interval == 60) strncat(buf, "<option value='60' selected>1 minute</option>", buffersize - strlen(buf));
  else strncat(buf, "<option value='60'>1 minute</option>", buffersize - strlen(buf));
  if (upd_interval == 3600) strncat(buf, "<option value='3600' selected>1 hour</option>", buffersize - strlen(buf));
  else strncat(buf, "<option value='3600'>1 hour</option>", buffersize - strlen(buf));
  if (upd_interval == 86400) strncat(buf, "<option value='86400' selected>24 hours</option>", buffersize - strlen(buf));
  else strncat(buf, "<option value='86400'>24 hours</option>", buffersize - strlen(buf));
  if (upd_interval == 0) strncat(buf, "<option value='0' selected>never</option></select>", buffersize - strlen(buf));
  else strncat(buf, "<option value='0'>never</option></select>", buffersize - strlen(buf));
  return buf;
}
void saveParamCallback() {
  preferences.begin("SLI", false);
  Serial.println("[CALLBACK] saveParamCallback fired");
  float temp_interval = atof(custom_logspeed.getValue());
  float temp_interval_can = atof(custom_logspeed_can.getValue());
  String temp_azure_host = String(custom_azure_host.getValue());
  String temp_azure_endpoint = String(custom_azure_endpoint.getValue());
  String temp_azure_key = String(custom_azure_key.getValue());
  String temp_azure_keyname = String(custom_azure_keyname.getValue());

  preferences.putString("azure_host", temp_azure_host);
  preferences.putString("azure_endpoint", temp_azure_endpoint);
  preferences.putString("azure_key", temp_azure_key);
  preferences.putString("azure_keyname", temp_azure_keyname);
  preferences.putFloat("logspeed_can", temp_interval_can);
  preferences.putFloat("logspeed_data", temp_interval);

  preferences.putBool("tc1", getParam_bool("tc1"));
  preferences.putBool("tc2", getParam_bool("tc2"));
  preferences.putBool("tc3", getParam_bool("tc3"));
  preferences.putBool("tc4", getParam_bool("tc4"));
  preferences.putBool("tc5", getParam_bool("tc5"));
  preferences.putBool("tc6", getParam_bool("tc6"));
  preferences.putBool("tc7", getParam_bool("tc7"));
  preferences.putBool("tc8", getParam_bool("tc8"));
  preferences.putBool("tc9", getParam_bool("tc9"));
  preferences.putBool("tc10", getParam_bool("tc10"));
  preferences.putBool("tc11", getParam_bool("tc11"));

  preferences.putBool("vcp1", getParam_bool("vcp1"));
  preferences.putBool("vcp2", getParam_bool("vcp2"));
  preferences.putBool("vcp3", getParam_bool("vcp3"));
  preferences.putBool("vcp4", getParam_bool("vcp4"));
  preferences.putBool("vcp5", getParam_bool("vcp5"));
  preferences.putBool("vcp6", getParam_bool("vcp6"));
  preferences.putBool("vcp7", getParam_bool("vcp7"));
  preferences.putBool("vcp8", getParam_bool("vcp8"));
  preferences.putBool("vcp9", getParam_bool("vcp9"));
  preferences.putBool("vcp10", getParam_bool("vcp10"));
  preferences.putBool("vcp11", getParam_bool("vcp11"));
  preferences.putBool("vcp12", getParam_bool("vcp12"));

  azure_host = preferences.getString("azure_host", "smartdata.servicebus.windows.net");
  azure_endpoint = preferences.getString("azure_endpoint", "/smartlogginginterface/messages/");
  azure_key = preferences.getString("azure_key", "/+8UZvshYLw+xTusaFvAa8BYspMBnJHT8fRjFBRAzIU=");
  azure_keyname = preferences.getString("azure_keyname", "smartlogginginterface");
  interval = preferences.getFloat("logspeed_can", 5);
  interval_can = preferences.getFloat("logspeed_data", 5);
  upd_interval = preferences.getFloat("updInterval", 60);
  preferences.end();
  status = "saved settings";
  oled_draw_text(status);
  get_parameters();
  data_timer.setInterval(interval * 1000);
  wm.server->send(200, "text/plain", "saved changes\nconfig portal will exit now...\ndevice will connect to wifi station again for normal operation");
  delay(4000);

  wm.stopConfigPortal();
  //delay(1000);
  //ESP.restart();
}



void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleRoot() {
  loadFromSD("/data.db");
}


bool loadFromSD(String path) {
  String dataType = "application/octet-stream";

  Serial.print("Requested page -> ");
  Serial.println(path);
  Serial.println("[HTTP] handle route");
  File dataFile = SD.open(path, FILE_READ);  // Now read data from SD Card
  if (dataFile) {
    if (server.streamFile(dataFile, dataType) != dataFile.size()) {
      Serial.println("Sent less data than expected!");
    } else {
      Serial.println("Page served!");
    }

    dataFile.close();  // close the file:
  } else {
    handleNotFound();
    return false;
  }
  return true;
}
void handleRoute_wm() {
  Serial.println("[HTTP] handle route");
  loadFromSD("/data.db");
  wm.server->send(200, "text/plain", "hello from wm");
}

void start_http_server() {
  server.on("/download", handleRoot);



  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void bindServerCallback() {
  //wm.server->on("/data",handleRoute);
  wm.server->on("/data", handleRoute_wm);  // you can override wm!
}
void on_pressed_button_config() {


  // start portal w delay
  Serial.println("Starting config portal");
  status = "start config portal";
  oled_draw_text(status);
  wm.setConfigPortalTimeout(120);

  if (!wm.startConfigPortal(deviceName.c_str())) {
    Serial.println("failed to connect or hit timeout");
    status = "config timeout";
    oled_draw_text(status);
    delay(3000);
    //start_http_server();
    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    status = "connected";
    oled_draw_text(status);
    //start_http_server();
  }
  start_http_server();
}

void on_pressed_button_config_reset() {



  Serial.println("Erasing Config, restarting");
  status = "config reset-->rebooting";
  oled_draw_text(status);
  //preferences.clear();
  wm.resetSettings();
  delay(3000);
  ESP.restart();
}

/**
* defines and declarations for database
*/
sqlite3 *db1 = NULL;
sqlite3_stmt *res;

//char sql[1024];
int rc;
const char *tail;
String JSON_data;
String json_can_config = "";
int counter_callback, code = -1;


int open_db() {
  if (db1 != NULL)
    sqlite3_close(db1);
  int rc = sqlite3_open(DATABASE_NAME, &db1);
  if (rc) {
    Serial.print(F("Can't open database: "));
    status = "can't open database";
    Serial.print(sqlite3_extended_errcode(db1));
    Serial.print(" ");
    Serial.println(sqlite3_errmsg(db1));
    return rc;
  } else
    //Serial.println(F("Opened database successfully"));
    //status = "database open";
    //display_heap();
    return rc;
}
/**
db_enquery for retrieving data from DB.

@param String enquery.
@return none.
*/
void db_enquery(String enquery, bool should_print) {

  open_db();
  JSON_data = "";


  //db_open();

  //Serial.println("Begin query");
  status = "database query";
  //display_heap();
  code = db_exec(enquery.c_str(), should_print);


  counter_callback = 0;
  sqlite3_close(db1);
}

/**
Callback function for db enqueries.

@param data, argc,argv,azColName.
@return 0.
*/
static int callback(void *daten, int argc, char **argv, char **azColName) {
  int i;
  //const int json_size = JSON_OBJECT_SIZE(MAX_ROWS);
  JsonArray *row_array = (JsonArray *)daten;
  DynamicJsonDocument doc(6400);

  JsonObject sub = doc.createNestedObject();

  for (i = 0; i < argc; i++) {

    sub[azColName[i]] = String(argv[i]);
  }

  (*row_array).add(sub);
  counter_callback++;
  //Serial.printf("\n");
  doc.clear();
  doc.garbageCollect();
  return 0;
}


char *zErrMsg = 0;
/**
db_exec.

execute db enquery.

@param sql.
@return rc.
*/
int db_exec(const char *sql, bool should_print) {
  if (db1 == NULL) {
    Serial.println("No database open");
    status = "No database open";
    return 0;
  }



  DynamicJsonDocument doc(12400);


  JsonArray daten = doc.createNestedArray("response");

  //Serial.println(sql);
  long start = micros();
  counter_callback = 0;
  //display_heap();
  int rc = sqlite3_exec(db1, sql, callback, &daten, &zErrMsg);
  //display_heap();
  doc["code"] = code;  //rc;
  if (counter_callback == 1) {
    doc.remove("response");
    doc["response"] = daten[0];
    daten.remove(0);
  }

  if (rc != SQLITE_OK) {
    Serial.printf("SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    //sqlite3_close(db1);
  } else {
    //Serial.printf("Operation done successfully\n");
    //Serial.printf("there are %d rows for this enquery\r\n", counter_callback);
    serializeJson(doc, JSON_data);
    if (should_print) {
      serializeJson(doc, Serial);
      Serial.println();
      Serial.printf("Time taken: %d rows: %d\n", (micros() - start) / 1000, counter_callback);
    }
  }
  doc.clear();
  doc.garbageCollect();

  return rc;
}

void displayFreeHeap() {
  Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
  Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
  Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
  Serial.printf("Max Alloc Heap: %d\n", ESP.getMaxAllocHeap());
  status = "Free Heap:" + String(esp_get_free_heap_size());
}


int packetId = 0;
int reconn_counter = 0;

float extractNbits(byte *frame, int startBit, int length, float factor, int offset, bool _signed = false) {
  unsigned long value = 0;
  int byteNum = startBit / 8;
  int bitNum = startBit % 8;

  for (int i = 0; i < length; i++) {
    value |= ((frame[byteNum] >> bitNum) & 1) << i;
    bitNum++;
    if (bitNum == 8) {
      byteNum++;
      bitNum = 0;
    }
  }
  float result = (value * factor) + offset;
  if (!_signed) result = abs(result);
  return result;
}
bool is_in_array(int val) {

  int arrSize = 0;
  for (int i = 0; i < sizeof(can_ids) / sizeof(can_ids[0]); i++) {
    if (can_ids[i] != 0) arrSize++;
  }
  //Serial.printf("array size: %i\n",arrSize);


  for (int i = 0; i < arrSize; i++) {
    if (can_ids[i] == val) {
      return true;
      //doc.remove(String(p.key().c_str()));
    }
  }
  return false;
}

void compareArrayToJSON() {
  //Serial.println("clean up doc_can");

  for (JsonPair p : doc_can["CAN"].as<JsonObject>()) {
    int key = String(p.key().c_str()).toInt();
    if (!is_in_array(key)) doc_can["CAN"].remove(String(key));
  }
  //memset(can_ids, 0, sizeof(can_ids));
}
bool array_has_value(int value) {
  for (int i = 0; i < sizeof(can_ids) / sizeof(can_ids[0]); i++) {
    if (can_ids[i] == value) return true;
  }
  return false;
}
void status_timer_callback() {
  DynamicJsonDocument doc_status(256);
  doc_status["status"] = status;
  doc_status["serial"] = status_serial;
  doc_status["wifi"] = wm.getWiFiSSID(true);
  doc_status["http"] = http_status_code;
  doc_status["length"] = content_length;
  doc_status["node"] = deviceName;
  doc_status["firmware"] = FirmwareVer;
  doc_status["updInterval"] = upd_interval;
  serializeJson(doc_status, Serial);
  Serial.println();
  doc_status.clear();
  doc_status.garbageCollect();
}


void data_timer_callback() {
  //Serial.println(data_json);

  ip = WiFi.localIP();
  mac = WiFi.macAddress();
  rssi = WiFi.RSSI();


  String temp_variables = "";





  doc_sensors["node"] = deviceName;
  //doc["mac"] = WiFi.macAddress();
  //doc["rssi"] = WiFi.RSSI();
  //doc_can.shrinkToFit();
  //doc_sensors["CAN"] = doc_can;

  //doc["ip"] = ip.toString();
  //doc["date"] = date;
  //doc["memory"] = String((float)(SD.usedBytes() / (1024.0 * 1024.0)));
  status = "publish sensors";
  //doc_sensors.shrinkToFit();
  serializeJson(doc_sensors, temp_variables);
  request = String("POST ") + azure_endpoint.c_str() + " HTTP/1.1\r\n" + "Host: " + azure_host.c_str() + "\r\n" + "Authorization: SharedAccessSignature " + fullSas + "\r\n" + "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" + "Content-Length: " + temp_variables.length() + "\r\n\r\n" + temp_variables;
  content_length = temp_variables.length();
  client.print(request);
  Serial.println(temp_variables);
  oled_draw_text(status);
}
void can_timer_callback() {

  for (int i = 0; i < 30; i++) {
    Serial.printf("can_id: %i, ", can_ids[i]);
  }
  Serial.println();
  String temp_can_msg = "";
  status = "publish can";
  doc_can["node"] = deviceName;
  serializeJson(doc_can, temp_can_msg);
  String request_can = String("POST ") + azure_endpoint.c_str() + " HTTP/1.1\r\n" + "Host: " + azure_host.c_str() + "\r\n" + "Authorization: SharedAccessSignature " + fullSas + "\r\n" + "Content-Type: application/atom+xml;type=entry;charset=utf-8\r\n" + "Content-Length: " + temp_can_msg.length() + "\r\n\r\n" + temp_can_msg;
  Serial.println(temp_can_msg);
  client.print(request_can);
}


void firmwareUpdate(void) {
  WiFiClientSecure client_ota_upd;
  client_ota_upd.setCACert(rootCACertificate);
  //httpUpdate.setLedPin(LED_BUILTIN, LOW);
  t_httpUpdate_return ret = httpUpdate.update(client_ota_upd, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      status = "OTA update failed";
      oled_draw_text(status);
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      status = "no new OTA update";
      oled_draw_text(status);
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      status = "OTA success";
      oled_draw_text(status);
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}
int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure *client_ota_upd = new WiFiClientSecure;

  if (client_ota_upd) {
    client_ota_upd->setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient https;

    if (https.begin(*client_ota_upd, fwurl)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK)  // if version received
      {
        payload = https.getString();  // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client_ota_upd;
  }

  if (httpCode == HTTP_CODE_OK)  // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version:%s\n", FirmwareVer);
      return 0;
    } else {
      Serial.println(payload);
      preferences.begin("SLI", false);
      preferences.putString("sw_version", payload);
      preferences.end();
      Serial.println("New firmware detected");
      return 1;
    }
  }
  return 0;
}
void check_update() {
  if (FirmwareVersionCheck()) {
    status = "OTA update";
    oled_draw_text(status);
    firmwareUpdate();
  }
}

void oled_drawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width() - LOGO_WIDTH) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}

void oled_draw_text(String status) {


  display.setTextSize(1);  // Normal 1:1 pixel scale
  display.setCursor(0, 0);
  display.setTextColor(WHITE, BLACK);
  display.println("IP: " + WiFi.localIP().toString());
  display.setCursor(0, 8);
  display.setTextColor(WHITE, BLACK);  // Draw white text
  display.println("MAC:" + WiFi.macAddress());
  display.drawLine(0, 18, display.width() - 1, 18, WHITE);

  display.setCursor(0, 25);
  display.println("                     ");
  display.setCursor(0, 25);
  display.setTextColor(WHITE, BLACK);  // Draw white text

  display.println("t:" + String(ambient_t, 2) + (char)247 + "C rh:" + String(ambient_rh, 1) + "%");
  display.setCursor(0, 35);
  display.println("                     ");
  display.setCursor(0, 35);
  display.setTextColor(WHITE, BLACK);  // Draw white text
  display.println("p:" + String(ambient_p, 0) + " mbar ah:" + String(ambient_absh, 1) + "%");
  display.drawLine(0, 50, display.width() - 1, 50, WHITE);
  display.setCursor(0, 55);
  display.println("                     ");
  display.setCursor(0, 55);
  display.setTextColor(WHITE, BLACK);  // Draw white text
  display.println(status);

  display.display();
}

void setup() {
  pinMode(0, OUTPUT);
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  deviceName += String(chipId);

  Serial.begin(115200);
  Serial.setRxBufferSize(2048);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  oled_drawbitmap();
  //sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  //sdSPI.setFrequency(1000000);
  //sdSPI.begin();
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI)) {
    Serial.println(" Memory card mount failed ");
    status = "Card mount failed";
    while (true) {
    }
    //return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {

    Serial.println(" No memory card connected ");
    status = "No memory card";
    return;
  } else if (cardType == CARD_MMC) {

    Serial.println(" Mounted MMC card ");
  } else if (cardType == CARD_SD) {

    Serial.println(" Mounted SDSC card ");
    status = "Mounted SDSC card";
  } else if (cardType == CARD_SDHC) {

    Serial.println(" Mounted SDHC card ");
    status = "Mounted SDHC card";
  } else {

    Serial.println(" An unknown memory card is mounted ");
  }

  sqlite3_initialize();



  Serial.setDebugOutput(false);

  /* start Setup WifiManager */
  //WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability
  client.setInsecure();
  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  wm.setHostname(deviceName.c_str());
  wm.setTitle("Smart Data Logger "+FirmwareVer);

  WiFi.setHostname(deviceName.c_str());
  //wm.setEnableConfigPortal(false);
  wm.setDebugOutput(false);


  //reset settings - for testing
  //wm.resetSettings();
  if (wm_nonblocking) wm.setConfigPortalBlocking(false);

  //wm.erase();
  // callbacks
  wm.setAPCallback(configModeCallback);
  wm.setWebServerCallback(bindServerCallback);
  wm.setSaveConfigCallback(saveWifiCallback);
  wm.setSaveParamsCallback(saveParamCallback);
  get_parameters();
  // add all your parameters here
  wm.addParameter(&custom_azure_host);
  wm.addParameter(&custom_azure_endpoint);
  wm.addParameter(&custom_azure_key);
  wm.addParameter(&custom_azure_keyname);
  wm.addParameter(&custom_logspeed);
  wm.addParameter(&custom_logspeed_can);
  char *options = get_option_string();
  //Serial.println(options);
  new (&custom_options) WiFiManagerParameter(options);                      // custom html input
  new (&custom_options_i2c) WiFiManagerParameter(generate_i2c_html_str());  // custom html input

  wm.addParameter(&custom_options);
  wm.addParameter(&custom_options_i2c);


  //Serial.printf("ch1: %d ch2: %d ch3: %d ch4: %d\n",a_ch1,a_ch2,a_ch3,a_ch4);
  custom_azure_host.setValue(azure_host.c_str(), 80);
  custom_azure_endpoint.setValue(azure_endpoint.c_str(), 80);
  custom_azure_keyname.setValue(azure_keyname.c_str(), 50);
  custom_azure_key.setValue(azure_key.c_str(), 100);
  char buffer_float_data[4];
  dtostrf(interval, 6, 2, buffer_float_data);
  char buffer_float_can[4];
  dtostrf(interval_can, 6, 2, buffer_float_can);
  custom_logspeed.setValue((const char *)buffer_float_data, 4);
  custom_logspeed_can.setValue((const char *)buffer_float_can, 4);

  wm.setDarkMode(true);

  std::vector<const char *> menu = { "wifi", "wifinoscan", "info", "param", "close", "sep", "erase", "update", "restart", "exit" };
  wm.setMenu(menu);  // custom menu, pass vector

  //Serial.println(wm.getWiFiSSID(true));



  wm.setConfigPortalTimeout(120);
  wm.setConnectTimeout(10);
  wm.setConnectRetries(3);
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect(deviceName.c_str());  // password protected ap

  if (!res) {
    status = "timeout";
    //Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } else {
    status = "connected";
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }

  /* end Setup WifiManager */

#ifdef USEOTA
  ArduinoOTA.begin();
#endif

  /* start Setup Azure */

  // START: Naive URL Encode
  String url = "https://" + (String)azure_host + (String)azure_endpoint;
  url.replace(":", "%3A");
  url.replace("/", "%2F");
  //Serial.println(url);
  // END: Naive URL Encode
  // START: Create SAS
  // https://azure.microsoft.com/en-us/documentation/articles/service-bus-sas-overview/
  // Where to get secods since the epoch: local service, SNTP, RTC
  int expire = 1771338232; //until 2024
  String stringToSign = url + "\n" + expire;
  // START: Create signature
  Sha256.initHmac((const uint8_t *)azure_key.c_str(), 44);
  Serial.println("set up SHA256");
  Sha256.print(stringToSign);
  char *sign = (char *)Sha256.resultHmac();
  //Serial.println(String (Sha256.resultHmac));
  int signLen = 32;
  // END: Create signature
  // START: Get base64 of signature
  int encodedSignLen = base64_enc_len(signLen);
  char encodedSign[encodedSignLen];
  base64_encode(encodedSign, sign, signLen);
  String encodedSas = (String)encodedSign;
  // Naive URL encode
  encodedSas.replace("=", "%3D");
  //Serial.println(encodedSas);
  // END: Get base64 of signature
  // SharedAccessSignature
  fullSas = "sr=" + url + "&sig=" + encodedSas + "&se=" + expire + "&skn=" + azure_keyname;
  // END: create SAS
  //Serial.println("SAS below");
  //Serial.println(fullSas);
  client.setInsecure();
  /* end Setup Azure */

  /* start Setup CAN */
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;

  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  //initialize CAN Module
  //Serial.println("init can");
  ESP32Can.CANInit();
  /* end Setup CAN */

  db_enquery("select * from can_config", false);
  DeserializationError err = deserializeJson(doc_can_config, JSON_data.c_str());
  Serial.println(JSON_data);
  if (err) {
    Serial.print(F("deserializeJson() inside CAN Callback failed with code "));
    Serial.println(err.f_str());
  }

  /* start setup sensors */
  setup_vcp_sensors();
  setup_adc();
  setup_digitals();
  setup_env_sensors();
  /* end setup sensors */
  button_config.begin();

  // Attach callback.
  button_config.onPressed(on_pressed_button_config);
  button_config.onPressedFor(10000, on_pressed_button_config_reset);
  //button_config.onSequence(5 /* number of presses */, 1000 /* timeout */, start_http_server /* callback */);
  //start_http_server();
  //SPI.begin();
  //SPI.setFrequency(5000000);
  can_timer.setInterval(3000);
  can_timer.setCallback(can_timer_callback);

  data_timer.setInterval(interval * 1000);
  data_timer.setCallback(data_timer_callback);
  status_timer.setInterval(3000);
  status_timer.setCallback(status_timer_callback);
  timer_update.setCallback(check_update);
  timer_update.setInterval(upd_interval * 1000);
  //Start the timer
  can_timer.start();
  data_timer.start();
  status_timer.start();
  if (upd_interval > 0) timer_update.start();
  status = "ready";
  status_serial = "ready";
  doc_can["node"] = deviceName;

  if (!client.connect(azure_host.c_str(), 443)) {
    Serial.println("Error while connecting to Azure");
    delay(1000);
    status = "Error Azure->Config";
    //res = wm.autoConnect(deviceName.c_str());  // password protected ap
  }


  if (FirmwareVersionCheck()) {
    status = "OTA update";

    firmwareUpdate();
  }


  // Clear the buffer
  display.clearDisplay();
}


void loop() {


  if (!has_connected) server.handleClient();
  if (wm_nonblocking) wm.process();  // avoid delays() in loop when non-blocking and other long running code
  button_config.read();

#ifdef USEOTA
  ArduinoOTA.handle();
#endif

  oled_draw_text(status);



  CAN_frame_t rx_frame;
  //receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    recv_ct++;
    can_timer.update();
    JsonArray daten = doc_can_config["response"].as<JsonArray>();
    if (!array_has_value(rx_frame.MsgID)) can_ids[id_counter] = rx_frame.MsgID;
    id_counter++;
    if (id_counter > CAN_ID_ARR_SIZE) id_counter = 0;
    int can_id;
    uint8_t start;
    uint8_t length;
    float factor;
    float offset;
    for (JsonVariant value : daten) {
      JsonObject params = value.as<JsonObject>();
      can_id = params["can_id"].as<int>();
      //Serial.println("CAN ID JSON: "+String(can_id));
      if (can_id == rx_frame.MsgID) {

        String msg_name = params["msg"].as<String>();

        start = params["s"].as<int>();
        length = params["l"].as<int>();
        factor = params["f"].as<float>();
        offset = params["o"].as<float>();
        float val = extractNbits(rx_frame.data.u8, start, length, factor, offset);
        doc_can["CAN"][String(can_id)][msg_name] = val;
      }
    }

    if (recv_ct % 50 == 0) compareArrayToJSON();
  }


  data_timer.update();
  status_timer.update();

  /* start get sensor data */
  get_adc();
  get_digitals();
  get_temperatures();
  get_env_sensors();
  get_vcp_sensors();
  get_StrainSensorData();

  /* end get sensor data */

  if (!client.connected()) client.connect(azure_host.c_str(), 443);  //reconnection of azure cloud if connection is lost
  String response = "";
  if (client.available()) {
    response = client.readStringUntil('\n');
    if (response.startsWith("HTTP")) {
      http_status_code = response.substring(9, 13).toInt();
    }
  }

  while (Serial.available()) {

    String raw_json = Serial.readString();  // read the incoming data as string
    //Serial.println(raw_json);
    DynamicJsonDocument doc_command(9000);
    DeserializationError error = deserializeJson(doc_command, raw_json);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed inside Serial command: "));
      Serial.println(error.f_str());
      status = "deserialization error";
      return;
    }
    if (doc_command["command"].as<int>() == -1) {  //LOAD
      db_enquery(doc_command["sql"], true);
    }
    if (doc_command["command"].as<int>() == 1) {  //LOAD
      i2c_scan();
      get_parameters();
    }
    if (doc_command["command"].as<int>() == 2) {  //SAVE

      status = "save settings";
      preferences.begin("SLI", false);
      preferences.putString("azure_host", doc_command["data"]["host"].as<String>());
      preferences.putString("azure_endpoint", doc_command["data"]["endp"].as<String>());
      preferences.putString("azure_key", doc_command["data"]["key"].as<String>());
      preferences.putString("azure_keyname", doc_command["data"]["keyn"].as<String>());
      preferences.putFloat("logspeed_data", doc_command["data"]["spd"].as<float>());


      preferences.putBool("vcp1", doc_command["data"]["vcp"][0].as<bool>());
      preferences.putBool("vcp2", doc_command["data"]["vcp"][1].as<bool>());
      preferences.putBool("vcp3", doc_command["data"]["vcp"][2].as<bool>());
      preferences.putBool("vcp4", doc_command["data"]["vcp"][3].as<bool>());
      preferences.putBool("vcp5", doc_command["data"]["vcp"][4].as<bool>());
      preferences.putBool("vcp6", doc_command["data"]["vcp"][5].as<bool>());
      preferences.putBool("vcp7", doc_command["data"]["vcp"][6].as<bool>());
      preferences.putBool("vcp8", doc_command["data"]["vcp"][7].as<bool>());
      preferences.putBool("vcp9", doc_command["data"]["vcp"][8].as<bool>());
      preferences.putBool("vcp10", doc_command["data"]["vcp"][9].as<bool>());
      preferences.putBool("vcp11", doc_command["data"]["vcp"][10].as<bool>());
      preferences.putBool("vcp12", doc_command["data"]["vcp"][11].as<bool>());
      preferences.putBool("tc1", doc_command["data"]["tc"][0].as<bool>());
      preferences.putBool("tc2", doc_command["data"]["tc"][1].as<bool>());
      preferences.putBool("tc3", doc_command["data"]["tc"][2].as<bool>());
      preferences.putBool("tc4", doc_command["data"]["tc"][3].as<bool>());
      preferences.putBool("tc5", doc_command["data"]["tc"][4].as<bool>());
      preferences.putBool("tc6", doc_command["data"]["tc"][5].as<bool>());
      preferences.putBool("tc7", doc_command["data"]["tc"][6].as<bool>());
      preferences.putBool("tc8", doc_command["data"]["tc"][7].as<bool>());
      preferences.putBool("tc9", doc_command["data"]["tc"][8].as<bool>());
      preferences.putBool("tc10", doc_command["data"]["tc"][9].as<bool>());
      preferences.putBool("tc11", doc_command["data"]["tc"][10].as<bool>());

      preferences.end();
      get_parameters();
    }
    if (doc_command["command"].as<int>() == 3) {  //preview settings without permanent save in spiffs

      status = "preview settings";

      azure_host = doc_command["data"]["host"].as<String>();
      azure_endpoint = doc_command["data"]["endp"].as<String>();
      azure_key = doc_command["data"]["key"].as<String>();
      azure_keyname = doc_command["data"]["keyn"].as<String>();
      interval = doc_command["data"]["spd"].as<float>();


      vcp1 = doc_command["data"]["vcp"][0].as<bool>();
      vcp2 = doc_command["data"]["vcp"][1].as<bool>();
      vcp3 = doc_command["data"]["vcp"][2].as<bool>();
      vcp4 = doc_command["data"]["vcp"][3].as<bool>();
      vcp5 = doc_command["data"]["vcp"][4].as<bool>();
      vcp6 = doc_command["data"]["vcp"][5].as<bool>();
      vcp7 = doc_command["data"]["vcp"][6].as<bool>();
      vcp8 = doc_command["data"]["vcp"][7].as<bool>();
      vcp9 = doc_command["data"]["vcp"][8].as<bool>();
      vcp10 = doc_command["data"]["vcp"][9].as<bool>();
      vcp11 = doc_command["data"]["vcp"][10].as<bool>();
      vcp12 = doc_command["data"]["vcp"][11].as<bool>();
      tc1 = doc_command["data"]["tc"][0].as<bool>();
      tc2 = doc_command["data"]["tc"][1].as<bool>();
      tc3 = doc_command["data"]["tc"][2].as<bool>();
      tc4 = doc_command["data"]["tc"][3].as<bool>();
      tc5 = doc_command["data"]["tc"][4].as<bool>();
      tc6 = doc_command["data"]["tc"][5].as<bool>();
      tc7 = doc_command["data"]["tc"][6].as<bool>();
      tc8 = doc_command["data"]["tc"][7].as<bool>();
      tc9 = doc_command["data"]["tc"][8].as<bool>();
      tc10 = doc_command["data"]["tc"][9].as<bool>();
      tc11 = doc_command["data"]["tc"][10].as<bool>();


      //get_parameters();
    }

    if (doc_command["command"].as<int>() == 4) {  //save CAN settings to sql database on SoC ESP32

      Serial.println("received CAN CONFIG");
      status = "recv CAN Config";
      JsonObject root = doc_command.as<JsonObject>();
      for (JsonPair kv : root) {
        JsonObject sub1 = kv.value().as<JsonObject>();
        Serial.println(kv.key().c_str());
        for (JsonPair sub_msg : sub1) {
          Serial.println(sub_msg.key().c_str());
          Serial.println(sub_msg.value().as<float>());
        }
        //Serial.println(kv.value().as<char*>());
      }
    }
  }
}
