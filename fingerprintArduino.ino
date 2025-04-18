#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Fingerprint sensor setup
SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Buttons
const int buttonA = 4;
const int buttonB = 5;
const int buttonC = 6;

// LEDs for candidates
const int ledA = A0;
const int ledB = A1;
const int ledC = A2;

// Buzzer
const int buzzer = 8;

// Voting state
bool fingerprintAuthenticated = false;
unsigned long lastVoteTime = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(57600);
  finger.begin(57600);

  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);

  pinMode(ledA, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledC, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(ledA, LOW);
  digitalWrite(ledB, LOW);
  digitalWrite(ledC, LOW);
  digitalWrite(buzzer, LOW);

  if (finger.verifyPassword()) {
    Serial.println("✅ Fingerprint sensor detected!");
  } else {
    Serial.println("❌ Fingerprint sensor not found!");
    while (1); // halt
  }

  finger.getTemplateCount();
  Serial.print("📦 Stored fingerprints: ");
  Serial.println(finger.templateCount);

  storeFingerprint(); // Store one fingerprint at start
}

void loop() {
  if (!fingerprintAuthenticated) {
    int id = getFingerprintID();
    if (id >= 0) {
      fingerprintAuthenticated = true;
      Serial.println("✅ Finger authenticated! Please vote.");
    }
  } else {
    checkVotingButtons();
  }
}

// -------------------- Voting Logic --------------------

void checkVotingButtons() {
  if (millis() - lastVoteTime < 1000) return; // debounce

  if (digitalRead(buttonA) == LOW) {
    castVote(ledA, "A");
  } else if (digitalRead(buttonB) == LOW) {
    castVote(ledB, "B");
  } else if (digitalRead(buttonC) == LOW) {
    castVote(ledC, "C");
  }
}

void castVote(int ledPin, String candidate) {
  Serial.println("✅ Vote cast for Candidate " + candidate);
  digitalWrite(ledPin, HIGH);
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzer, LOW);

  fingerprintAuthenticated = false; // require re-authentication
  lastVoteTime = millis();
  Serial.println("🔒 Vote complete. Authenticate again for next vote.");
}

// -------------------- Fingerprint Logic --------------------

int getFingerprintID() {
  Serial.println("👉 Place finger for authentication...");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ No match found.");
    return -1;
  }

  Serial.print("🎉 Match found! ID: ");
  Serial.println(finger.fingerID);
  return finger.fingerID;
}

void storeFingerprint() {
  int id = 1; // You can increment this if storing more
  int p = -1;

  Serial.println("📌 Fingerprint Enrollment");

  // Step 1: Get first image
  while (p != FINGERPRINT_OK) {
    Serial.println("👉 Place finger (1st time)...");
    p = finger.getImage();
    delay(500);
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ Failed to convert 1st image");
    return;
  }

  Serial.println("✋ Remove finger...");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);

  // Step 2: Get second image
  p = -1;
  while (p != FINGERPRINT_OK) {
    Serial.println("👉 Place same finger (2nd time)...");
    p = finger.getImage();
    delay(500);
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ Failed to convert 2nd image");
    return;
  }

  // Create model and store
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ Failed to create fingerprint model");
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("✅ Fingerprint stored successfully!");
  } else {
    Serial.println("❌ Failed to store fingerprint.");
  }
}