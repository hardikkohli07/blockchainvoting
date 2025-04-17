#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// === TFT Pins ===
#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2

// Create TFT object
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

byte rowPins[ROWS] = {13, 12, 14, 27}; // Adjust to your actual pins
byte colPins[COLS] = {26, 25, 33, 32};    // Adjust to your actual pins

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String voterID = "";

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Init TFT
  tft.initR(INITR_BLACKTAB);  // ST7735-specific
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  // Welcome Message
  tft.setCursor(10, 30);
  tft.println("Welcome to");
  tft.setCursor(10, 55);
  tft.println("eVoting");
  Serial.println("Welcome to eVoting");
  delay(2000);

  // Clear and prompt
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println("Enter Voter ID:");
  Serial.println("Enter Voter ID:");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);

    if (key == '#') {
      // Display Final Voter ID
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Voter ID:");
      tft.setCursor(0, 30);
      tft.println(voterID);
      Serial.println("Voter ID Entered: " + voterID);

      delay(3000); // Show result for 3 sec

      // Reset
      voterID = "";
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Enter Voter ID:");
      Serial.println("Enter Voter ID:");
    }
    else if (key == '*') {
      // Clear current input
      voterID = "";
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.println("Enter Voter ID:");
      Serial.println("Voter ID cleared");
    }
    else {
      // Append digit
      voterID += key;
      tft.print(key);
      Serial.print("Current Input: ");
      Serial.println(voterID);
    }
  }
}
