#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Data.h>
#include "secrets.h"

WiFiClient wifi;
HTTPClient http;

// Helper function to handle HTTP response
bool handleResponse(int httpResponseCode)
{
    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        return true;
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return false;
    }
}

// Individual data sending functions
void sendTemp(float temp)
{
    StaticJsonDocument<64> doc;
    doc["temp"] = temp;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/temperature");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendHum(float hum)
{
    StaticJsonDocument<64> doc;
    doc["hum"] = hum;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/humidity");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendSound(float sound)
{
    StaticJsonDocument<64> doc;
    doc["sound"] = sound;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/sound");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendLight(float light)
{
    StaticJsonDocument<64> doc;
    doc["light"] = light;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/light");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendAccelData(float x, float y, float z)
{
    StaticJsonDocument<128> doc;
    doc["accel_x"] = x;
    doc["accel_y"] = y;
    doc["accel_z"] = z;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/acceleration");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendRainData(float rain_quantity, bool isRaining)
{
    StaticJsonDocument<128> doc;
    doc["rain_quantity"] = rain_quantity;
    doc["isRaining"] = isRaining;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/rain");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendAirPressure(float air_pressure)
{
    StaticJsonDocument<64> doc;
    doc["air_pressure"] = air_pressure;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/pressure");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

void sendStationId(uint64_t station_id)
{
    StaticJsonDocument<64> doc;
    doc["station_id"] = station_id;

    String jsonString;
    serializeJson(doc, jsonString);

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/station");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    handleResponse(httpResponseCode);
    http.end();
}

// Function to send complete data packet
bool sendDataPacket(const DataPacket &data)
{
    // Using the toJson method from DataPacket class
    String jsonString = data.toJson();

    http.begin(wifi, String(SERVER_ADDRESS) + ":" + String(SERVER_PORT) + "/data");
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    bool success = handleResponse(httpResponseCode);
    http.end();
    return success;
}

#endif // SERVER_COMMANDS_H