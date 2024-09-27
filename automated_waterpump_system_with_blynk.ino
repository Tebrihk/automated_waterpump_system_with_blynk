#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>

#define BLYNK_TEMPLATE_ID "TMPL2XMWK4eBZ"
#define BLYNK_TEMPLATE_NAME "water guage"
#define BLYNK_AUTH_TOKEN "u8JBuezTZiyhmTxtPWUan-4nw-X1xtnb"

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Redmi Note 12 Pro";
char pass[] = "highspypunk";

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Ultrasonic ultrasonic(5, 18); 


const int bulb1 = 23;
const int bulb2 = 16;
const int bulb3 = 17;
const int bulb4 = 19;
const int relayPin = 13;

void setup() {
  Wire.begin(22, 21); 
  Serial.begin(9600);

pinMode(bulb1, OUTPUT);
  pinMode(bulb2, OUTPUT);
  pinMode(bulb3, OUTPUT);
  pinMode(bulb4, OUTPUT);
  pinMode(relayPin, OUTPUT); 

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("connecting to blynk...");
  Serial.println("Connecting to blynk...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.print("IP address");
  lcd.print(WiFi.localIP());

Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(auth, ssid, pass); 
}

void loop() {
  if (!Blynk.connected()) {
    Serial.println("Blynk not connected");
    Blynk.connect(); 
  }

  Blynk.run();
  long distance = ultrasonic.read();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");

float percentage = (1.0 - (float)(distance - 3) / (20 - 3)) * 100.0;

  percentage = constrain(percentage, 0, 100);

  Serial.print("Percentage: ");
  Serial.print(percentage);
  Serial.println(" %");
  static long lastDistance = -1;

  if (distance != lastDistance) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Level is: ");
    lcd.setCursor(0, 1);
    lcd.print(percentage);
    lcd.print(" %");
    lastDistance = distance;
  }

  // Send distance to Blynk gauge
  Blynk.virtualWrite(V2, percentage);
  
  if (distance > 15 && distance <= 20) {
    Blynk.virtualWrite(V7, 1);
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V4, 0);
    digitalWrite(bulb1, HIGH);
    digitalWrite(bulb2, LOW);
    digitalWrite(bulb3, LOW);
    digitalWrite(bulb4, LOW);
    digitalWrite(relayPin, HIGH); 
  } else if (distance > 10 && distance <= 15) {
    Blynk.virtualWrite(V7, 1);
    Blynk.virtualWrite(V6, 1);
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V4, 0);
    digitalWrite(bulb1, HIGH);
    digitalWrite(bulb2, HIGH);
    digitalWrite(bulb3, LOW);
    digitalWrite(bulb4, LOW);
    digitalWrite(relayPin, HIGH); 
  } else if (distance > 5 && distance <= 10) {

    digitalWrite(bulb1, HIGH);
    digitalWrite(bulb2, HIGH);
    digitalWrite(bulb3, HIGH);
    digitalWrite(bulb4, LOW);
    Blynk.virtualWrite(V7, 1);
    Blynk.virtualWrite(V6, 1);
    Blynk.virtualWrite(V5, 1);
    Blynk.virtualWrite(V4, 0);
    digitalWrite(relayPin, HIGH); 
  } else if (distance > 3 && distance <= 5) {
    digitalWrite(bulb1, HIGH);
    digitalWrite(bulb2, HIGH);
    digitalWrite(bulb3, HIGH);
    digitalWrite(bulb4, HIGH);
    delay(200);
    digitalWrite(bulb4, LOW);
    delay(100);
    Blynk.virtualWrite(V7, 1);
    Blynk.virtualWrite(V6, 1);
    Blynk.virtualWrite(V5, 1);
    Blynk.virtualWrite(V4, 1);
    delay(200);
    Blynk.virtualWrite(V4, 0);
    delay(100);
    digitalWrite(relayPin, HIGH); 
    
  } else if (distance <= 3 && distance > 0) {
    digitalWrite(bulb1, HIGH);
    digitalWrite(bulb2, HIGH);
    digitalWrite(bulb3, HIGH);
    digitalWrite(bulb4, HIGH);
    Blynk.virtualWrite(V7, 1);
    Blynk.virtualWrite(V6, 1);
    Blynk.virtualWrite(V5, 1);
    Blynk.virtualWrite(V4, 1);
    digitalWrite(relayPin, LOW); 
  } else {
    digitalWrite(bulb1, LOW);
    digitalWrite(bulb2, LOW);
    digitalWrite(bulb3, LOW);
    digitalWrite(bulb4, LOW);
    Blynk.virtualWrite(V7, 0);
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V5, 0);
    Blynk.virtualWrite(V4, 0);
    digitalWrite(relayPin, LOW); 
  }

  if (!(distance <= 3 && distance > 0)) {
    delay(1500);
  }
}