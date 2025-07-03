# ESP32 Weather Station with Wind Speed Measurement

This ESP32-based weather station collects environmental data including temperature, humidity, air pressure, light levels, sound levels, and wind speed. It features earthquake detection and Telegram bot integration for remote monitoring.

## Features

- 🌡️ Temperature and humidity monitoring
- 💨 Wind speed measurement using MPU6050 gyroscope
- 🌀 Earthquake detection with accelerometer threshold
- ☁️ Barometric pressure measurement
- 💡 Ambient light sensing
- 🔊 Sound level monitoring
- 🤖 Telegram bot integration for remote data access
- 📶 WiFi connectivity for data transmission
- 📺 TFT display for local data visualization

## Hardware Requirements

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Dev Board | 1 | Main microcontroller |
| DHT11 Sensor | 1 | Temperature and humidity |
| BMP280 Sensor | 1 | Barometric pressure |
| MPU6050 | 1 | Accelerometer/Gyroscope for wind speed and earthquake detection |
| TFT Display | 1 | 1.8" or 2.4" SPI display |
| Sound Sensor | 1 | Analog sound sensor |
| Light Sensor | 1 | Analog light sensor |
| Jumper Wires | - | For connections |
| Breadboard | 1 | Optional for prototyping |

## Pin Connections

| ESP32 Pin | Component | Connection |
|-----------|-----------|------------|
| 3.3V | All sensors | Power |
| GND | All sensors | Ground |
| SDA (22) | I2C Sensors | SDA |
| SCL (23) | I2C Sensors | SCL |
| 13 | DHT11 | Data |
| 35 | Sound Sensor | Analog Input |
| 34 | Light Sensor | Analog Input |
| SPI Pins | TFT Display | Standard SPI connection |

## Setup Instructions

### 1. Install Required Libraries
Install these libraries through Arduino Library Manager:
- `TFT_eSPI`
- `LIS3DHTR`
- `Seeed_BMP280`
- `Adafruit_MPU6050`
- `DHT_sensor_library`
- `WiFi`
- `HTTPClient`
- `ArduinoJson`
- `UniversalTelegramBot`

### 2. Configure secrets.h
Create a `secrets.h` file with your credentials:

```cpp
// WiFi credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASS "your_wifi_password"

// Server configuration
#define SERVER_ADDRESS "YOUR-SERVER-ADDRESS"
#define SERVER_PORT 3000

// Telegram Bot configuration
#define BOT_TOKEN "your_telegram_bot_token"
#define TELEGRAM_CERTIFICATE_ROOT "Telegram certificate"
#define ALLOWED_CHAT_IDS {"chat_id1", "chat_id2"}
```

### 3. Configure TFT_eSPI Library
Edit the `TFT_eSPI` library's `User_Setup.h` file to match your display specifications.

### 4. Upload the Code
1. Connect your ESP32 to your computer
2. Select the correct board in Arduino IDE (ESP32 Dev Module)
3. Select the appropriate port
4. Compile and upload the code

## How to Use

### Initial Setup
1. Power on the device
2. The display will show initialization messages
3. The device will connect to WiFi
4. Sensors will be calibrated (gyroscope calibration will occur)

### Normal Operation
- The display will show all sensor readings
- Data is sent to the server every 5 seconds
- Earthquake alerts are sent when acceleration thresholds are exceeded

### Telegram Bot Commands
Interact with the weather station through Telegram:

- `/start` - Show welcome message
- `/senddata` - Get all current sensor data
- `/sendtemp` - Get current temperature
- `/sendhum` - Get current humidity
- `/sendsound` - Get current sound level
- `/sendlight` - Get current light level
- `/sendpressure` - Get current air pressure
- `/sendwind` - Get current wind speed

## Wind Speed Measurement

The system calculates wind speed using the MPU6050 gyroscope:

1. The gyroscope measures rotational velocity
2. Angular displacement is integrated over time
3. Rotations are converted to linear wind speed
4. Calibration factor adjusts for anemometer geometry

Formula:
```
windSpeed = 2 * π * radius * frequency * calibration_factor
```

## Earthquake Detection

The system monitors acceleration values for sudden movements:
- Triggers when Y or Z acceleration > 0.5g
- Sends immediate alerts to all authorized Telegram chats
- Visual indicator on the display

## File Structure

```
esp32-weather-station/
├── weather-station.ino      # Main Arduino sketch
├── secrets.h                # Configuration file
├── Data.h                   # Data packet structure
├── ServerCommands.h         # HTTP server communication
└── BotCommands.h            # Telegram bot functionality
```

## Dependencies

| Library | Purpose |
|---------|---------|
| `TFT_eSPI` | Display control |
| `WiFi` | WiFi connectivity |
| `HTTPClient` | HTTP communication |
| `ArduinoJson` | JSON data handling |
| `UniversalTelegramBot` | Telegram bot integration |
| `Adafruit_MPU6050` | Gyroscope/accelerometer |
| `DHT_sensor_library` | Temperature/humidity |
| `LIS3DHTR` | Accelerometer |
| `Seeed_BMP280` | Barometric pressure |

## Configuration

### Calibration Parameters
```cpp
// Anemometer parameters
const float SENSOR_RADIUS = 0.02;      // 2cm from center (in meters)
const float CALIBRATION_FACTOR = 12.5; // Calibrate for accuracy

// Gyro calibration
const int CALIBRATION_SAMPLES = 500;

// Earthquake detection threshold
const float EARTHQUAKE_THRESHOLD = 0.5; // 0.5g acceleration
```

### Update Intervals
```cpp
unsigned long latestMillis = 0;
unsigned long currentMillis = 0;
unsigned long updateMillis = 0;

const unsigned long DISPLAY_UPDATE_INTERVAL = 100;   // 100ms
const unsigned long SERVER_UPDATE_INTERVAL = 5000;   // 5 seconds
const unsigned long WIND_INTERVAL = 3000;            // 3-second wind measurement window
```

## Troubleshooting

1. **Sensors not detected**:
   - Check I2C connections (SDA/SCL)
   - Verify sensor power (3.3V)
   - Ensure proper library installation

2. **WiFi connection issues**:
   - Verify WiFi credentials in secrets.h
   - Check router settings
   - Ensure ESP32 is within range

3. **Telegram bot not responding**:
   - Verify bot token
   - Check allowed chat IDs
   - Ensure correct time setting (`configTime()`)

4. **Inaccurate wind measurements**:
   - Calibrate gyroscope properly
   - Adjust calibration factor
   - Ensure stable power supply
