#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <rdm6300.h>
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include "DFRobotDFPlayerMini.h"

#define MQTT_TOPIC_LEN   128
#define MQTT_PAYLOAD_LEN 512
#define MQTT_MAX_PACKET_SIZE 1024  // increase this
#include <PubSubClient.h>

Preferences prefer;
/* ===================== PIN CONGIG =====================*/
#define RDM6300_RX_PIN 13

#define GREEN_LED_PIN 15
#define RED_LED_PIN 4
#define OUTPUT_COFFEE 5
#define OUTPUT_TEA 18
/* INPUT */
#define INPUT_BUZZER 22
#define INPUT_USABLE 21
#define INPUT_COFFEE 19
#define INPUT_TEA 23

#define DFPLAYER_RX 16
#define DFPLAYER_TX 17
/* =================EIND ================================*/
/*For RFID*/
Rdm6300 rdm6300;
/*RFID EIND*/
/* DF MINI PLAYER : Audio Playear*/
DFRobotDFPlayerMini player;

/* Aydio Player Setup eind */
/*================== Audio Tracks ===================*/
// For SH 12
// #define AUDIO_HTTP_ERROR 1                    // HTTP error
// #define AUDIO_HTTP_ERROR_SUBMIT 2             // Submit HTTP error
// #define AUDIO_INVALID_JSON 3                  // Invalid JSON response
// #define AUDIO_LOW_BALANCE 4                   // Member has low balance
// #define AUDIO_MACHINE_NOT_RESPONSE 5          // Machine didn't respond
// #define AUDIO_MACHINE_NOTFOUND_IN_SERVER 6    // Machine not found on server
// #define AUDIO_MACHINE_STOCK_NOT_FOUND 7       // Coffee machine stock not found
// #define AUDIO_MASTER_CARD_LIMIT 8             // Master card limit reached
// #define AUDIO_MQTT_CONNECT 9                  // MQTT connected
// #define AUDIO_MQTT_FAILED 10                  // MQTT connection failed
// #define AUDIO_NO_ACCESS 11                    // No access
// #define AUDIO_NO_PERMISSION_WITHOUT_MEMBER 12 // No permission without member
// #define AUDIO_PLS_REMOVE_CARD 13              // Please remove card
// #define AUDIO_PREPARING_COFFEE 14             // Preparing coffee
// #define AUDIO_PREPARING_TEA 15                // Preparing tea
// #define AUDIO_SELECT_TIME_OUT 16              // Selection timeout
// #define AUDIO_SPECIAL_CARD_LIMIT 17           // Special card limit reached
// #define AUDIO_SPECIAL_CARD_NO_PERMISSION 18   // Special card no permission
// #define AUDIO_WELCOME_TO_SH 19                // Welcome to office/site


// #define AUDIO_SUBMIT_DONE 21                  // Purchase submitted successfully
// #define AUDIO_TEA_READY 23                    // Tea is ready
// #define AUDIO_UNUSABLE 24                     // Machine unusable
// #define AUDIO_SUBMIT_MEMBER_NOT_FOUND 25      // Member not found
// #define AUDIO_WALLET_NOT_FOUND 26             // Wallet not found
// #define AUDIO_WAITING_FOR_SELECTION 28        // Waiting for selection
// #define AUDIO_WIFI_NOT_CONNECT 29             // WiFi not connected
// #define AUDIO_AI_CMD 30                       // AI command received
// #define AUDIO_AUTO_CANCEL 31                  // Auto cancel
// #define AUDIO_CHECKING 32                     // Checking card
// #define AUDIO_COFFEE_READY 33                 // Coffee is ready
// #define AUDIO_CUP_NOT_FOUND_IN_SERVER 35      // Cup not found in server
// #define AUDIO_CUP_OUT_OF_STOCK 36             // Cup out of stock
// #define AUDIO_EMPLOYEE_LIMIT 37               // Employee limit reached
// #define AUDIO_EMPLOYEE_NO_PERMISSION 38       // Employee no permission
// #define AUDIO_THANK_YOU_FOR_ST_W_SH 39

// for SH 03 ======================================
// unsigned int AUDIO_HTTP_ERROR = 34;                    // HTTP error
// unsigned int AUDIO_HTTP_ERROR_SUBMIT = 35;             // Submit HTTP error
// unsigned int AUDIO_INVALID_JSON = 36;                  // Invalid JSON response
// unsigned int AUDIO_LOW_BALANCE = 37;                   // Member has low balance
// unsigned int AUDIO_MACHINE_NOT_RESPONSE = 1;          // Machine didn't respond
// unsigned int AUDIO_MACHINE_NOTFOUND_IN_SERVER = 2;    // Machine not found on server
// unsigned int AUDIO_MACHINE_STOCK_NOT_FOUND = 3;       // Coffee machine stock not found
// unsigned int AUDIO_MASTER_CARD_LIMIT = 4;             // Master card limit reached
// unsigned int AUDIO_MQTT_CONNECT = 5;                  // MQTT connected
// unsigned int AUDIO_MQTT_FAILED = 6;                  // MQTT connection failed
// unsigned int AUDIO_NO_ACCESS = 7;                    // No access
// unsigned int AUDIO_NO_PERMISSION_WITHOUT_MEMBER = 8; // No permission without member
// unsigned int AUDIO_PLS_REMOVE_CARD = 9;              // Please remove card
// unsigned int AUDIO_PREPARING_COFFEE = 10;             // Preparing coffee
// unsigned int AUDIO_PREPARING_TEA = 11;                // Preparing tea
// unsigned int AUDIO_SELECT_TIME_OUT = 12;              // Selection timeout
// unsigned int AUDIO_SPECIAL_CARD_LIMIT = 13;           // Special card limit reached
// unsigned int AUDIO_SPECIAL_CARD_NO_PERMISSION = 14;   // Special card no permission
// unsigned int AUDIO_WELCOME_TO_SH = 38;                // Welcome to office/site
// unsigned int AUDIO_USER_NOT_FOUND = 19;               // USER NOT FOUND


