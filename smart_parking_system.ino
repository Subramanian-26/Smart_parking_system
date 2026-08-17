#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;

// ---------------- PIN DEFINITIONS ----------------

// Ultrasonic Sensor 1
#define TRIG1 19
#define ECHO1 18

// Ultrasonic Sensor 2
#define TRIG2 2
#define ECHO2 15

// Ultrasonic Sensor 3
#define TRIG3 5
#define ECHO3 17

// Slot 1 LEDs
#define GREEN1 14
#define RED1 27

// Slot 2 LEDs
#define GREEN2 33
#define RED2 32

// Slot 3 LEDs
#define GREEN3 26
#define RED3 25

// Servo
#define SERVO_PIN 13

// Distance below which slot is considered occupied
#define PARKING_DISTANCE 20


void setup() {

  Serial.begin(115200);

  // ---------------- ULTRASONIC SENSORS ----------------

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);


  // ---------------- LEDs ----------------

  pinMode(GREEN1, OUTPUT);
  pinMode(RED1, OUTPUT);

  pinMode(GREEN2, OUTPUT);
  pinMode(RED2, OUTPUT);

  pinMode(GREEN3, OUTPUT);
  pinMode(RED3, OUTPUT);


  // ---------------- LCD ----------------

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();


  // ---------------- SERVO ----------------

  gateServo.attach(SERVO_PIN);
  gateServo.write(0);


  // ---------------- STARTUP MESSAGE ----------------

  lcd.setCursor(0, 0);
  lcd.print("SMART PARKING");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM READY");

  delay(2000);

  lcd.clear();
}


// =====================================================
// FUNCTION TO MEASURE DISTANCE
// =====================================================

float getDistance(int trigPin, int echoPin) {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  // Timeout after 30 ms
  long duration = pulseIn(echoPin, HIGH, 30000);

  // If no echo is received
  if (duration == 0) {
    return 999;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // ---------------- READ DISTANCES ----------------

  float distance1 = getDistance(TRIG1, ECHO1);
  delay(50);

  float distance2 = getDistance(TRIG2, ECHO2);
  delay(50);

  float distance3 = getDistance(TRIG3, ECHO3);


  // ---------------- CHECK SLOT STATUS ----------------

  bool slot1Occupied = distance1 < PARKING_DISTANCE;
  bool slot2Occupied = distance2 < PARKING_DISTANCE;
  bool slot3Occupied = distance3 < PARKING_DISTANCE;


  // =====================================================
  // SLOT 1 LEDs
  // =====================================================

  if (slot1Occupied) {

    digitalWrite(RED1, HIGH);
    digitalWrite(GREEN1, LOW);

  } else {

    digitalWrite(RED1, LOW);
    digitalWrite(GREEN1, HIGH);
  }


  // =====================================================
  // SLOT 2 LEDs
  // =====================================================

  if (slot2Occupied) {

    digitalWrite(RED2, HIGH);
    digitalWrite(GREEN2, LOW);

  } else {

    digitalWrite(RED2, LOW);
    digitalWrite(GREEN2, HIGH);
  }


  // =====================================================
  // SLOT 3 LEDs
  // =====================================================

  if (slot3Occupied) {

    digitalWrite(RED3, HIGH);
    digitalWrite(GREEN3, LOW);

  } else {

    digitalWrite(RED3, LOW);
    digitalWrite(GREEN3, HIGH);
  }


  // =====================================================
  // COUNT AVAILABLE SLOTS
  // =====================================================

  int availableSlots = 0;

  if (!slot1Occupied)
    availableSlots++;

  if (!slot2Occupied)
    availableSlots++;

  if (!slot3Occupied)
    availableSlots++;


  // =====================================================
  // SERVO / GATE CONTROL
  // =====================================================

  if (availableSlots > 0) {

    // At least one parking slot is free
    gateServo.write(90);

  } else {

    // All parking slots are occupied
    gateServo.write(0);
  }


  // =====================================================
  // LCD SCREEN 1 - SLOT STATUS
  // =====================================================

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("S1:");

  if (slot1Occupied)
    lcd.print("OCC ");
  else
    lcd.print("FREE");

  lcd.print(" S2:");

  if (slot2Occupied)
    lcd.print("OCC");
  else
    lcd.print("FREE");


  lcd.setCursor(0, 1);

  lcd.print("S3:");

  if (slot3Occupied)
    lcd.print("OCC");
  else
    lcd.print("FREE");

  delay(2000);


  // =====================================================
  // LCD SCREEN 2 - AVAILABLE SLOTS
  // =====================================================

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("Available: ");
  lcd.print(availableSlots);
  lcd.print("/3");


  lcd.setCursor(0, 1);

  if (availableSlots > 0) {

    lcd.print("Gate: OPEN");

  } else {

    lcd.print("Gate: CLOSED");
  }

  delay(2000);


  // =====================================================
  // SERIAL MONITOR
  // =====================================================

  Serial.print("Slot 1: ");
  Serial.print(slot1Occupied ? "OCCUPIED" : "FREE");

  Serial.print(" | Slot 2: ");
  Serial.print(slot2Occupied ? "OCCUPIED" : "FREE");

  Serial.print(" | Slot 3: ");
  Serial.print(slot3Occupied ? "OCCUPIED" : "FREE");

  Serial.print(" | Available: ");
  Serial.println(availableSlots);
}