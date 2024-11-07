# SmartLights Documentation

SmartLights is a DIY smart lighting system using NodeMCU (ESP8266), Blynk for mobile app integration, and capacitive touch sensors (TTP223) to control relay-based lighting. The system offers local and remote control of up to 4 lights, with optional extension to 8 channels.

## Table of Contents

1. Overview
2. Hardware and Library Requirements
3. Configuration
4. Core Features
5. OTA (Over-The-Air) Updates
6. Error Handling
7. Example Code Setup

---

## 1. Overview

The SmartLights project integrates touch sensors, relays, and NodeMCU to create a versatile lighting system. Using Blynk and local network setup, users can control lights remotely from a smartphone. Additionally, it includes Over-The-Air (OTA) updates for seamless firmware upgrades.

## 2. Hardware and Library Requirements
![Full circuit](https://github.com/user-attachments/assets/fffaf704-6de6-4f28-a9f0-d6b5900be280)


### Hardware
- **NodeMCU (ESP8266)**
- **4-Channel Relay Module** (expandable to 8)
- **TTP223 Capacitive Touch Sensors** (one for each relay)
- **240v to 5v step down power supply** 

![Esp8266](https://github.com/user-attachments/assets/ce149b00-574c-4b08-82bc-2c69bcab5fec)
![RelayModule](https://github.com/user-attachments/assets/80e6e35f-5c46-4cae-ad1b-9e8c781b3f64)
![TouchSensor](https://github.com/user-attachments/assets/2d5f21fb-2139-4d77-bfc6-8d3a453b9e5e)


### Libraries
- **Blynk**: For remote control through Blynk mobile app
- **ESP8266WiFi**: For WiFi connectivity
- **ESP8266WebServer**: For web server setup
- **ArduinoOTA**: For OTA updates

```cpp
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
```

## 3. Configuration

### WiFi and Blynk Setup
Add your WiFi credentials and Blynk authentication token:

```cpp
#define WIFI_SSID "Your_SSID"
#define WIFI_PASS "Your_Password"
#define AUTH "Your_Blynk_Auth_Token"
#define SERVER "blynk-cloud.com"
#define PORT 8442
```

### Pin Definitions
Configure pins for relays and touch sensors based on your wiring:

```cpp
#define RELAY_PIN_1 D1
#define RELAY_PIN_2 D2
#define RELAY_PIN_3 D3
#define RELAY_PIN_4 D4

#define TOUCH_SENSOR_1 D5
#define TOUCH_SENSOR_2 D6
#define TOUCH_SENSOR_3 D7
#define TOUCH_SENSOR_4 D8

#define VPIN_BUTTON_1 V1
#define VPIN_BUTTON_2 V2
#define VPIN_BUTTON_3 V3
#define VPIN_BUTTON_4 V4
```

## 4. Core Features

### 4.1 Blynk Control
The Blynk integration allows remote control via virtual pins. When a button is toggled in the Blynk app, it triggers corresponding relays on the NodeMCU:

```cpp
BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}
```

### 4.2 Touch Sensor Control
Each relay has an associated touch sensor for manual control. The `checktouchButton()` function continuously monitors the state of each sensor to toggle relay states and update the Blynk app:

```cpp
void checktouchButton() {
  if (digitalRead(TOUCH_SENSOR_1) == LOW) {
    relay1State = !relay1State;
    digitalWrite(RELAY_PIN_1, relay1State);
    Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
  }
  // Repeat for other relays and sensors...
}
```

### 4.3 WiFi and MDNS Setup
On startup, the NodeMCU connects to WiFi and starts an MDNS responder for easy local network access:

```cpp
WiFi.begin(ssid, password);
MDNS.begin("smartlights");
```

## 5. OTA (Over-The-Air) Updates

This feature allows firmware updates without needing a USB connection. The NodeMCU serves a simple HTML form for uploading new firmware:

```cpp
server.on("/update", HTTP_POST, []() {
  server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
});
```

## 6. Error Handling and Reset

### Restart Bug
On restart, lights may turn on randomly due to pin states. Implement a state-saving mechanism (e.g., saving states in EEPROM or flash memory) to preserve relay states across reboots.

### WiFi Failure
If WiFi fails to connect, the device prints an error message on the serial monitor. Retry logic can be added to ensure reconnection attempts.

## 7. Example Code Setup

Here's a basic setup function demonstrating the relay and WiFi initialization:

```cpp
void setup() {
  Serial.begin(115200);
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);

  // Initialize relays and touch sensors
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(TOUCH_SENSOR_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);

  // Repeat for other relays...

  timer.setInterval(500L, checktouchButton);
}
```