// unsigned int AUDIO_SUBMIT_DONE = 15;                  // Purchase submitted successfully
// unsigned int AUDIO_TEA_READY = 17;                    // Tea is ready
// unsigned int AUDIO_UNUSABLE = 18;                     // Machine unusable
// unsigned int AUDIO_SUBMIT_MEMBER_NOT_FOUND = 16;      // Member not found
// unsigned int AUDIO_WALLET_NOT_FOUND = 20;             // Wallet not found
// unsigned int AUDIO_WAITING_FOR_SELECTION = 22;        // Waiting for selection || select item coffee or tea
// unsigned int AUDIO_WIFI_NOT_CONNECT = 23;             // WiFi not connected
// unsigned int AUDIO_AI_CMD = 24;                       // AI command received
// unsigned int AUDIO_AUTO_CANCEL = 25;                  // Auto cancel
// unsigned int AUDIO_CHECKING = 26;                     // Checking card
// unsigned int AUDIO_COFFEE_READY = 27;                 // Coffee is ready
// unsigned int AUDIO_CUP_NOT_FOUND_IN_SERVER = 29;      // Cup not found in server
// unsigned int AUDIO_CUP_OUT_OF_STOCK = 30;             // Cup out of stock
// unsigned int AUDIO_EMPLOYEE_LIMIT = 31;               // Employee limit reached
// unsigned int AUDIO_EMPLOYEE_NO_PERMISSION = 32;     // Employee no permission
// unsigned int AUDIO_THANK_YOU_FOR_ST_W_SH = 39;

// For SH8 Super Home 8 Audio ALSO SH5 SH05 Supper Home 5 , SH04 SH4 Supper Home 4 , SH07 SH7 Supper Home06 
// unsigned int AUDIO_HTTP_ERROR = 9;                    // HTTP error
// unsigned int AUDIO_HTTP_ERROR_SUBMIT = 10;             // Submit HTTP error
// unsigned int AUDIO_INVALID_JSON = 11;                  // Invalid JSON response
// unsigned int AUDIO_LOW_BALANCE = 12;                   // Member has low balance
// unsigned int AUDIO_MACHINE_NOT_RESPONSE = 14;          // Machine didn't respond
// unsigned int AUDIO_MACHINE_NOTFOUND_IN_SERVER = 13;    // Machine not found on server
// unsigned int AUDIO_MACHINE_STOCK_NOT_FOUND = 15;       // Coffee machine stock not found
// unsigned int AUDIO_MASTER_CARD_LIMIT = 16;             // Master card limit reached
// unsigned int AUDIO_MQTT_CONNECT = 17;                  // MQTT connected
// unsigned int AUDIO_MQTT_FAILED = 18;                  // MQTT connection failed
// unsigned int AUDIO_NO_ACCESS = 19;                    // No access
// unsigned int AUDIO_NO_PERMISSION_WITHOUT_MEMBER = 20; // No permission without member
// unsigned int AUDIO_PLS_REMOVE_CARD = 21;              // Please remove card
// unsigned int AUDIO_PREPARING_COFFEE = 22;             // Preparing coffee
// unsigned int AUDIO_PREPARING_TEA = 23;                // Preparing tea
// unsigned int AUDIO_SELECT_TIME_OUT = 24;              // Selection timeout
// unsigned int AUDIO_SPECIAL_CARD_LIMIT = 25;           // Special card limit reached
// unsigned int AUDIO_SPECIAL_CARD_NO_PERMISSION = 26;   // Special card no permission
// unsigned int AUDIO_WELCOME_TO_SH = 36;                // Welcome to office/site
// unsigned int AUDIO_USER_NOT_FOUND = 32;               // USER NOT FOUND


// unsigned int AUDIO_SUBMIT_DONE = 27;                  // Purchase submitted successfully
// unsigned int AUDIO_TEA_READY = 29;                    // Tea is ready
// unsigned int AUDIO_UNUSABLE = 31;                     // Machine unusable
// unsigned int AUDIO_SUBMIT_MEMBER_NOT_FOUND = 28;      // Member not found
// unsigned int AUDIO_WALLET_NOT_FOUND = 33;             // Wallet not found
// unsigned int AUDIO_WAITING_FOR_SELECTION = 35;        // Waiting for selection || select item coffee or tea
// unsigned int AUDIO_WIFI_NOT_CONNECT = 37;             // WiFi not connected
// unsigned int AUDIO_AI_CMD = 1;                       // AI command received
// unsigned int AUDIO_AUTO_CANCEL = 38;                  // Auto cancel
// unsigned int AUDIO_CHECKING = 2;                     // Checking card
// unsigned int AUDIO_COFFEE_READY = 3;                 // Coffee is ready
// unsigned int AUDIO_CUP_NOT_FOUND_IN_SERVER = 5;      // Cup not found in server
// unsigned int AUDIO_CUP_OUT_OF_STOCK = 4;             // Cup out of stock
// unsigned int AUDIO_EMPLOYEE_LIMIT = 6;               // Employee limit reached
// unsigned int AUDIO_EMPLOYEE_NO_PERMISSION = 7;     // Employee no permission
// unsigned int AUDIO_THANK_YOU_FOR_ST_W_SH = 30;

/*.............................. Sound Serial for Supper Home 06 SH06 SH6*/
unsigned int AUDIO_HTTP_ERROR = 11;                    // HTTP error
unsigned int AUDIO_HTTP_ERROR_SUBMIT = 12;             // Submit HTTP error
unsigned int AUDIO_INVALID_JSON = 13;                  // Invalid JSON response
unsigned int AUDIO_LOW_BALANCE = 14;                   // Member has low balance
unsigned int AUDIO_MACHINE_NOT_RESPONSE = 16;          // Machine didn't respond
unsigned int AUDIO_MACHINE_NOTFOUND_IN_SERVER = 15;    // Machine not found on server
unsigned int AUDIO_MACHINE_STOCK_NOT_FOUND = 17;       // Coffee machine stock not found
unsigned int AUDIO_MASTER_CARD_LIMIT = 18;             // Master card limit reached
unsigned int AUDIO_MQTT_CONNECT = 19;                  // MQTT connected
unsigned int AUDIO_MQTT_FAILED = 20;                  // MQTT connection failed
unsigned int AUDIO_NO_ACCESS = 9;                    // No access
unsigned int AUDIO_NO_PERMISSION_WITHOUT_MEMBER = 21; // No permission without member
unsigned int AUDIO_PLS_REMOVE_CARD = 22;              // Please remove card
unsigned int AUDIO_PREPARING_COFFEE = 23;             // Preparing coffee
unsigned int AUDIO_PREPARING_TEA = 24;                // Preparing tea
unsigned int AUDIO_SELECT_TIME_OUT = 25;              // Selection timeout
unsigned int AUDIO_SPECIAL_CARD_LIMIT = 26;           // Special card limit reached
unsigned int AUDIO_SPECIAL_CARD_NO_PERMISSION = 27;   // Special card no permission
unsigned int AUDIO_WELCOME_TO_SH = 36;                // Welcome to office/site
unsigned int AUDIO_USER_NOT_FOUND = 32;               // USER NOT FOUND


