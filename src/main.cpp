#include "thingProperties.h"
#include <WiFi.h>

#define CURRENT_PIN 34
#define BATTERY_PIN 35
#define GRID_PIN 18

#define POWER_LED 2
#define GRID_LED 4
#define BATTERY_LED 5

unsigned long lastDebug = 0;

void setup()
{
  Serial.begin(115200);
  delay(1500);

  pinMode(POWER_LED, OUTPUT);
  pinMode(GRID_LED, OUTPUT);
  pinMode(BATTERY_LED, OUTPUT);

  pinMode(GRID_PIN, INPUT_PULLDOWN);

  Serial.println("\n--- SYSTEM START ---");

  // WiFi debug BEFORE cloud starts
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);

  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    wifiAttempts++;

    if (wifiAttempts > 20)
    {
      Serial.println("\nWiFi failed to connect (timeout)");
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("WiFi not connected. Cloud may fail.");
  }

  // IoT Cloud init
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(4); // 🔥 increased debug level
  ArduinoCloud.printDebugInfo();

  Serial.println("Cloud starting...");
}

void loop()
{
  ArduinoCloud.update();

  // ---- PERIODIC DEBUG ----
  if (millis() - lastDebug > 3000)
  {
    lastDebug = millis();

    Serial.println("\n--- CONNECTION STATUS ---");

    Serial.print("WiFi status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print("RSSI: ");
      Serial.println(WiFi.RSSI());
    }

    Serial.print("Cloud connected: ");
    Serial.println(ArduinoCloud.connected() ? "YES" : "NO");
  }

  // ---- SENSOR READ ----
  int currentRaw = analogRead(CURRENT_PIN);
  int batteryRaw = analogRead(BATTERY_PIN);

  bool grid = digitalRead(GRID_PIN);

  float currentVoltage = currentRaw * (3.3f / 4095.0f);
  float batteryVoltage = batteryRaw * (3.3f / 4095.0f);

  inverterOn = (currentVoltage > 1.5f);
  gridAvailable = grid;

  int batteryScaled = (int)(batteryVoltage * 100.0f);
  int pct = map(batteryScaled, 200, 300, 0, 100);
  batteryPct = constrain(pct, 0, 100);

  // LEDs
  digitalWrite(POWER_LED, inverterOn ? HIGH : LOW);
  digitalWrite(GRID_LED, gridAvailable ? HIGH : LOW);
  digitalWrite(BATTERY_LED, batteryPct < 30 ? HIGH : LOW);

  // fast debug (lightweight)
  Serial.print("V=");
  Serial.print(currentVoltage, 2);
  Serial.print(" | Batt=");
  Serial.print(batteryPct);
  Serial.print("% | Grid=");
  Serial.println(gridAvailable ? "ON" : "OFF");

  delay(100);
}