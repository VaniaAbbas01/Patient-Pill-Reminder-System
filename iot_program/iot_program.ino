
/*
PILL REMINDER IOT SYSTEM:
Sensors used: PULSE SENSOR, TEMPERATURE SENSOR
Aactuators used: BUZZER
*/
#include <TimeLib.h>  
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_MLX90614.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>


// Provide the token generation process info.
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Wi-Fi Credentials
const char* ssid = "StormFiber";
const char* password = "wland5d31f";

// Firebase Credentials
#define API_KEY "AIzaSyDiwSosj-Kb5r6h8GeN4Chl0HdzO6wN4oA"
#define DATABASE_URL "https://reminder-application-4c715-default-rtdb.firebaseio.com/"
#define FIREBASE_PROJECT_ID "reminder-application-4c715"

// User Credentials
#define USER_EMAIL "vania2@gmail.com"
#define USER_PASSWORD "Vania123"

// Firebase objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Set port of web server
ESP8266WebServer server(80);

// Hardware components
const int pulseSensorPin = A0;
const int buzzerPin = D5;
const int RX_PIN = D6;
const int TX_PIN = D7;

SoftwareSerial mySerial(RX_PIN, TX_PIN);
DFRobotDFPlayerMini myDFPlayer;

// sensor data variables
float pulse;
float temperature;

unsigned long sendDataPrevMillis = 0;
String uid;

// NTP Time (for ESP8266/ESP32)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // UTC time

// WiFi initialization function
void initWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected! IP Address: " + WiFi.localIP().toString());
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  initWiFi();

  // checking if mlx connection is initiated
  if(!mlx.begin()) {
    Serial.println("Error Connecting to MLX90614 sensor. Check wiring!");
    while(1);
  }

  // Initializing NTP
  timeClient.begin();
  // Set system time from NTP

  // Assigning Firebase API key and database URL
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Authorising Firebase with email and password
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assigning token status callback function
  config.token_status_callback = tokenStatusCallback;

  // Begining Firebase authentication
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Waiting for UID to be available
  Serial.println("Getting User UID...");
  unsigned long startMillis = millis();
  while (auth.token.uid == "" && millis() - startMillis < 10000) { // Timeout after 10 seconds
    Serial.print(".");
    delay(1000);
  }

  if (auth.token.uid != "") {
    uid = auth.token.uid.c_str();
    Serial.println("\nUser UID: " + uid);
  } else {
    Serial.println("\nFailed to get UID!");
  }

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected!");
    while (true);
  }

  myDFPlayer.volume(20);

  pinMode(pulseSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // get the index page on root route
  server.on("/", get_index); 
  server.begin();
  Serial.println("Server listening...");
}

void loop() {

  timeClient.update();
  setTime(timeClient.getEpochTime());

  server.handleClient();
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 180000)) {
    sendDataPrevMillis = millis();

    pulse = readingPulse();
    temperature = readingTemperature();
    sendingTempPulse(pulse, temperature); 

    fetchReminders();
  }
  delay(5000);
}

// Function to get the current date and time as a formatted string
String getCurrentDateTime() {
  return String(year()) + "-" + String(month()) + "-" + String(day()) + " " +
         String(hour()) + ":" + String(minute()) + ":" + String(second());
}

// Function to read pulse from the sensor
float readingPulse() {
  int pulse = analogRead(pulseSensorPin);
  return normalisePulse(pulse);
}

// Function to read temperature from the sensor
float readingTemperature() {
  return temperature = mlx.readObjectTempC();
}

// Function to normalize to BPM range
float normalisePulse(int rawPulse) {
  return map(rawPulse, 0, 1023, 50, 150); 
}

// Function to send pulse to firebase's Realtime Database
void sendingTempPulse(float pul, float temp) {
  String path = "/sensorData/" + uid + "/";
  String dateTime = getCurrentDateTime();

  // Sending pulse value
  if (Firebase.RTDB.setInt(&firebaseData, path + "pulse", pul)) {
    Serial.printf("Writing value: %.2f at path: %s\nPASSED\n", pul, path.c_str());
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
  }

  // Sending temperature value
  if (Firebase.RTDB.setInt(&firebaseData, path + "temperature", temp)) {
    Serial.printf("Writing value: %.2f at path: %s\nPASSED\n", temp, path.c_str());
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
  }

  // Sending date time
  if (Firebase.RTDB.setString(&firebaseData, path + "time", dateTime)) {
    Serial.printf("Writing date/time at path time\n");
  } else {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
  }
}