unsigned int AUDIO_SUBMIT_DONE = 30;                  // Purchase submitted successfully
unsigned int AUDIO_TEA_READY = 10;                    // Tea is ready
unsigned int AUDIO_UNUSABLE = 31;                     // Machine unusable
unsigned int AUDIO_SUBMIT_MEMBER_NOT_FOUND = 29;      // Member not found
unsigned int AUDIO_WALLET_NOT_FOUND = 33;             // Wallet not found
unsigned int AUDIO_WAITING_FOR_SELECTION = 35;        // Waiting for selection || select item coffee or tea
unsigned int AUDIO_WIFI_NOT_CONNECT = 37;             // WiFi not connected
unsigned int AUDIO_AI_CMD = 1;                       // AI command received
unsigned int AUDIO_AUTO_CANCEL = AUDIO_NO_ACCESS;                  // Auto cancel
unsigned int AUDIO_CHECKING = 2;                     // Checking card
unsigned int AUDIO_COFFEE_READY = 3;                 // Coffee is ready
unsigned int AUDIO_CUP_NOT_FOUND_IN_SERVER = 5;      // Cup not found in server
unsigned int AUDIO_CUP_OUT_OF_STOCK = 4;            // Cup out of stock
unsigned int AUDIO_EMPLOYEE_LIMIT = 6;               // Employee limit reached
unsigned int AUDIO_EMPLOYEE_NO_PERMISSION = 7;     // Employee no permission
unsigned int AUDIO_THANK_YOU_FOR_ST_W_SH = 30;
//....................................................................................................................
/*Eind Audio List*/



/*Http Server Setup*/
//String Chipid = String(ESP.getEfuseMac(), DEC);


// String Chipid = "1";//5 means sh3 coffee machine data base ID  . id 1 for SH8
const char* api_secret = "put your api secret here"; /*" "*/
String url_1 = "put your check card status URL here";
String url_2 = "put your check card status URL here";

String sub_url_1 = "put your submit purchase URL here";
String sub_url_2 = "put your submit purchase URL here";

bool reSubmit = false,FS_Faulty_update_requied = false;

/* Server Setup Eind*/
String ssid = "Coffee-M:SH08";
WiFiManager wm;
const int ST_LED = 2;
volatile bool led_ST = false;
#define machine_slug "coffee-machine"
String MY_MACHINE_TYPE_ID = "1";
String MY_MACHINE_ID = "2";// 2 only for sh8 rooftop coffee machine // 8 for sh7 //12 for rooftop HS11 , 10 for sh6 ,,,,, //6 for sh4 , 5 means sh3 coffee machine data base ID  . id 1 for SH8
// ✅ THE WORKING URL
const char* otaApiUrl = "put your OTA API URL here"; 
const char* currentFirmwareVersion = "1.0.1"; 

// ........................................................... MQTT Setup.......................................................................

QueueHandle_t mqttQueue;

typedef struct {
  char topic[MQTT_TOPIC_LEN];
  char payload[MQTT_PAYLOAD_LEN];
  bool retained;
} mqtt_msg_t;


unsigned long mqtttryTime = 0;
unsigned long mqtttryConnectDelay = 30000;
// ====== MQTT Broker ====== 
const char* mqttServer = "put your MQTT broker URL here"; /*" "*/
const int mqttPort = 1883;

// MQTT Username/Password
const char* mqttUser = "put your MQTT username here";
const char* mqttPassword = "put your MQTT password here";
String infoTopic = "devices/" + MY_MACHINE_ID + "/fs";
String eventTopic = "devices/" + MY_MACHINE_ID + "/event";
String statusTopic = "devices/" + MY_MACHINE_ID + "/status";

// ✅ GLOBAL TOPIC
String globalUpdateTopic = "ota/coffee/update"; 


// MQTT Client
WiFiClient espClient;
PubSubClient client(espClient);

// .........................................................................MQTT Setup End..............................................................


// WiFi reconnection timing constants
#define RECONNECT_ATTEMPTS      5
#define ATTEMPT_SPACING_MS      5000
#define RETRY_CYCLE_DELAY_MS    60000  // 1 minutes
#define CONNECT_TRY_MS          10000

// WiFi reconnection state variables
volatile bool rtc_offlineRestarted = false;
volatile unsigned int reconnectAttempts = 0;
volatile unsigned long lastAttemptMs = 0;
volatile unsigned long lastCycleStartMs = 0;
volatile unsigned long nextSyncDueMs = 0;
volatile bool ota_request = false;

static const char* kHost = "put your MQTT broker URL here";
bool dns_ok() {
  IPAddress ip;
  int r = WiFi.hostByName(kHost, ip);
  if (r != 1) {
    Serial.println("[NET] DNS lookup failed; skipping HTTP this cycle.");
    return false;
  }
  return true;
}

// --------- Safe WDT kick (only for watched tasks) ---------
/* Safe Duel Core Handale*/
// ---------- Net scheduling & backoff (like glass door) ----------
TaskHandle_t core0Handle = NULL;
TaskHandle_t core1Handle = NULL;
void reconnectWiFi();
void updateWifiLed();

