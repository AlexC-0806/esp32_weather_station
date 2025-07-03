#ifndef DATA_H
#define DATA_H

#include <ArduinoJson.h>

class DataPacket
{
public:
    uint64_t station_id;
    float temp;
    float hum;
    float accel_x;
    float accel_y;
    float accel_z;
    float sound;
    float wind_speed;
    float light;
    float air_pressure;

    String toJson() const
    {
        StaticJsonDocument<256> doc;
        doc["station_id"] = station_id;
        doc["temp"] = temp;
        doc["hum"] = hum;
        doc["accel_x"] = accel_x;
        doc["accel_y"] = accel_y;
        doc["accel_z"] = accel_z;
        doc["sound"] = sound;
        doc["wind_speed"] = wind_speed;
        doc["light"] = light;
        doc["air_pressure"] = air_pressure;

        String output;
        serializeJson(doc, output);
        return output;
    }

    static DataPacket fromJson(const String &jsonString)
    {
        StaticJsonDocument<256> doc;
        deserializeJson(doc, jsonString);

        DataPacket packet;
        packet.station_id = doc["station_id"];
        packet.temp = doc["temp"];
        packet.hum = doc["hum"];
        packet.accel_x = doc["accel_x"];
        packet.accel_y = doc["accel_y"];
        packet.accel_z = doc["accel_z"];
        packet.sound = doc["sound"];
        packet.wind_speed = doc["wind_speed"];
        packet.light = doc["light"];
        packet.air_pressure = doc["air_pressure"];

        return packet;
    }
};

#endif // DATA_H