/***************************************************
 Solar Smart Irrigation System
 ESP8266 + Blynk + Auto/Manual Mode
***************************************************/

#define BLYNK_TEMPLATE_ID "TMPL3p5ueh-t8"
#define BLYNK_TEMPLATE_NAME "Solar Irrigation"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WiFi Credentials
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

// Relay Pin
#define relayPin D1

// Variables
int threshold = 500;
int mode = 0; // 0 = AUTO, 1 = MANUAL
int manualPump = 0;

BlynkTimer timer;

// Function: Read Moisture + Auto Control
void sendMoisture() {

  int value = analogRead(A0);

  Serial.println("------ SYSTEM STATUS ------");

  Serial.print("Moisture Value: ");
  Serial.println(value);

  Serial.print("Mode: ");
  Serial.println(mode == 0 ? "AUTO" : "MANUAL");

  // Send Moisture Value to Blynk
  Blynk.virtualWrite(V0, value);

  // AUTO MODE
  if (mode == 0) {

    if (value > threshold) {

      // Dry Soil → Pump ON
      digitalWrite(relayPin, LOW);
      Blynk.virtualWrite(V2, 1);

      Serial.println("Pump Status: ON (AUTO - Dry Soil)");

    } else {

      // Wet Soil → Pump OFF
      digitalWrite(relayPin, HIGH);
      Blynk.virtualWrite(V2, 0);

      Serial.println("Pump Status: OFF (AUTO - Wet Soil)");
    }
  }

  Serial.println("---------------------------\n");
}

// Manual Pump Control (V5)
BLYNK_WRITE(V5) {

  manualPump = param.asInt();

  if (mode == 1) {

    digitalWrite(relayPin, manualPump ? LOW : HIGH);

    Blynk.virtualWrite(V2, manualPump);

    Serial.print("Manual Pump: ");

    if (manualPump == 1) {
      Serial.println("ON");
    } else {
      Serial.println("OFF");
    }
  }
}

// Mode Switch (V7)
BLYNK_WRITE(V7) {

  mode = param.asInt();

  Serial.print("Mode Changed To: ");

  if (mode == 0) {
    Serial.println("AUTO");
  } else {
    Serial.println("MANUAL");
  }
}

void setup() {

  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);

  // Pump OFF initially
  digitalWrite(relayPin, HIGH);

  Serial.println("System Starting...");

  // Connect to WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Read moisture every 1 second
  timer.setInterval(1000L, sendMoisture);
}

void loop() {

  Blynk.run();
  timer.run();
}