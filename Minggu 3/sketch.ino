#define BLYNK_DEVICE_NAME "Esp32Coba"
#define BLYNK_PRINT Serial

#define BLYNK_AUTH_TOKEN "Ev5T621129NdER7HqQ1Sv-9RYIJp7B-M"
#define BLYNK_TEMPLATE_ID "TMPL6G-x2Le-P"
#define BLYNK_TEMPLATE_NAME "Paiz"

#include <Arduino.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHTesp.h>

char auth[] = BLYNK_AUTH_TOKEN ;

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const int DHT_PIN = 15;

int value0, value1, value2, value3, value6;

byte LED_R = 26;
byte LED_Y = 27;
byte LED_G = 14;
byte LED_B = 12;

DHTesp dht;

BlynkTimer timer;

void sendSensor()
{
 TempAndHumidity  data = dht.getTempAndHumidity();

Serial.print("% Temperature: ");
Serial.print(data.temperature);
Serial.println("C ");
Serial.print("% Kelembaban: ");
Serial.print(data.humidity);
Serial.println("% ");

Blynk.virtualWrite(V4, data.temperature);
Blynk.virtualWrite(V3, data.humidity);
}

BLYNK_WRITE(V5)
{
  int nilaiBacaIO =param.asInt();
   digitalWrite(LED_R, nilaiBacaIO);
  Blynk.virtualWrite(V6, nilaiBacaIO);
}

void setup()
{
Serial.begin(115200);
dht.setup(DHT_PIN, DHTesp::DHT22);
pinMode(LED_R, OUTPUT);

Blynk.begin(auth, ssid, pass);
timer.setInterval(1000, sendSensor);
}

void loop()
{

Blynk.run();
timer.run();
}