inline void wdt_kick_safe() {
  TaskHandle_t me = xTaskGetCurrentTaskHandle();
  if (me == core0Handle || me == core1Handle) {
    esp_task_wdt_reset();
  }
}
void core0Task(void *parameter);
void core1Task(void *parameter);
// Mqtt Setup 
// ====== Callback ======
bool mqttPublishQueue(const char* topic, const char* payload, bool retained) {
  if (mqttQueue == NULL) return false;

  mqtt_msg_t msg;
  mqtt_msg_t dump;   // ✅ declare dump

  memset(&msg, 0, sizeof(msg));

  strncpy(msg.topic, topic, MQTT_TOPIC_LEN - 1);
  strncpy(msg.payload, payload, MQTT_PAYLOAD_LEN - 1);
  msg.retained = retained;

  // Try to send
  if (xQueueSend(mqttQueue, &msg, 0) != pdTRUE) {
    // Queue full → drop oldest
    xQueueReceive(mqttQueue, &dump, 0);
    // Retry send
    if (xQueueSend(mqttQueue, &msg, 0) != pdTRUE) {
      return false; // still failed
    }
  }

  return true;
}
void update(const String& msg) {
  if (mqttPublishQueue(eventTopic.c_str(), msg.c_str(), false)) {
    Serial.println("[MQTT] Feedback update queued: " + eventTopic);
  } else {
    Serial.println("[MQTT] Feedback update FAILED: " + eventTopic);
  }
}
void handleGet() {
  update("handleGet");
}
void handleReset(){
  update("Reset");
  ESP.restart();
}
void giveCoffee(){
  update("giveCoffee");
}
void giveTea(){

  update("giveTea");
}
void handleRun(){
  update("handleRun");
}
void handleStop(){
  update("handleStop");
}
void loadAudio(String key,uint32_t val){
  prefer.begin("state",false);
  prefer.putUInt(key.c_str(), val);
  prefer.end();
}
void loadVolume(uint32_t val){
  player.volume(val);
  prefer.begin("state",false);
  prefer.putUInt("volume", val);
  prefer.end();
  update("volume:" + String(val));
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String currentTopic = String(topic); // Convert to String for comparison
  Serial.print("[MQTT] Topic: ");
  Serial.println(currentTopic);

  // -------- Payload to String ----------
  String jsonStr;
  for (unsigned int i = 0; i < length; i++) {
    jsonStr += (char)payload[i];
  }
  Serial.print("[MQTT] Payload: ");
  Serial.println(jsonStr);

  // --- 1. ✅ Handle Global Broadcast Topic ---
  if (currentTopic == globalUpdateTopic) {
    Serial.println("[GLOBAL CMD] Broadcast update received!");
    update("Broadcast OTA check triggered.");
    ota_request = true;
    return; // Exit early
  }

  // -------- JSON Parse ----------
  DynamicJsonDocument doc(4096);// increase if cards list is big
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    Serial.print("[MQTT] JSON Error: ");
    Serial.println(error.c_str());
    return;
  }

  // -------- CMD Read ----------
  const char* cmd = doc["cmd"];
  if (!cmd) {
    Serial.println("[MQTT] cmd missing");
    return;
  }

  String command = String(cmd);
  command.toLowerCase();
  // -------- CMD ROUTER ----------
  if(command == "volume"){
    loadVolume(doc["volume"] | 20);
  }else if(command == "set_audio"){
    loadAudio(doc["key"],doc["val"]);
  }else if (command == "coffee") {
    giveCoffee();
  }
  else if (command == "tea") {
    giveTea();
  }
  else if (command == "run") {
    handleRun();
  }else if(command == "stop"){
    handleStop();
  }
  else if (command == "update") { // ✅ Added OTA Trigger
    update("Checking for OTA Update...");
    ota_request = true;
  }
  else if (command == "reset") {
    handleReset();
  }
  else if (command == "get") {
    handleGet();
  }else if(command == "restart"){
    update("Restarting....");
    ESP.restart();
  }else if(command == "format"){
    if (LittleFS.format()){
      Serial.println("LittleFS formated.");
      update("LittleFS formated.");
    }else{
      update("LittleFS Can't format ESP Problems.");
      Serial.println("LittleFS Can't format ESP Problems.");
    }
  }
  else {
    Serial.print("[MQTT] Unknown cmd: ");
    Serial.println(command);
  }
}
void connectMQTT() {
  unsigned long now = millis();
  if(now - mqtttryTime >= mqtttryConnectDelay){
    mqtttryTime = now;
    if (!client.connected()) {

      Serial.print("Connecting to MQTT... ");
      if (client.connect(MY_MACHINE_ID.c_str(), mqttUser, mqttPassword,
                        statusTopic.c_str(), 1, true, "offline")) {
        Serial.println("connected");

        // Publish online status (retained)
        client.publish(statusTopic.c_str(), "online", true);
        Serial.print("Status sent to topic: ");
        Serial.println(statusTopic);

        // Subscribe to command topic
        String cmdTopic = "devices/" + MY_MACHINE_ID + "/cmd";
        client.subscribe(cmdTopic.c_str());
        
        // ✅ SUBSCRIBE TO GLOBAL TOPIC
        client.subscribe(globalUpdateTopic.c_str());
        Serial.print("Subscribed to global topic: ");
        Serial.println(globalUpdateTopic);

        if(FS_Faulty_update_requied){
          FS_Faulty_update_requied = false;
          client.publish(infoTopic.c_str(), "LittleFS Faulty", true); // retained
          Serial.println("[MQTT] LittleFS fault reported to topic: " + infoTopic);
        }else{
          client.publish(infoTopic.c_str(), "LittleFS Ok", true); // retained
          Serial.println("[MQTT] LittleFS Ok reported to topic: " + infoTopic);
        }
        Serial.print("Subscribed to topic: ");
        Serial.println(cmdTopic);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" retrying in 30 seconds");
      }
    }
  }
    
}
// ================== WiFi reconnect + LED ==================

void reconnectWiFi() {
  wl_status_t st = WiFi.status();

  if (st == WL_CONNECTED) {
    if (wm.getConfigPortalActive()) wm.stopConfigPortal();
    updateWifiLed();
    rtc_offlineRestarted = false;
    reconnectAttempts = 0;
    lastAttemptMs = 0;
    lastCycleStartMs = 0;
    return;
  }

  updateWifiLed();

  if (!wm.getConfigPortalActive()) {
    Serial.println("[WM] Disconnected → starting config portal (SDL-...).");
    wm.startConfigPortal(ssid.c_str());
  }
  wm.process();

  // Pause STA while a user is in the portal
  int clients = WiFi.softAPgetStationNum();
  if (clients > 0) {
    esp_wifi_disconnect();
    WiFi.disconnect(false, false);
    reconnectAttempts = 0;
    lastAttemptMs = 0;
    return;
  }

  unsigned long now = millis();

  // ✅ MODIFIED: Simple retry attempt counting without hourly batch cooldown
  // Start new cycle if needed
  if (lastCycleStartMs == 0 || reconnectAttempts >= RECONNECT_ATTEMPTS) {
    if (reconnectAttempts >= RECONNECT_ATTEMPTS && (now - lastCycleStartMs < RETRY_CYCLE_DELAY_MS)) {
      return; // Wait before next cycle
    }
    lastCycleStartMs = now;
    reconnectAttempts = 0;
    Serial.printf("[WIFI] Starting new reconnect cycle (attempt %u/%u)...\n", reconnectAttempts + 1, RECONNECT_ATTEMPTS);
  }

  // Attempt connection if spacing is satisfied
  if (now - lastAttemptMs >= ATTEMPT_SPACING_MS) {
    lastAttemptMs = now;
    reconnectAttempts++;
    Serial.printf("[WIFI] Connection attempt %u/%u...\n", reconnectAttempts, RECONNECT_ATTEMPTS);

    esp_wifi_disconnect();
    WiFi.begin();
    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < CONNECT_TRY_MS) {
      wm.process();
      wdt_kick_safe();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("[WIFI] ✅ Connected! IP: %s\n", WiFi.localIP().toString().c_str());
      updateWifiLed();
      if (wm.getConfigPortalActive()) wm.stopConfigPortal();

      // Immediately schedule sync and reset counters
      rtc_offlineRestarted = false;
      nextSyncDueMs = millis() + 2000;
      ota_request = true; // ✅ Force OTA check on Reconnect
      reconnectAttempts = 0;
      lastAttemptMs = 0;
      lastCycleStartMs = 0;
      return;
    }

    Serial.println("[WIFI] Connection attempt failed.");
  }
}

