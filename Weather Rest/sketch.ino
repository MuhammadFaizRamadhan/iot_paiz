#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define NEXT_BUTTON 18
#define BACK_BUTTON 19

const char* ssid = "Wokwi-GUEST";
const char* password = "";
String apiKey = "092d40e705576a5cdbb937b38eb40d7d";
String city = "Malang";
String units = "metric";
String server = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&units=" + units + "&appid=" + apiKey;

LiquidCrystal_I2C lcd(0x27, 16, 2);

float temp, feels_like, precipitation, wind_speed, wind_deg, humidity, clouds, pressure;
String sunrise, sunset;
int page = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();
  
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }
  lcd.clear();
  lcd.print("Connected!");
  delay(1000);
  lcd.clear();

  pinMode(NEXT_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);

  getWeatherData();
  showPage();
}

void loop() {
  if (millis() - lastDebounceTime > debounceDelay) {
    if (digitalRead(NEXT_BUTTON) == LOW) {
      page++;
      if (page > 9) page = 0;
      showPage();
      lastDebounceTime = millis();
    }
    if (digitalRead(BACK_BUTTON) == LOW) {
      page--;
      if (page < 0) page = 9;
      showPage();
      lastDebounceTime = millis();
    }
  }
}

void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(server);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      StaticJsonDocument<2000> doc;
      deserializeJson(doc, payload);

      temp = doc["main"]["temp"];
      feels_like = doc["main"]["feels_like"];
      precipitation = doc["rain"]["1h"] | 0; 
      wind_speed = doc["wind"]["speed"];
      wind_deg = doc["wind"]["deg"];
      humidity = doc["main"]["humidity"];
      clouds = doc["clouds"]["all"];
      pressure = doc["main"]["pressure"];
      unsigned long sunrise_ts = doc["sys"]["sunrise"];
      unsigned long sunset_ts = doc["sys"]["sunset"];

      sunrise = convertUnixTime(sunrise_ts);
      sunset = convertUnixTime(sunset_ts);
    } else {
      Serial.println("HTTP Request Failed");
    }
    http.end();
  }
}

String convertUnixTime(unsigned long timestamp) {
  time_t rawtime = timestamp;
  struct tm * ti;
  ti = gmtime(&rawtime);
  char buffer[6];
  sprintf(buffer, "%02d:%02d", (ti->tm_hour + 7) % 24, ti->tm_min); // WIB = GMT+7
  return String(buffer);
}

void showPage() {
  lcd.clear();
  switch (page) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.setCursor(0, 1);
      lcd.print(temp);
      lcd.print(" C");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Feels Like:");
      lcd.setCursor(0, 1);
      lcd.print(feels_like);
      lcd.print(" C");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Precip:");
      lcd.setCursor(0, 1);
      lcd.print(precipitation);
      lcd.print(" mm");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Wind Speed:");
      lcd.setCursor(0, 1);
      lcd.print(wind_speed);
      lcd.print(" m/s");
      break;
    case 4:
      lcd.setCursor(0, 0);
      lcd.print("Wind Dir:");
      lcd.setCursor(0, 1);
      lcd.print(wind_deg);
      lcd.print(" deg");
      break;
    case 5:
      lcd.setCursor(0, 0);
      lcd.print("Humidity:");
      lcd.setCursor(0, 1);
      lcd.print(humidity);
      lcd.print(" %");
      break;
    case 6:
      lcd.setCursor(0, 0);
      lcd.print("Clouds:");
      lcd.setCursor(0, 1);
      lcd.print(clouds);
      lcd.print(" %");
      break;
    case 7:
      lcd.setCursor(0, 0);
      lcd.print("Pressure:");
      lcd.setCursor(0, 1);
      lcd.print(pressure);
      lcd.print(" hPa");
      break;
    case 8:
      lcd.setCursor(0, 0);
      lcd.print("Sunrise:");
      lcd.setCursor(0, 1);
      lcd.print(sunrise);
      break;
    case 9:
      lcd.setCursor(0, 0);
      lcd.print("Sunset:");
      lcd.setCursor(0, 1);
      lcd.print(sunset);
      break;
  }
}
