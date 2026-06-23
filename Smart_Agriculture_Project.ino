#define BLYNK_TEMPLATE_ID "TMPL3KKGhhARz"
#define BLYNK_TEMPLATE_NAME "Smart Agriculture"
#define BLYNK_AUTH_TOKEN "nOleXn0uIQZno_vT7cGKrgyTg9OSGav_"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

char ssid[] = "vivo";
char pass[] = "Siva9676";

#define DHTPIN 13
#define DHTTYPE DHT11

#define RAIN_PIN 34
#define SOIL_PIN 35

#define BUZZER_PIN 5

#define SERVO1_PIN 18
#define SERVO2_PIN 19

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);

Servo servo1;
Servo servo2;

// Roof Control Button V4
BLYNK_WRITE(V4)
{
  int state = param.asInt();

  if(state == 1)
  {
    servo1.write(90);
    servo2.write(90);
    Serial.println("Roof Closed");
  }
  else
  {
    servo1.write(0);
    servo2.write(0);
    Serial.println("Roof Open");
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  dht.begin();

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(0);
  servo2.write(0);

  Wire.begin(21,22);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED Failed");
    while(1);
  }

  display.clearDisplay();
  display.display();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Smart Agriculture Started");
}

void loop()
{
  Blynk.run();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int rainValue = analogRead(RAIN_PIN);
  int soilValue = analogRead(SOIL_PIN);

  // Soil %
  int soilPercent = map(soilValue, 4095, 0, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  // Rain
  if(rainValue < 2000)
  {
    digitalWrite(BUZZER_PIN, HIGH);

    Blynk.virtualWrite(V0, "RAIN DETECTED");
    Blynk.virtualWrite(V5, 255);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);

    Blynk.virtualWrite(V0, "NO RAIN");
    Blynk.virtualWrite(V5, 0);
  }

  // Blynk Values
  Blynk.virtualWrite(V1, temperature);
  Blynk.virtualWrite(V2, humidity);
  Blynk.virtualWrite(V3, soilPercent);

  // OLED
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0,0);
  display.print("Temp:");
  display.print(temperature);
  display.println(" C");

  display.setCursor(0,15);
  display.print("Hum:");
  display.print(humidity);
  display.println("%");

  display.setCursor(0,30);
  display.print("Soil:");
  display.print(soilPercent);
  display.println("%");

  display.setCursor(0,45);
  display.print("Rain:");
  display.println(rainValue);

  display.display();

  // Serial Monitor
  Serial.println("----------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Rain Raw Value: ");
  Serial.println(rainValue);

  Serial.print("Soil Raw Value: ");
  Serial.println(soilValue);

  Serial.print("Soil Moisture %: ");
  Serial.println(soilPercent);

  delay(2000);
}