void updateWifiLed() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ST_LED, HIGH);  // ON when connected
    led_ST = true;
  } else {
    digitalWrite(ST_LED, LOW);   // OFF when not connected
    led_ST = false;
  }
}
void infoPlay(int message_code){
  if(message_code == 1) { //'message_code' 	=> 1,  //Member in Auto Cancel 
      player.play(AUDIO_AUTO_CANCEL);
      // Play error audio here
  }else if(message_code == 3){//'message_code' 	=> 3,  //Member have not enouth balance. Minimum balance need 20 taka 
      player.play(AUDIO_LOW_BALANCE);
  }else if(message_code == 4){//'message_code' 	=> 4,  //Member wallet not found
      player.play(AUDIO_WALLET_NOT_FOUND);
  }else if(message_code == 5){// 'message_code' 	=> 5,  //Employee reach the limit of taking coffee/tea for today
      player.play(AUDIO_EMPLOYEE_LIMIT);
  }else if(message_code == 6){//'message_code' 	=> 6,  //Employee have no permission
      player.play(AUDIO_EMPLOYEE_NO_PERMISSION);
  }else if(message_code == 7){//'message_code' 	=> 7,  //Special Card reach the limit of taking coffee/tea for today
      player.play(AUDIO_SPECIAL_CARD_LIMIT);
  }/*else if(message_code == 8){//'message_code' 	=> 8,  //Special Card Bearer can take coffee/tea
      player.Play(AUDIO_NO_ACCESS);
  }*/else if(message_code == 9){// 'message_code' 	=> 9,  //Special Card Bearer have no permission
      player.play(AUDIO_SPECIAL_CARD_NO_PERMISSION);
  }else if(message_code == 10){// 'message_code' 	=> 10, //Master Card reach the limit of taking coffee/tea for today
      player.play(AUDIO_MASTER_CARD_LIMIT);
  }/*else if(message_code == 11){// 'message_code' 	=> 11, //Master Card Bearer can take coffee/tea
      player.Play(AUDIO_NO_ACCESS);
  }*/else if(message_code == 12){//'message_code' 	=> 12, //No User found
      player.play(AUDIO_USER_NOT_FOUND);
  }else if(message_code == 13){// 'message_code' 	=> 13, //Coffee machine have no permission without members!
      player.play(AUDIO_NO_PERMISSION_WITHOUT_MEMBER);
  }else if(message_code == 14){//'message_code' 	=> 14, //Coffee/Paper Cup Out of stock
      player.play(AUDIO_CUP_OUT_OF_STOCK);
  }else if(message_code == 15){// 'message_code' 	=> 15, //Coffee Machine Stock not found
      player.play(AUDIO_MACHINE_STOCK_NOT_FOUND);
  }else if(message_code == 16){// 'message_code' 	=> 16, //Coffee/Paper Cup not found
      player.play(AUDIO_CUP_NOT_FOUND_IN_SERVER);
  }else if(message_code == 17){// 'message_code' 	=> 17, //Coffee Machine not found
      player.play(AUDIO_MACHINE_NOTFOUND_IN_SERVER);
  }
}
void selection(int pin){
  digitalWrite(pin, HIGH);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  digitalWrite(pin, LOW);
  Serial.println("Selection Work");
}
int isDone(){
  Serial.println("isDone Call");
  unsigned long startTime = millis();
  // delay(BUZZER_READING_DELAY);
  while (millis() - startTime < 26000) { // 26s timeout
      // int val = analogRead(INPUT_BUZZER);
      // Serial.println(val);
      wdt_kick_safe();
      //other machine
      // if (!digitalRead(INPUT_BUZZER)){
      if (digitalRead(INPUT_BUZZER)) {
          if(millis() - startTime < 12000){ // false detect
              if(millis() - startTime < 1500){
                continue;
              }
              return 2;
          }else {
              return 1;
          }
      }
      vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  Serial.println("Wating Timeout for Submit");
  return 0;
}
void submitPurchase(String urlx,String cardNum, String userType, String userId,String walletType, String walletId,String balance, int stockQty,String purchaseType){
    // player.play(AUDIO_HTTP_ERROR_SUBMIT);
    // return;
    if(!led_ST){
        Serial.println("[Submit] WiFi not connected");
        player.play(AUDIO_WIFI_NOT_CONNECT); 
        return;
    }
    HTTPClient http;
    http.begin(urlx);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData =
        "api_secret=" + String(api_secret) +
        "&card_number=" + cardNum +
        "&coffee_machine_id=" + MY_MACHINE_ID +
        "&user_type=" + userType +
        "&user_id=" + userId +
        "&wallet_type=" + walletType +
        "&wallet_id=" + walletId +
        "&balance=" + balance +
        "&purchase_type=" + purchaseType +
        "&stock_quantity=" + String(stockQty);

    Serial.println("[Submit] Posting: " + postData);
    int code = http.POST(postData);

    if (code != 200) {
        Serial.println("[Submit] HTTP Error: " + String(code));
        http.end();

        if(!reSubmit){
            reSubmit = true;
            player.play(AUDIO_HTTP_ERROR_SUBMIT); 
            if(urlx == sub_url_1){
              submitPurchase(sub_url_2,cardNum,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
            }else{
              submitPurchase(sub_url_1,cardNum,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
            }
        }
        reSubmit = false;
        return;
    }
    reSubmit = false;
    String payload = http.getString();
    http.end();
    Serial.println("[Submit] Response: " + payload);

    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, payload)) {
        player.play(AUDIO_INVALID_JSON); 
        Serial.println("[Submit] JSON Decode Error");
        return;
    }

    if (String(doc["status"]) == "success") {
        // player.play(SUBMIT_DONE); 
        Serial.println("[Submit] Purchase successful!");
        player.play(AUDIO_THANK_YOU_FOR_ST_W_SH); 
    } else {
        int message_code = doc["message_code"];
        Serial.println("[Submit] Server rejected purchase");
        String msg = doc["message"] | "";
        Serial.println(msg);
        if(message_code == 38){//'message_code' 	=> 38, //Member information not found!
            player.play(AUDIO_SUBMIT_MEMBER_NOT_FOUND);
        }else{
            player.play(AUDIO_HTTP_ERROR_SUBMIT); 
        }
    }
}
void cardCK(String urlx,String card){
  HTTPClient http;
  http.begin(urlx);
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  String postData = "api_secret="+String(api_secret)+"&card_number="+card+"&coffee_machine_id="+String(MY_MACHINE_ID);
  int code = http.POST(postData);
  Serial.println("Http response code: " + String(code));
  if(code != 200) { 
    if(urlx == url_2){
      http.end(); player.play(AUDIO_HTTP_ERROR); 
      Serial.println("Http Error code: " + String(code));
      return;
    }else{
      cardCK(url_2,card);
    }
  }
  String payload = http.getString();
  http.end();
  Serial.print("POST DATA :");
  Serial.println(postData);

  Serial.println("Payload :" + payload);

  StaticJsonDocument<512> doc;
  if(deserializeJson(doc,payload)){
      player.play(AUDIO_INVALID_JSON); 
      return;
  }
  String status = doc["status"] | "";

  if(status=="success") {
      player.play(AUDIO_WAITING_FOR_SELECTION);
      String userType = (const char*) doc["user_type"];
      String userId = (const char*) doc["user_id"];
      String walletType = (const char*) doc["wallet_type"];
      String walletId = (const char*) doc["wallet_id"];
      String balance = (const char*) doc["balance"];
      int stockQty = doc["stock_quantity"] | 0;
      String purchaseType;
      Serial.println("success");
      bool selectionMade = false;
      unsigned long startTime = millis();
      while(millis()-startTime<15000) {
        wdt_kick_safe();
        if(!digitalRead(INPUT_COFFEE)) {
            player.play(AUDIO_PREPARING_COFFEE);
            purchaseType = "coffee";
            Serial.println("Select Coffee");
            selection(OUTPUT_COFFEE);
            if(isDone() == 1){
                player.play(AUDIO_COFFEE_READY); 
                Serial.println("Coffee Rady");
                vTaskDelay(600 / portTICK_PERIOD_MS);
                if(urlx == url_1){
                  submitPurchase(sub_url_1,card,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
                }else{
                  submitPurchase(sub_url_2,card,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
                }
            }else if(isDone() == 2){
                player.play(AUDIO_UNUSABLE);
            }else{
                player.play(AUDIO_MACHINE_NOT_RESPONSE); 
                Serial.println("Coffee Give but Machine don't response");
            }
            selectionMade = true;
            return;
        } else if(!digitalRead(INPUT_TEA)) {
            player.play(AUDIO_PREPARING_TEA); 
            purchaseType = "tea";
            Serial.println("Select Tea");
            selection(OUTPUT_TEA);
            if(isDone() == 1){
                player.play(AUDIO_TEA_READY); 
                Serial.println("Tea Rady");
                vTaskDelay(600 / portTICK_PERIOD_MS);
                if(urlx == url_1){
                  submitPurchase(sub_url_1,card,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
                }else{
                  submitPurchase(sub_url_2,card,userType,userId,walletType,walletId,balance,stockQty,purchaseType);
                }
            }else if(isDone() == 2){
                player.play(AUDIO_UNUSABLE);
            }else{
                player.play(AUDIO_MACHINE_NOT_RESPONSE);
                Serial.println("Coffee Give but Machine don't response");
            }
            selectionMade = true;
            return;
        }
      }
      if(selectionMade) return;
      player.play(AUDIO_SELECT_TIME_OUT);
      Serial.println("Select Timeout");
  } else{
      // player.Play(LOW_BALANCE);
      int message_code = doc["message_code"];
      String msg = doc["message"] | "";
      Serial.println("error " + msg);
      infoPlay(message_code);
  }
}
void executeOTA(String url) {
  WiFiClientSecure* client = new WiFiClientSecure;
  client->setInsecure();
  client->setTimeout(120000); // 2 Minute Timeout
  
  HTTPClient http;
  http.setReuse(true); // Keep connection alive

  Serial.println("[OTA] Downloading from: " + url);

  if (http.begin(*client, url)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      int contentLength = http.getSize();
      Serial.printf("[OTA] Firmware Size: %d bytes\n", contentLength);

      if (contentLength > 0) {
        if (Update.begin(contentLength)) {
          Serial.println(F("[OTA] Writing to Flash..."));
          
          // Manual Loop
          WiFiClient* stream = http.getStreamPtr();
          uint8_t buff[1280]; 
          size_t written = 0;
          unsigned long lastByteTime = millis();

          while (http.connected() && written < contentLength) {
            size_t size = stream->available();
            if (size) {
              int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
              Update.write(buff, c);
              written += c;
              lastByteTime = millis(); 
              esp_task_wdt_reset(); // Keep watchdog happy
            } else {
              if (millis() - lastByteTime > 30000) { // 30s stall limit
                 Serial.println("[OTA] Error: Connection Stalled.");
                 break;
              }
              delay(1);
            }
          }

          if (written == contentLength) {
            if (Update.end()) {
              Serial.println(F("✅ OTA Success! Rebooting..."));
              update("OTA Success. Rebooting...");
              delay(1000);
              ESP.restart();
            } else {
               Serial.printf("[OTA] Error: %d\n", Update.getError());
               update("OTA Error: " + String(Update.getError()));
            }
          } else {
             Serial.printf("[OTA] Short write: %d/%d\n", written, contentLength);
             update("OTA Failed: Download Incomplete");
          }
        } else {
          Serial.println(F("[OTA] Not enough space"));
        }
      } else {
        Serial.println(F("[OTA] Empty content length"));
      }
    } else {
      Serial.printf("[OTA] Download failed: %d\n", httpCode);
    }
    http.end();
  }
  delete client;
}
void checkAndPerformOTA() {
  if (!led_ST) return;
  Serial.println(F("[OTA] Connecting to API..."));
  
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String postData = "api_secret=" + String(api_secret) + 
                    "&machine_slug=" + String(machine_slug) + 
                    "&device_code=" + String(MY_MACHINE_ID) + 
                    "&machine_type_id=" + String(MY_MACHINE_TYPE_ID);

  if (http.begin(client, otaApiUrl)) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);

    Serial.print("[OTA] HTTP Code: "); 
    Serial.println(httpCode);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.print("[OTA] Response: "); Serial.println(payload);

      DynamicJsonDocument doc(4096);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error && doc["status"] == "success") {
        
        if (doc.containsKey("firmware") && !doc["firmware"].isNull()) {
            
            String serverTypeId = doc["firmware"]["machine_type_id"].as<String>();
            String serverVersion = doc["firmware"]["version_number"].as<String>();
            String downloadUrl = doc["file_url"].as<String>();
            String typeName = doc["firmware"]["machine_type_name"].as<String>();

            Serial.printf("[OTA] Server proposes: %s (Type ID: %s)\n", typeName.c_str(), serverTypeId.c_str());

            // Check if IDs match (String comparison)
            if (serverTypeId == String(MY_MACHINE_TYPE_ID)) {
              Serial.print(F("[OTA] Type Match. Checking version... "));
              if (serverVersion != currentFirmwareVersion && downloadUrl.length() > 0) {
                 Serial.printf("New version %s found. Updating...\n", serverVersion.c_str());
                 update("Downloading version " + serverVersion);
                 executeOTA(downloadUrl);
              } else {
                 Serial.println(F("Up to date."));
                 update("Already up to date.");
              }
            } else {
              Serial.printf("[OTA] REJECTED. Server sent Type %s, I am Type %s\n", serverTypeId.c_str(), MY_MACHINE_TYPE_ID);
              update("Update rejected: Wrong Device Type");
            }
        } else {
            Serial.println(F("[OTA] Success, but no firmware object."));
        }

      } else {
        Serial.println(F("[OTA] JSON Error or invalid status"));
      }
    } else {
      Serial.printf("[OTA] HTTP Error: %d\n", httpCode);
    }
    http.end();
  }
  ota_request = false;
}
void initPrefer(){
  prefer.begin("state",false);
  AUDIO_HTTP_ERROR =  prefer.getUInt("AUDIO_HTTP_ERROR",AUDIO_HTTP_ERROR);//                 // HTTP error
  AUDIO_HTTP_ERROR_SUBMIT =  prefer.getUInt("AUDIO_HTTP_ERROR_SUBMIT",AUDIO_HTTP_ERROR_SUBMIT);             // Submit HTTP error
  AUDIO_INVALID_JSON =  prefer.getUInt("AUDIO_INVALID_JSON",AUDIO_INVALID_JSON);                   // Invalid JSON response
  AUDIO_LOW_BALANCE =  prefer.getUInt("AUDIO_LOW_BALANCE",AUDIO_LOW_BALANCE);                   // Member has low balance
  AUDIO_MACHINE_NOT_RESPONSE =  prefer.getUInt("AUDIO_MACHINE_NOT_RESPONSE",AUDIO_MACHINE_NOT_RESPONSE);           // Machine didn't respond
  AUDIO_MACHINE_NOTFOUND_IN_SERVER =  prefer.getUInt("AUDIO_MACHINE_NOTFOUND_IN_SERVER",AUDIO_MACHINE_NOTFOUND_IN_SERVER);     // Machine not found on server
  AUDIO_MACHINE_STOCK_NOT_FOUND =  prefer.getUInt("AUDIO_MACHINE_STOCK_NOT_FOUND",AUDIO_MACHINE_STOCK_NOT_FOUND);       // Coffee machine stock not found
  AUDIO_MASTER_CARD_LIMIT =  prefer.getUInt("AUDIO_MASTER_CARD_LIMIT",AUDIO_MASTER_CARD_LIMIT);             // Master card limit reached
  AUDIO_MQTT_CONNECT =  prefer.getUInt("AUDIO_MQTT_CONNECT",AUDIO_MQTT_CONNECT);                 // MQTT connected
  AUDIO_MQTT_FAILED =  prefer.getUInt("AUDIO_MQTT_FAILED",AUDIO_MQTT_FAILED);                   // MQTT connection failed
  AUDIO_NO_ACCESS =  prefer.getUInt("AUDIO_NO_ACCESS",AUDIO_NO_ACCESS);                  // No access
  AUDIO_NO_PERMISSION_WITHOUT_MEMBER =  prefer.getUInt("AUDIO_NO_PERMISSION_WITHOUT_MEMBER",AUDIO_NO_PERMISSION_WITHOUT_MEMBER);// No permission without member
  AUDIO_PLS_REMOVE_CARD =  prefer.getUInt("AUDIO_PLS_REMOVE_CARD",AUDIO_PLS_REMOVE_CARD);              // Please remove card
  AUDIO_PREPARING_COFFEE =  prefer.getUInt("AUDIO_PREPARING_COFFEE",AUDIO_PREPARING_COFFEE);             // Preparing coffee
  AUDIO_PREPARING_TEA =  prefer.getUInt("AUDIO_PREPARING_TEA",AUDIO_PREPARING_TEA);                // Preparing tea
  AUDIO_SELECT_TIME_OUT =  prefer.getUInt("AUDIO_SELECT_TIME_OUT",AUDIO_SELECT_TIME_OUT);              // Selection timeout
  AUDIO_SPECIAL_CARD_LIMIT =  prefer.getUInt("AUDIO_SPECIAL_CARD_LIMIT",AUDIO_SPECIAL_CARD_LIMIT);           // Special card limit reached
  AUDIO_SPECIAL_CARD_NO_PERMISSION =  prefer.getUInt("AUDIO_SPECIAL_CARD_NO_PERMISSION",AUDIO_SPECIAL_CARD_NO_PERMISSION);   // Special card no permission
  AUDIO_WELCOME_TO_SH =  prefer.getUInt("AUDIO_WELCOME_TO_SH",AUDIO_WELCOME_TO_SH);                 // Welcome to office/site
  AUDIO_USER_NOT_FOUND =  prefer.getUInt("AUDIO_USER_NOT_FOUND",AUDIO_USER_NOT_FOUND);               // USER NOT FOUND


  AUDIO_SUBMIT_DONE =  prefer.getUInt("AUDIO_SUBMIT_DONE",AUDIO_SUBMIT_DONE);                  // Purchase submitted successfully
  AUDIO_TEA_READY =  prefer.getUInt("AUDIO_TEA_READY",AUDIO_TEA_READY);                    // Tea is ready
  AUDIO_UNUSABLE =  prefer.getUInt("AUDIO_UNUSABLE",AUDIO_UNUSABLE);                   // Machine unusable
  AUDIO_SUBMIT_MEMBER_NOT_FOUND =  prefer.getUInt("AUDIO_SUBMIT_MEMBER_NOT_FOUND",AUDIO_SUBMIT_MEMBER_NOT_FOUND);      // Member not found
  AUDIO_WALLET_NOT_FOUND =  prefer.getUInt("AUDIO_WALLET_NOT_FOUND",AUDIO_WALLET_NOT_FOUND);             // Wallet not found
  AUDIO_WAITING_FOR_SELECTION =  prefer.getUInt("AUDIO_WAITING_FOR_SELECTION",AUDIO_WAITING_FOR_SELECTION);        // Waiting for selection || select item coffee or tea
  AUDIO_WIFI_NOT_CONNECT =  prefer.getUInt("AUDIO_WIFI_NOT_CONNECT",AUDIO_WIFI_NOT_CONNECT);              // WiFi not connected
  AUDIO_AI_CMD =  prefer.getUInt("AUDIO_AI_CMD",AUDIO_AI_CMD);                       // AI command received
  AUDIO_AUTO_CANCEL =  prefer.getUInt("AUDIO_AUTO_CANCEL",AUDIO_AUTO_CANCEL);                   // Auto cancel
  AUDIO_CHECKING =  prefer.getUInt("AUDIO_CHECKING",AUDIO_CHECKING);                  // Checking card
  AUDIO_COFFEE_READY =  prefer.getUInt("AUDIO_COFFEE_READY",AUDIO_COFFEE_READY);                 // Coffee is ready
  AUDIO_CUP_NOT_FOUND_IN_SERVER =  prefer.getUInt("AUDIO_CUP_NOT_FOUND_IN_SERVER",AUDIO_CUP_NOT_FOUND_IN_SERVER);      // Cup not found in server
  AUDIO_CUP_OUT_OF_STOCK =  prefer.getUInt("AUDIO_CUP_OUT_OF_STOCK",AUDIO_CUP_OUT_OF_STOCK);             // Cup out of stock
  AUDIO_EMPLOYEE_LIMIT =  prefer.getUInt("AUDIO_EMPLOYEE_LIMIT",AUDIO_EMPLOYEE_LIMIT);               // Employee limit reached
  AUDIO_EMPLOYEE_NO_PERMISSION =  prefer.getUInt("AUDIO_EMPLOYEE_NO_PERMISSION",AUDIO_EMPLOYEE_NO_PERMISSION);        // Employee no permission
  AUDIO_THANK_YOU_FOR_ST_W_SH =  prefer.getUInt("AUDIO_THANK_YOU_FOR_ST_W_SH",AUDIO_THANK_YOU_FOR_ST_W_SH);
  player.volume(prefer.getUInt("volume",28));
  prefer.end();
}
void setup(){

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);

  pinMode(OUTPUT_COFFEE, OUTPUT);
  pinMode(OUTPUT_TEA, OUTPUT);
  digitalWrite(OUTPUT_COFFEE, LOW);
  digitalWrite(OUTPUT_TEA, LOW);

  pinMode(INPUT_BUZZER, INPUT);
  pinMode(INPUT_USABLE, INPUT_PULLDOWN);

  pinMode(INPUT_COFFEE, INPUT_PULLUP);
  pinMode(INPUT_TEA, INPUT_PULLUP);
  delay(50);
  
  pinMode(ST_LED,OUTPUT);
  Serial.begin(115200);
  delay(50);
  rdm6300.begin(RDM6300_RX_PIN);
  delay(100);
  // Pass 'true' to allow formatting on failure
  if (!LittleFS.begin(true)) {
    Serial.println(" Mount Failed.");
    Serial.println("Formatting failed or Partition Scheme incorrect.");
    FS_Faulty_update_requied = true;
  } else {
    Serial.println(" Success!");
    FS_Faulty_update_requied = false;
    
    // Optional: Print file listing to debug
    Serial.println("Checking files:");
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    while(file){
        Serial.print("  FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
    file.close();
  }
  delay(10);
  // Setup MQTT server and callback
  client.setBufferSize(10240);   // 4 KB buffer
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
  mqttQueue = xQueueCreate(20, sizeof(mqtt_msg_t));
  if (!mqttQueue) {
    Serial.println("MQTT Queue create failed!");
    while (1);
  }
  delay(100);

  Serial2.begin(9600, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);
  if (player.begin(Serial2)) {
      Serial.println("✅ DFPlayer Mini ready");
      player.volume(25);
  } else {
      Serial.println("❌ DFPlayer Mini failed");
  }

  delay(10);
  // Reading Data from internal system
  //=======================================================================================================
  initPrefer();
  //=============================================================================================================
  // Watchdog (clean re-init; only core0/core1 are watched)
  esp_task_wdt_deinit();
  esp_task_wdt_config_t twdt_config = {
    .timeout_ms     = 60000,
    .idle_core_mask = 0,
    .trigger_panic  = true
  };
  esp_task_wdt_init(&twdt_config);

  // WiFiManager non-blocking portal
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  wm.setDebugOutput(true);
  wm.setConnectTimeout(5);
  wm.setConfigPortalBlocking(false);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  unsigned long startTime = millis();
  if (wm.autoConnect(ssid.c_str())) {
    Serial.println(F("Wifi autoConnect!"));
  } else {
    Serial.println(F("Configportal running"));
  }

  // Create tasks pinned to cores
  BaseType_t t0 = xTaskCreatePinnedToCore(core0Task, "Core0Task", 8192,  NULL, 1, &core0Handle, 0);
  BaseType_t t1 = xTaskCreatePinnedToCore(core1Task, "Core1Task", 10000, NULL, 1, &core1Handle, 1);

  if (t0 == pdPASS && t1 == pdPASS) {
    esp_task_wdt_add(core0Handle);
    esp_task_wdt_add(core1Handle);
    Serial.println("[TASK] Both tasks started.");
  } else {
    Serial.println("[TASK] Task creation failed! Restarting...");
    delay(500);
    ESP.restart();
  }

}
void loop(){

}
void core0Task(void *parameter) {
  Serial.printf("[CORE 0] Running on core: %d\n", xPortGetCoreID());
  Serial.print("MY_MACHINE_ID: ");
  Serial.println(MY_MACHINE_ID);
  mqtt_msg_t msg;
  for(;;){
    wdt_kick_safe();
    reconnectWiFi();
    // updateWifiLed();
    // --- ✅ OTA LOGIC ---
    if (ota_request && led_ST) { 
      Serial.println(F("[OTA] Request received. Connecting to API..."));
      checkAndPerformOTA();
    }

    if (!client.connected() && led_ST) {
      connectMQTT();
    }else{
      // Limit publish per cycle
      int sent = 0;
      while (xQueueReceive(mqttQueue, &msg, 0) == pdTRUE && sent < 5) {
        Serial.print("[MQTT] Topic   : ");
        Serial.println(msg.topic);

        Serial.print("[MQTT] Payload : ");
        Serial.println(msg.payload);

        Serial.print("[MQTT] Retain  : ");
        Serial.println(msg.retained);

        client.publish(msg.topic, msg.payload, msg.retained);
        Serial.println("[MQTT] Event published\n");
        sent++;
      }
    }
    client.loop();
  }
}
void core1Task(void *parameter) {
  Serial.printf("[CORE 1] Running on core: %d\n", xPortGetCoreID());
  for (;;) {
    wdt_kick_safe();
    if (rdm6300.get_new_tag_id()) {
      player.play(AUDIO_WELCOME_TO_SH);
      if(digitalRead(INPUT_USABLE)){
        player.play(AUDIO_UNUSABLE);
      }else if(led_ST){
        String cardString = String(rdm6300.get_tag_id(), DEC);
        while (cardString.length() < 10) cardString = "0" + cardString;
        Serial.print("[RFID] Read: "); Serial.println(cardString);
        cardCK(url_1,cardString);
      }else{
        player.play(AUDIO_WIFI_NOT_CONNECT);
        Serial.println("WiFi Not Connected");
      }
    }
  }
}