#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library
#include <Data.h>
#include "LIS3DHTR.h"
#include "Seeed_BMP280.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <WiFi.h>
#include "secrets.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

#define DHT_PIN 12
#define SOUND_PIN 14
#define LIGHT_PIN 27
#define RAIN_PIN 26
#define RAIN_DIGITAL_PIN 25

#define SDA 23
#define SCL 22

DHT dht(DHT_PIN, DHT11);

BMP280 bmp;

LIS3DHTR<TwoWire> LIS; //IIC
#define WIRE Wire

DataPacket data;

void throwInitializationError(String message){
    tft.fillScreen(TFT_RED);
    tft.setCursor(0, 0);
    tft.println("Error during initialization");
    tft.println(message);
    while(1);
}

void setupSensors(){
    Wire.begin(SDA, SCL);
    pinMode(SOUND_PIN, INPUT);
    pinMode(LIGHT_PIN, INPUT);
    pinMode(RAIN_PIN, INPUT);
    pinMode(RAIN_DIGITAL_PIN, INPUT);
    LIS.begin(WIRE, LIS3DHTR_ADDRESS_UPDATED); 
    LIS.setOutputDataRate(LIS3DHTR_DATARATE_400HZ);
    delay(100);
    if(!LIS){
        throwInitializationError("LIS3DHTR not found");
    }
    dht.begin();
    bmp.init();
    if (!bmp.init()) {
        Serial.println("BMP280 not connected or broken!");
      }
}

void setup(void) {
    Serial.begin(115200);
    data.station_id = ESP.getEfuseMac();
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("Stazione meteo");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        tft.println("Connecting to WiFi..");
    }
    tft.println("Connected to WiFi, with IP: " + WiFi.localIP().toString());
    tft.print("Station ID: ");
    tft.println(data.station_id);
    tft.println("Setup of sensors...");
    setupSensors();
    delay(1000);
}

void retrieveData() {
    data.sound = analogRead(SOUND_PIN);
    data.light = analogRead(LIGHT_PIN);
    data.rain_quantity = analogRead(RAIN_PIN);
    data.isRaining = digitalRead(RAIN_DIGITAL_PIN);
    data.accel_x = LIS.getAccelerationX();
    data.accel_y = LIS.getAccelerationY();
    data.accel_z = LIS.getAccelerationZ();
    data.temp = dht.readTemperature();
    data.hum = dht.readHumidity();
    data.air_pressure = bmp.getPressure();
}

void showDataOnScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.print("Temperature: ");
    tft.print(data.temp);
    tft.print("C");
    tft.setCursor(0, 10);
    tft.print("Humidity: ");
    tft.print(data.hum);
    tft.print("%");
    tft.setCursor(0, 20);
    tft.print("Sound: ");
    tft.print(data.sound);
    tft.setCursor(0, 30);
    tft.print("Light: ");
    tft.println(data.light);
    tft.setCursor(0, 40);
    tft.print("Rain Quantity: ");
    tft.println(data.rain_quantity);
    tft.setCursor(0, 50);
    tft.print("Air Pressure: ");
    tft.println(data.air_pressure);
    tft.setCursor(0, 60);
    tft.print("Is Raining: ");
    tft.println(data.isRaining);
    tft.setCursor(0, 70);
    tft.print("Accel X: ");
    tft.println(data.accel_x);
    tft.setCursor(0, 80);
    tft.print("Accel Y: ");
    tft.println(data.accel_y);
    tft.setCursor(0, 90);
    tft.print("Accel Z: ");
    tft.println(data.accel_z);
    tft.setCursor(0,100);
    tft.print("ID: ");
    tft.print(data.station_id);
}

void loop() {
    retrieveData();
    showDataOnScreen();
 }