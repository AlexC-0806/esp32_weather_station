#include <ArduinoJson.h>

class DataPacket {
public:
    uint64_t station_id;
    float temp;
    float hum;
    float accel_x;
    float accel_y;
    float accel_z;
    float sound;
    float rain_quantity;
    bool isRaining;
    float light;
    float air_pressure;

    String toJson() const {
        StaticJsonDocument<256> doc;
        doc["station_id"] = station_id;
        doc["temp"] = temp;
        doc["hum"] = hum;
        doc["accel_x"] = accel_x;
        doc["accel_y"] = accel_y;
        doc["accel_z"] = accel_z;
        doc["sound"] = sound;
        doc["rain_quantity"] = rain_quantity;
        doc["isRaining"] = isRaining;
        doc["light"] = light;
        doc["air_pressure"] = air_pressure;

        String output;
        serializeJson(doc, output);
        return output;
    }

    static DataPacket fromJson(const String& jsonString) {
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
        packet.rain_quantity = doc["rain_quantity"];
        packet.isRaining = doc["isRaining"];
        packet.light = doc["light"];
        packet.air_pressure = doc["air_pressure"];

        return packet;
    }
};