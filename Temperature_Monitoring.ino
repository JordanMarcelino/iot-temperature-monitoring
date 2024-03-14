#include <Arduino.h>
#include <math.h>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#define DHTTYPE DHT22
#define WIFI_SSID "EXCLUSIVE ACCESS"
#define WIFI_PASSWORD "digitalsnn"

const int LED = 13;
const int DHTPIN = 26;

void connectWifi();
void postHttp();

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  dht.begin();
  connectWifi();

  delay(2000);
}

void loop()
{
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(humidity) || isnan(temp))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (temp <= 18.0 || temp >= 30.0)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temp);
  Serial.println("°C ");

  postHttp(temp, humidity);

  delay(5000);
}

void connectWifi()
{
  Serial.println("Connecting to wifi!");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    digitalWrite(LED, !digitalRead(LED));
    delay(1000);
  }

  Serial.println("Wifi Connected");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}

void postHttp(float temp, float humidity)
{
  Serial.println("[HTTP] begin...");

  String url = "http://192.168.43.67:5000/api/dht";
  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  DynamicJsonDocument jsonDoc(256);

  float roundedTemp = round(temp * 100.0) / 100.0;
  float roundedHumidity = round(humidity * 100.0) / 100.0;

  jsonDoc["temperature"] = roundedTemp;
  jsonDoc["humidity"] = roundedHumidity;

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);
  Serial.println(jsonStr);

  int httpCode = http.POST(jsonStr);

  if (httpCode > 0)
  {
    if (httpCode == 201)
    {
      String payload = http.getString();
      Serial.println(payload);
    }
    else
    {
      Serial.printf("HTTP error code: %d\n", httpCode);
    }
  }
  else
  {
    Serial.println("Failed to connect to server");
  }

  http.end();
}
