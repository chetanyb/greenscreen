#include <Arduino.h>
#include <SHT1x-ESP.h>
#include <MQ135.h>
#include <vector>
#include <Wire.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <HTTPClient.h>

#define SHT10_DATA_PIN  21
#define SHT10_CLOCK_PIN 22
#define MQ135_ANALOG_PIN 34
#define SSID "please"
#define PASSWORD "trialnetwork"
#define GREEN_LED_PIN 25
#define YELLOW_LED_PIN 26
#define RED_LED_PIN 23
#define BUTTON_PIN 32
#define EXHAUST_FLOW_RATE 0.05

SHT1x sht10(SHT10_DATA_PIN, SHT10_CLOCK_PIN);
MQ135 mq135_sensor(MQ135_ANALOG_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
AsyncWebServer server(80);

bool isMeasuring = false;
unsigned long startTime = 0;
unsigned long stopTime = 0;
float totalEmissions = 0;

struct CO2Reading {
  float concentration;
  unsigned long timestamp;
};

std::vector<CO2Reading> co2Readings;

void calculateAndDisplayResults();
float calculateCO2Emissions(float co2_ppm, float flow_rate_m3s);
void handleCO2Endpoint(AsyncWebServerRequest *request);
void wifiServerTask(void *pvParameters);
void measureCO2Task(void *pvParameters);
void httpGetTask(void * parameter);

void setup() {
  Serial.begin(115200);
  Wire.begin(19, 18);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CO2 Emissions");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  xTaskCreatePinnedToCore(wifiServerTask, "WifiServerTask", 10000, NULL, 1, NULL, 0); // Core 0
  xTaskCreatePinnedToCore(measureCO2Task, "MeasureCO2Task", 10000, NULL, 1, NULL, 1); // Core 1
}

void loop() {
  // Empty loop
}

void wifiServerTask(void *pvParameters) {
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());

  server.on("/co2", HTTP_GET, handleCO2Endpoint);
  server.begin();

  for (;;) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  server.on("/triggerHttpRequest", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Start the HTTP request task when this endpoint is accessed
    xTaskCreate(
        httpGetTask,          /* Task function */
        "HTTPGetTask",        /* Name of the task */
        10000,                /* Stack size (in bytes) */
        NULL,                 /* Parameter to pass to the task */
        1,                    /* Task priority */
        NULL);                /* Task handle */

    // Respond to the HTTP request
    request->send(200, "text/plain", "HTTP request triggered");
});

}

void measureCO2Task(void *pvParameters) {
  unsigned long lastSampleTime = 0;
  for (;;) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      if (isMeasuring) {
        stopTime = millis();
        isMeasuring = false;
        calculateAndDisplayResults();
      } else {
        startTime = millis();
        isMeasuring = true;
        lcd.clear();
        lcd.print("Measuring CO2...");
        lcd.setCursor(0, 1);
        lcd.print("SCALED for demo");
      }
      delay(500); // Debounce delay
    }

    if (isMeasuring && millis() - lastSampleTime >= 2000) {
      lastSampleTime = millis();
      CO2Reading reading;
      reading.concentration = mq135_sensor.getCorrectedPPM(sht10.readTemperatureC(), sht10.readHumidity());
      reading.timestamp = millis();
      co2Readings.push_back(reading);

      Serial.print("CO2 Concentration: ");
      Serial.print(reading.concentration);
      Serial.print(" ppm at ");
      Serial.println(reading.timestamp);

      if (reading.concentration < 200) {
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
      } else if (reading.concentration < 2000) {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
      } else {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(YELLOW_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void calculateAndDisplayResults() {
  float lastEmission = 0;
  for (size_t i = 1; i < co2Readings.size(); i++) {
    float timeIntervalHours = (co2Readings[i].timestamp - co2Readings[i - 1].timestamp) / 3600000.0;
    float emission = calculateCO2Emissions(co2Readings[i].concentration, EXHAUST_FLOW_RATE) * timeIntervalHours;
    lastEmission = calculateCO2Emissions(co2Readings[i].concentration, EXHAUST_FLOW_RATE);
    totalEmissions += lastEmission;
  }

  lcd.clear();
  lcd.print("Total Emissions:");
  lcd.setCursor(0, 1);
  lcd.print(totalEmissions);
  lcd.print(" tonnes");
  co2Readings.clear();
  xTaskCreate(
        httpGetTask,          /* Task function. */
        "HTTPGetTask",        /* String with name of task. */
        10000,                /* Stack size in bytes. */
        NULL,                 /* Parameter passed as input of the task */
        1,                    /* Priority of the task. */
        NULL);                /* Task handle. */

    co2Readings.clear();
}

float calculateCO2Emissions(float co2_ppm, float flow_rate_m3s) {
  float mass_fraction = co2_ppm / 1000000.0;
  float density_of_exhaust = 1.204;
  float mass_of_CO2_per_second = flow_rate_m3s * density_of_exhaust * mass_fraction;
  return mass_of_CO2_per_second * 3600 * 0.001 * 100;
}

void handleCO2Endpoint(AsyncWebServerRequest *request) {
    StaticJsonDocument<200> doc;
    doc["message"] = "Latest CO2 emission reading SCALED BY 100000 for demo purposes";
    doc["emission"] = totalEmissions;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
}

void httpGetTask(void * parameter){
    HTTPClient http;
    http.begin("http://e8f8-213-43-1-133.ngrok-free.app/sendRequest");
    http.addHeader("ngrok-skip-browser-warning", "1"); 

    int httpCode = http.GET(); // Perform the GET request
    if(httpCode > 0) {
        String payload = http.getString();
        Serial.println(payload); // Print the response payload
    } else {
        Serial.println("Error on HTTP request");
    }

    http.end(); // Free the resources
    vTaskDelete(NULL); // Delete the task when done
}
