#include <WiFi.h>
#include <HTTPClient.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// === WiFi Configuration ===
const char* ssid = "Dev";
const char* password = "123456789";
const char* serverName = "http://192.168.125.214:8000/submit_voter_id";

// === TFT Pins ===
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2

// === TFT Object ===
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// === Keypad Configuration ===
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// === Global Variables ===
String voterID = "";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // === WiFi Connection ===
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // === Initialize TFT ===
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  // Welcome Screen
  tft.setCursor(10, 30);
  tft.println("Welcome to");
  tft.setCursor(10, 55);
  tft.println("eVoting");
  delay(2000);

  // Prompt for voter ID
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println("Enter Voter ID:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '#') {
      if (voterID.length() == 12) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.println("Voter ID:");
        tft.setCursor(0, 30);
        tft.println(voterID);
        Serial.println("Voter ID Entered: " + voterID);

        // Send to server
        sendVoterID(voterID);
      } else {
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.println("Invalid ID");
        tft.setCursor(0, 30);
        tft.println("Must be 12 digits");
        Serial.println("Invalid ID length. Must be 12 digits.");
      }

      delay(3000); // Pause to show result

      // Reset input
      voterID = "";
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Enter Voter ID:");
    }
    else if (key == '*') {
      // Clear input
      voterID = "";
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Enter Voter ID:");
      Serial.println("Voter ID cleared");
    }
    else if (isdigit(key)) {
      // Append digit
      if (voterID.length() < 12) {
        voterID += key;
        tft.print(key);
        Serial.print("Current Input: ");
        Serial.println(voterID);
      }
    }
  }
}

void sendVoterID(String id) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    
    // Method 2: Increase HTTP timeout to 30 seconds
    http.setTimeout(30000);
    
    String json = "{\"voter_id\": \"" + id + "\"}";
    
    // Method 3: Add better user feedback
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("Sending...");
    
    int httpResponseCode = http.POST(json);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
      
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      
      if (response.indexOf("accepted") > 0) {
        tft.println("Vote Recorded");
        tft.setCursor(0, 30);
        tft.println("Successfully!");
      } else if (response.indexOf("rejected") > 0) {
        tft.println("Already Voted");
      } else {
        tft.println("Server Error");
        tft.setCursor(0, 30);
        tft.println("Try Again");
      }
    } else {
      Serial.println("Error sending ID: " + String(httpResponseCode));
      
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Server Error:");
      tft.setCursor(0, 30);
      
      // Display specific error message based on error code
      switch(httpResponseCode) {
        case -1: tft.println("Connection Failed"); break;
        case -11: tft.println("Timeout Error"); break;
        default: tft.println("Code: " + String(httpResponseCode));
      }
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.println("WiFi Error");
    tft.setCursor(0, 30);
    tft.println("Reconnecting...");
    
    WiFi.reconnect();
  }
}