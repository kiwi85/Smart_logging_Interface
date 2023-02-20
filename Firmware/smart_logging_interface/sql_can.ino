
#include <SPI.h>
#include <FS.h>
#include "SD.h"
SPIClass sdSPI(VSPI);
#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCLK 18
#define SD_CS 0  //for final board


#define MAX_FILE_NAME_LEN 100
#define MAX_STR_LEN 500
#define DATABASE_NAME "/sd/data.db"
//#include <stdio.h>
//#include <stdlib.h>
#include <sqlite3.h>
/**
* defines and declarations for database
*/
sqlite3 *db1 = NULL;
sqlite3_stmt *res;

//char sql[1024];
int rc;
const char *tail;


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

// Open database 1
void save_can_config_to_sql(String can_config_json) {
  DynamicJsonDocument doc(11200);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, can_config_json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed inside save_can_config: "));
    Serial.println(error.f_str());
    return;
  }
  db_enquery("drop table if EXISTS can_config;",false);
  db_enquery("CREATE TABLE if not EXISTS can_config (can_id INT NOT NULL, msg TEXT NOT NULL,s INT,l INT,f NUMERIC,o NUMERIC)",false);

  open_db();
    const char *sql = "INSERT INTO can_config (can_id,msg,s,l,f,o) VALUES (?,?,?,?,?,?)";
    rc = sqlite3_prepare_v2(db1, sql, strlen(sql), &res, &tail);
    if (rc != SQLITE_OK) {
      Serial.printf("ERROR preparing sql for can values: %s\n", sqlite3_errmsg(db1));
      sqlite3_close(db1);
      return;
    }
 
    // Loop through the keys in the document
    for (JsonPair kvp : doc.as<JsonObject>()) {
      // Get the key as a string
      String can_id = kvp.key().c_str();
      // Get the object associated with the key
      JsonObject obj = kvp.value();
      // Loop through the keys in the object
      for (JsonPair subkvp : obj) {
        // Get the sub-key and sub-object
        String msg = subkvp.key().c_str();
        JsonObject params = subkvp.value();
        // Print the key and value
        Serial.print(can_id);
        Serial.print(": ");
        Serial.print(msg);
        Serial.print(": ");
        Serial.print(params["s"].as<int>());
        Serial.print(": ");
        Serial.print(params["l"].as<int>());
        Serial.print(": ");
        Serial.print(params["f"].as<float>());
        Serial.print(": ");
        Serial.print(params["o"].as<float>());
        Serial.println();
        sqlite3_bind_int(res,1, can_id.toInt());
        sqlite3_bind_text(res, 2, subkvp.key().c_str(), strlen(subkvp.key().c_str()), SQLITE_STATIC);
        sqlite3_bind_int(res, 3, params["s"].as<int>());
        sqlite3_bind_int(res, 4, params["l"].as<int>());
        sqlite3_bind_double(res, 5, params["f"].as<float>());
        sqlite3_bind_double(res, 6, params["o"].as<float>());

        if (sqlite3_step(res) != SQLITE_DONE) {
      Serial.printf("ERROR executing stmt: %s\n", sqlite3_errmsg(db1));
      sqlite3_close(db1);
      return;
    }
    sqlite3_clear_bindings(res);
    rc = sqlite3_reset(res);
    if (rc != SQLITE_OK) {
      sqlite3_close(db1);
      return;
    }

        
      }
     
    }



    
    

    sqlite3_finalize(res);
    sqlite3_close(db1);
  
}

void setup_sd_db() {
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
}