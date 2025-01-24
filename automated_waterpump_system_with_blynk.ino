#define BLYNK_TEMPLATE_ID "TMPL2VWg54cgH"
#define BLYNK_TEMPLATE_NAME "Water gauge"
#define BLYNK_AUTH_TOKEN "M7vkpPUiqqTO-apruo7-AH4zSEkU2kTj"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Ultrasonic.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Redmi Note 12 Pro";
char pass[] = "highspypunk";

Ultrasonic ultrasonic(5, 18); 

const int bulb1 = 23;
const int bulb2 = 16;
const int bulb3 = 17;
const int bulb4 = 19;
const int RELAY_PIN = 13;
float lastNotificationPercentage = -1;
bool manualOverride = false;

void setup() {
  Wire.begin(22, 21); 
  Serial.begin(9600);

  pinMode(bulb1, OUTPUT);
  pinMode(bulb2, OUTPUT);
  pinMode(bulb3, OUTPUT);
  pinMode(bulb4, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT); 

  Serial.println("Connecting to Blynk...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass); 
}

void loop() {
  if (!manualOverride) {  
    if (!Blynk.connected()) {
      Serial.println("Blynk not connected");
      Blynk.connect();
    }
    Blynk.run();

    long distance = ultrasonic.read();
    float percentage = (1.0 - (float)(distance - 3) / (20 - 3)) * 100.0;
    percentage = constrain(percentage, 0, 100);

    Blynk.virtualWrite(V3, percentage);
    sendNotification(percentage);
    controlBulbsAndRelay(distance);
  }
}

void controlBulbsAndRelay(long distance) {
  if (distance > 20) {
    digitalWrite(RELAY_PIN, HIGH);
  } else if (distance < 3) {
    digitalWrite(RELAY_PIN, LOW);
  }

  if (distance > 15 && distance <= 20) {
    updateBulbs(1, 0, 0, 0);
  } else if (distance > 10 && distance <= 15) {
    updateBulbs(1, 1, 0, 0);
  } else if (distance > 3 && distance <= 10) {
    updateBulbs(1, 1, 1, 0);
  } else if (distance <= 3 && distance > 0) {
    updateBulbs(1, 1, 1, 1);
  } else {
    updateBulbs(0, 0, 0, 0);
  }
}

void updateBulbs(int b1, int b2, int b3, int b4) {
  digitalWrite(bulb1, b1);
  digitalWrite(bulb2, b2);
  digitalWrite(bulb3, b3);
  digitalWrite(bulb4, b4);
  Blynk.virtualWrite(V7, b1);
  Blynk.virtualWrite(V6, b2);
  Blynk.virtualWrite(V5, b3);
  Blynk.virtualWrite(V4, b4);
}

void sendNotification(float percentage) {
  if (percentage == 0 && lastNotificationPercentage != 0) {
    Blynk.logEvent("water_0", "Water level is at a critical level (0%)");
    lastNotificationPercentage = 0;
  } else if (percentage <= 25 && lastNotificationPercentage != 25) {
    Blynk.logEvent("water_25", "Water level is increasing (now at 25%)");
    lastNotificationPercentage = 25;
  } else if (percentage <= 50 && lastNotificationPercentage != 50) {
    Blynk.logEvent("water_50", "Water level is at minimal level (50%)");
    lastNotificationPercentage = 50;
  } else if (percentage <= 75 && lastNotificationPercentage != 75) {
    Blynk.logEvent("water_75", "Water level is now at 75%)");
    lastNotificationPercentage = 75;
  } else if (percentage == 100 && lastNotificationPercentage != 100) {
    Blynk.logEvent("water_100", "Water level is full (now at 100%)");
    lastNotificationPercentage = 100;
    digitalWrite(RELAY_PIN, LOW);
  }
}

BLYNK_WRITE(V0) {
  manualOverride = true;  
  int pinValue = param.asInt();
  digitalWrite(RELAY_PIN, pinValue);

  if (pinValue == LOW) {
    manualOverride = false;
  }
  
  Serial.print("Manual relay state changed to: ");
  Serial.println(pinValue ? "HIGH" : "LOW");
}
