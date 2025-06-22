#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <Data.h>
#include "LIS3DHTR.h"
#include "Seeed_BMP280.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <WiFi.h>
#include <ServerCommands.h>
#include <BotCommands.h>
#include "secrets.h"

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#define DHT_PIN 13
#define SOUND_PIN 35
#define LIGHT_PIN 34
#define RAIN_PIN 32
#define RAIN_DIGITAL_PIN 25

#define SDA 22
#define SCL 23

DHT dht(DHT_PIN, DHT11);

BMP280 bmp;

Adafruit_MPU6050 mpu;

LIS3DHTR<TwoWire> LIS; // IIC
#define WIRE Wire

unsigned long latestMillis = 0;
unsigned long currentMillis = 0;
unsigned long updateMillis = 0;

// Anemometer parameters
const float SENSOR_RADIUS = 0.02;     // 2cm from center (in meters)
const float CALIBRATION_FACTOR = 2.5; // Start with 2.5, calibrate for accuracy

// Gyro calibration variables
float gyroOffsetZ = 0;
const int CALIBRATION_SAMPLES = 500;

// Anemometer sampling parameters
float windSpeed = 0;
float accumulatedAngle = 0;
unsigned long lastWindCalc = 0;
const unsigned long WIND_INTERVAL = 3000; // 3-second window
unsigned long lastGyroTime = 0;

DataPacket data;

void calibrateGyro()
{
    Serial.println("Calibrating gyro... keep sensor still");
    float sumZ = 0;

    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        sumZ += g.gyro.z;
        delay(5);
    }

    gyroOffsetZ = sumZ / CALIBRATION_SAMPLES;
    Serial.print("Calibration complete. Z-offset: ");
    Serial.println(gyroOffsetZ);
}

void throwInitializationError(String message)
{
    tft.fillScreen(TFT_RED);
    tft.setCursor(0, 0);
    tft.println("Error during initialization");
    tft.println(message);
    while (1)
        ;
}

void setupSensors()
{
    Wire.begin(SDA, SCL);
    pinMode(SOUND_PIN, INPUT);
    pinMode(LIGHT_PIN, INPUT);
    pinMode(RAIN_PIN, INPUT);
    pinMode(RAIN_DIGITAL_PIN, INPUT);
    LIS.begin(WIRE, LIS3DHTR_ADDRESS_UPDATED);
    LIS.setOutputDataRate(LIS3DHTR_DATARATE_400HZ);
    delay(100);
    if (!LIS)
    {
        throwInitializationError("LIS3DHTR not found");
    }
    dht.begin();
    if (!bmp.init())
    {
        throwInitializationError("BMP280 not connected or broken!");
    }
    if (!mpu.begin())
    {
        throwInitializationError("MPU6050 not connected or broken!");
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);

    // Calibrate gyro
    calibrateGyro();
}

void setup(void)
{
    Serial.begin(115200);
    data.station_id = ESP.getEfuseMac();
    tft.init();
    tft.setRotation(1); // Set rotation to landscape
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("Stazione meteo");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    http.begin(wifi, SERVER_ADDRESS, SERVER_PORT);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        tft.println("Connecting to WiFi..");
    }
    tft.println("Connected to WiFi, with IP: " + WiFi.localIP().toString());
    tft.print("Station ID: ");
    tft.println(data.station_id);
    tft.println("Setup of sensors...");
    setupSensors();
    tft.println("Sensors initialized successfully!");
    setupBot();
    tft.println("Bot setup completed!");
    delay(1000);
}
void measureWindSpeed()
{
    unsigned long now = millis();

    // First call initialization
    if (lastGyroTime == 0)
    {
        lastGyroTime = now;
        return;
    }

    // Get time delta in seconds
    float dt = (now - lastGyroTime) / 1000.0f;
    lastGyroTime = now;

    // Read gyro
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Integrate angular velocity (absolute value)
    accumulatedAngle += abs(g.gyro.z - gyroOffsetZ) * dt;

    // Calculate at fixed intervals
    if (now - lastWindCalc >= WIND_INTERVAL)
    {
        float rotations = accumulatedAngle / (2 * PI);
        float frequency = rotations / (WIND_INTERVAL / 1000.0f);
        windSpeed = 2 * PI * SENSOR_RADIUS * frequency * CALIBRATION_FACTOR;

        // Reset for next interval
        accumulatedAngle = 0;
        lastWindCalc = now;
    }
}

void retrieveData()
{
    measureWindSpeed();                        // Call to measure wind speed
    data.wind_speed = windSpeed;               // Store wind speed in data packet
    data.sound = analogRead(SOUND_PIN) / 10.0; // Scale down for better readability
    data.light = exp(float(analogRead(LIGHT_PIN)) / 80.0);
    data.rain_quantity = analogRead(RAIN_PIN);
    data.isRaining = digitalRead(RAIN_DIGITAL_PIN);
    data.accel_x = LIS.getAccelerationX();
    data.accel_y = LIS.getAccelerationY();
    data.accel_z = LIS.getAccelerationZ();
    data.temp = dht.readTemperature();
    data.hum = dht.readHumidity();
    data.air_pressure = bmp.getPressure() / 100.0; // Convert to hPa
}

void showDataOnScreen()
{
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
    tft.print("Wind Speed: ");
    tft.print(data.wind_speed);
    tft.println(" m/s");
    tft.setCursor(0, 70);
    tft.print("Is Raining: ");
    tft.println(data.isRaining ? "Yes" : "No");
    tft.setCursor(0, 80);
    tft.print("Accel X: ");
    tft.println(data.accel_x);
    tft.setCursor(0, 90);
    tft.print("Accel Y: ");
    tft.println(data.accel_y);
    tft.setCursor(0, 100);
    tft.print("Accel Z: ");
    tft.println(data.accel_z);
    tft.setCursor(0, 110);
    tft.print("ID: ");
    tft.print(data.station_id);
}

void loop()
{
    retrieveData();
    if (data.accel_y > 0.5 || data.accel_z > 0.5)
    {
        sendEarthquakeAlert(data);
    }
    currentMillis = millis();
    if (currentMillis - latestMillis >= 100)
    {
        latestMillis = currentMillis;
        showDataOnScreen();
        updateBot(data);
    }
    if (currentMillis - updateMillis >= 5000)
    {
        updateMillis = currentMillis;
        sendDataPacket(data);
    }
}