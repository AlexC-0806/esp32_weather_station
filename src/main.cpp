#include <Arduino.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TFT_eSPI.h>       // Hardware-specific library
#include <Data.h>


TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

#define RX 27
#define TX 26

DataPacket data;

SoftwareSerial mySerial(RX, TX);

void setup(void) {
    pinMode(RX, INPUT);
    pinMode(TX, OUTPUT);    
    mySerial.begin(9600);
    Serial.begin(9600);
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.print("Hello, World!");
    delay(1000);

}

void retrieveData() {
    if (mySerial.available() > 0) {
        String serialMessage = mySerial.readStringUntil('\n');
        data = DataPacket::fromJson(serialMessage);
    }
}

void loop() {
    retrieveData();
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
 }