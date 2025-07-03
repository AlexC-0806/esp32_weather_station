#ifndef BOT_COMMANDS_H
#define BOT_COMMANDS_H
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Data.h>
#include "secrets.h"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setupBot()
{
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    time_t now = time(nullptr);
}

void sendDataPacketToBot(DataPacket data, String chat_id)
{
    String message = "Data from station ID: " + String(data.station_id) + "\n";
    message += "Temperature: " + String(data.temp) + "C\n";
    message += "Humidity: " + String(data.hum) + "%\n";
    message += "Acceleration X: " + String(data.accel_x) + "\n";
    message += "Acceleration Y: " + String(data.accel_y) + "\n";
    message += "Acceleration Z: " + String(data.accel_z) + "\n";
    message += "Sound: " + String(data.sound) + " dB" + "\n";
    message += "Wind Speed: " + String(data.wind_speed) + " m/s" + "\n";
    message += "Light: " + String(data.light) + " lux" + "\n";
    message += "Air Pressure: " + String(data.air_pressure) + " hPa" + "\n";
    bot.sendMessage(chat_id, message, "");
    Serial.println("Data sent to bot: " + message);
}

void sendTemp(DataPacket data, String chat_id)
{
    String tempMessage = "Current temperature: " + String(data.temp) + "C";
    bot.sendSimpleMessage(chat_id, tempMessage, "");
}
void sendHumidity(DataPacket data, String chat_id)
{
    String humMessage = "Current humidity: " + String(data.hum) + "%";
    bot.sendSimpleMessage(chat_id, humMessage, "");
}
void sendSound(DataPacket data, String chat_id)
{
    String soundMessage = "Current sound level: " + String(data.sound);
    bot.sendSimpleMessage(chat_id, soundMessage, "");
}
void sendLight(DataPacket data, String chat_id)
{
    String lightMessage = "Current light level: " + String(data.light) + " lux";
    bot.sendSimpleMessage(chat_id, lightMessage, "");
}
void sendAirPressure(DataPacket data, String chat_id)
{
    String pressureMessage = "Current air pressure: " + String(data.air_pressure) + " hPa";
    bot.sendSimpleMessage(chat_id, pressureMessage, "");
}

void sendWindSpeed(DataPacket data, String chat_id)
{
    String windMessage = "Current wind speed: " + String(data.wind_speed) + " m/s";
    bot.sendSimpleMessage(chat_id, windMessage, "");
}

void sendEarthquakeAlert(DataPacket data)
{
    String alertMessage = "Earthquake detected! X: " + String(data.accel_x) + ", Y: " + String(data.accel_y) + ", Z: " + String(data.accel_z);
    for (const String &chat_id : ALLOWED_CHAT_IDS)
    {
        bot.sendSimpleMessage(chat_id, alertMessage, "");
    }
}

bool isAuthorized(String chat_id)
{
    for (String allowed_id : ALLOWED_CHAT_IDS)
    {
        if (chat_id == allowed_id)
        {
            return true;
        }
    }
    return false;
}

void handleMessages(int numNewMessages, DataPacket data)
{
    for (int i = 0; i < numNewMessages; i++)
    {
        String text = bot.messages[i].text;
        String chat_id = bot.messages[i].chat_id;

        Serial.println("Message from: " + chat_id);
        Serial.println("Text: " + text);
        if (!isAuthorized(chat_id))
        {
            Serial.println("Unauthorized chat ID: " + chat_id);
            bot.sendSimpleMessage(chat_id, "You are not authorized to use this bot.", "");
            continue;
        }

        if (text == "/start")
        {
            bot.sendSimpleMessage(
                chat_id,
                "Welcome to the Bot! Use /senddata to send data. \n If you are connected to " + String(WIFI_SSID) +
                    " you can see data at this address " + SERVER_ADDRESS + "/" + SERVER_PORT,
                "");
        }
        else if (text == "/senddata")
        {
            sendDataPacketToBot(data, chat_id);
        }
        else if (text == "/sendtemp")
        {
            sendTemp(data, chat_id);
        }
        else if (text == "/sendhum")
        {
            sendHumidity(data, chat_id);
        }
        else if (text == "/sendsound")
        {
            sendSound(data, chat_id);
        }
        else if (text == "/sendlight")
        {
            sendLight(data, chat_id);
        }
        else if (text == "/sendpressure")
        {
            sendAirPressure(data, chat_id);
        }
        else if (text == "/sendwind")
        {
            sendWindSpeed(data, chat_id);
        }
    }
}

void updateBot(DataPacket data)
{
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
        Serial.println("got response");
        handleMessages(numNewMessages, data);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
}

#endif // BOT_COMMANDS_H