// Fetching reminders from Firebase's Firestore 
void fetchReminders() {
  
  // fetching reminders for the user with the id - uid
  if (Firebase.Firestore.getDocument(&firebaseData, FIREBASE_PROJECT_ID, "", "reminders/" + uid, "")) {
    Serial.println("Reminders fetched successfully:");
    Serial.println(firebaseData.payload().c_str());

    FirebaseJson json;
    json.setJsonData(firebaseData.payload().c_str());
    
    // extracting reminder time from the Reminder data received
    FirebaseJsonData reminderData;
    if (json.get(reminderData, "fields/time/stringValue")) {
      String reminderTimeStr = reminderData.stringValue.c_str();
      Serial.printf("Next reminder at: %s\n", reminderTimeStr.c_str());

      // Extract hours, minutes, and seconds from reminder time string
      struct tm reminderTm = {};
      sscanf(reminderTimeStr.c_str(), "%*d-%*d-%*dT%2d:%2d:%2d", 
             &reminderTm.tm_hour, &reminderTm.tm_min, &reminderTm.tm_sec);

      // Get current time
      time_t now = timeClient.getEpochTime();
      struct tm currentTm;
      localtime_r(&now, &currentTm);

      Serial.printf("Current Time: %02d:%02d:%02d\n", currentTm.tm_hour, currentTm.tm_min, currentTm.tm_sec);
      Serial.printf("Reminder Time: %02d:%02d:%02d\n", reminderTm.tm_hour, reminderTm.tm_min, reminderTm.tm_sec);

      // Compare HH:MM:SS only
      if (reminderTm.tm_hour == currentTm.tm_hour &&
          reminderTm.tm_min == currentTm.tm_min) {
        Serial.println("Reminder time reached! Triggering alarm...");
        triggerAlarm();
      } else {
        Serial.println("Reminder time is in the future. No action needed.");
      } 
    } else {
      Serial.println("Failed to extract reminder time.");
    }
  } else {
    Serial.println("Failed to fetch reminders: " + firebaseData.errorReason());
  }
}


// Function to convert ISO 8601 time to UNIX timestamp
time_t convertISOtoUnix(String isoTime) {
    struct tm timeStruct;
    memset(&timeStruct, 0, sizeof(struct tm));

    int hour, minute, second;
    
    // Extract only HH:MM:SS from the string
    sscanf(isoTime.c_str(), "%*d-%*d-%*dT%2d:%2d:%2d", &hour, &minute, &second);

    // Set time values (ignore date)
    timeStruct.tm_hour = hour;
    timeStruct.tm_min = minute;
    timeStruct.tm_sec = second;

    return mktime(&timeStruct);  // Return UNIX timestamp (time of day only)
}


// Function to trigger the buzzer alarm
void triggerAlarm() {
  Serial.println("⚠️ Reminder Time Reached! Triggering Alarm...");

  // Play audio from SD card (001.mp3)
  myDFPlayer.play(1);
  
  // buzzer beeps 5 times
  for (int i = 0; i < 5; i++) { 
    digitalWrite(buzzerPin, HIGH);
    delay(500);
    digitalWrite(buzzerPin, LOW);
    delay(500);
  }

  Serial.println("⏹️ Alarm Stopped.");
}

void get_index() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta http=equiv=\'refresh\' content=\"Z\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
  html += "<body><div style={display:flex, flexDirection:\"column\", alignItems:\"center\", justifyContent:\"center\"}><h1>PILL REMINDER DASHBOARD</h1>";
  html += "<p>Welcome to the Pill Reminder Dashboard</p>";
  html += "<div><p> <strong> Patient Pulse is: ";
  html += readingPulse();
  html += "</strong></p></div><div>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
