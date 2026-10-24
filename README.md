# Coffee Machine SH3 with Sound

## Project Description
This project is an ESP32-based smart coffee vending machine controller designed for RFID-based access and purchase processing. It reads RFID cards, verifies the user with a backend server, allows the user to choose coffee or tea, controls the dispensing outputs, and plays audio feedback using a DFPlayer Mini module.

The system is built for smart office, hostel, or facility environments where users can access a vending machine through a card-based authentication system. It connects to WiFi, supports MQTT commands, performs OTA checks, and submits purchase records to a remote server.

## Main Features
- RFID card verification using RDM6300
- Coffee and tea dispensing control
- DFPlayer Mini audio playback support
- WiFi connectivity with WiFiManager fallback portal
- MQTT command support for remote control and OTA triggering
- HTTPS verification and purchase submission to backend server
- Local preferences storage for audio settings and volume
- OTA update support using server API
- Automatic WiFi reconnect handling
- Audio messages for success, errors, invalid cards, and request timeouts

## Hardware Components
- ESP32 development board
- RDM6300 RFID reader module
- DFPlayer Mini MP3 module
- Speaker / audio output
- Relay or transistor-driven coffee and tea outputs
- LEDs for status indication
- Buzzer input for machine state detection
- Door / machine usable input logic
- Coffee and tea selection inputs

## Pin Configuration
The code defines the main pins used for the machine:

- RFID reader:
  - `RDM6300_RX_PIN = 13`
- Status LEDs:
  - `GREEN_LED_PIN = 15`
  - `RED_LED_PIN = 4`
- Beverage outputs:
  - `OUTPUT_COFFEE = 5`
  - `OUTPUT_TEA = 18`
- Inputs:
  - `INPUT_BUZZER = 22`
  - `INPUT_USABLE = 21`
  - `INPUT_COFFEE = 19`
  - `INPUT_TEA = 23`
- DFPlayer serial pins:
  - `DFPLAYER_RX = 16`
  - `DFPLAYER_TX = 17`

## Software Libraries Used
The project requires the following Arduino libraries:
- `WiFi.h`
- `WiFiManager.h`
- `HTTPClient.h`
- `LittleFS.h`
- `ArduinoJson.h`
- `rdm6300.h`
- `Preferences.h`
- `DFRobotDFPlayerMini.h`
- `PubSubClient.h`
- `Update.h` (built into ESP32 core)

## Project Workflow
1. The ESP32 boots and initializes RFID, MQTT, WiFi, and DFPlayer.
2. It tries to connect to WiFi with saved settings or opens the WiFiManager portal if needed.
3. It subscribes to MQTT topics for device commands and OTA updates.
4. When a card is scanned, the device sends the card number to the server.
5. The server returns the user information and allowed purchase details.
6. The user selects coffee or tea through the input buttons.
7. The machine activates the correct beverage output.
8. After product delivery, purchase details are submitted to the server.
9. The device plays the corresponding audio feedback.
10. If internet connectivity fails, the machine can notify the user and skip or retry the request safely.

## Core Operation Logic
The machine performs the following main actions:
- Card check via HTTP request
- User validation via backend server
- Selection timeout and machine readiness checks
- Beverage activation
- Machine response validation using buzzer input
- Purchase submission to server
- Audio message playback for success or failure states

## Server Communication
The code communicates with HTTP endpoints to validate cards and submit purchases.

The project uses variables like:
- `url_1`
- `url_2`
- `sub_url_1`
- `sub_url_2`
- `api_secret`
- `MY_MACHINE_ID`
- `MY_MACHINE_TYPE_ID`

These must be configured correctly for your deployment environment.

## MQTT Functionality
The device listens to MQTT topics such as:
- device command topic
- global OTA update topic: `ota/coffee/update`
- status and event topics for each device

It supports commands like:
- `coffee`
- `tea`
- `run`
- `stop`
- `update`
- `reset`
- `get`
- `volume`
- `set_audio`

## OTA Update Support
The project includes OTA update logic. It checks a remote API for a newer firmware version and downloads the new firmware if available. If a valid update is found, the ESP32 writes the firmware to flash and restarts automatically.

## Audio System
The sketch includes a large set of audio track mappings for different machine states, such as:
- invalid card
- no access
- low balance
- purchase success
- coffee ready
- tea ready
- machine unusable
- waiting for selection
- WiFi not connected
- HTTP failure
- invalid JSON response
- user not found
- selection timeout

The audio numbers are configurable and can also be overridden and stored in flash preferences.

## Important Notes
- `api_secret`, `mqttServer`, `mqttUser`, `mqttPassword`, and server URLs are currently placeholder values in the source code. Replace them before deployment.
- The WiFiManager portal is enabled for initial network configuration.
- The code uses HTTPS with `setInsecure()`, which is acceptable for test or internal deployment but should be replaced with proper certificate validation for production.
- The machine logic depends on physical hardware states such as input signals and mechanical feedback from the dispenser. Test carefully on real hardware before deployment.

## Upload Instructions
1. Open `Coffee_Machine_SH3_with-sound.ino` in the Arduino IDE.
2. Install the required libraries.
3. Select the correct ESP32 board and COM port.
4. Update the API, MQTT, and machine configuration values in the code.
5. Compile and upload the firmware.
6. Open the serial monitor to observe startup, WiFi connection, RFID verification, and OTA status.

## File Structure
- `Coffee_Machine_SH3_with-sound.ino` – main firmware
- `README.md` – documentation

## Example Use Cases
- Smart coffee vending machine for offices
- Hostel or staff cafeteria access machine
- Self-service beverage station
- RFID-based employee beverage automation

## Disclaimer
This project is intended for embedded systems development and testing. For production deployment, ensure security, network configuration, and hardware reliability are fully validated in your real environment.
