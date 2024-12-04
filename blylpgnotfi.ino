#define BLYNK_TEMPLATE_ID "TMPL387JsFkj0";
#define BLYNK_TEMPLATE_NAME "gas alert";
#define BLYNK_AUTH_TOKEN "r1imFKNmiblVhuTMuVNEkis6u0z7qZfq";

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>

// Pin connections
const int MQ_SENSOR_PIN = A0;
const int BUZZER_PIN = D8;
const int SERVO_PIN = D3;
const int RELAY_PIN = D4;
const int RED_LED_PIN = D6;
const int GREEN_LED_PIN = D5;

// I2C LCD display setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo motor setup
Servo servoMotor;

char auth[] = "r1imFKNmiblVhuTMuVNEkis6u0z7qZfq";
char ssid[] = "Your_WiFi_SSID";
char pass[] = "Your_WiFi_Password";

bool alertSent = false; // To avoid multiple notifications

void setup() {
  Serial.begin(9600);
  pinMode(MQ_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  servoMotor.attach(SERVO_PIN);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  int sensorValue = analogRead(MQ_SENSOR_PIN);
  Serial.println(sensorValue); // Print sensor value for debugging

  // Send sensor value to Blynk app via virtual pin V0
  Blynk.virtualWrite(V0, sensorValue);

  lcd.clear(); // Clear the LCD before updating

  if (sensorValue > 630) { // Adjusted threshold for gas detection
    Serial.println("Gas Detected!");
    Serial.println("Regulator OFF");

    lcd.setCursor(0, 0);
    lcd.print("Gas Detected");
    lcd.setCursor(0, 1);
    lcd.print("Regulator OFF");

   // Turn on red LED
    digitalWrite(RED_LED_PIN, HIGH);

    // Activate buzzer
    tone(BUZZER_PIN, 1000);

   // Turn off green LED
    digitalWrite(GREEN_LED_PIN, LOW);

    // Move servo motor to 180 degrees
    servoMotor.write(180);

    // Turn on relay
    digitalWrite(RELAY_PIN, HIGH);

    // Send Blynk alert
    if (!alertSent) {
      Blynk.logEvent("gas_alert", "Gas Detected: Regulator OFF");
      alertSent = true; // Avoid duplicate alerts
      Serial.println("Blynk alert sent.");
    }
  } else {
    Serial.println("No Gas Detected");
    Serial.println("Regulator ON");

    lcd.setCursor(0, 0);
    lcd.print("No Gas Detected");
    lcd.setCursor(0, 1);
    lcd.print("Regulator ON");

    // Turn off buzzer
    noTone(BUZZER_PIN);

    // Turn on green LED
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);

    // Move servo motor to 0 degrees
    servoMotor.write(0);

    // Turn off relay
    digitalWrite(RELAY_PIN, LOW);

    // Reset alertSent for the next gas detection
    alertSent = false;
  }

  Blynk.run(); // Run Blynk in the loop
  delay(900); // Reduced delay for faster updates
}
