# 🌫️ IoT-Based Air Quality Monitoring System Using CAN Protocol

This project monitors environmental air quality parameters like **PM2.5**, **PM10**, **Temperature**, **Humidity**, and **Air Quality (CO2 ppm)** using various sensors and transmits the data over the **CAN (Controller Area Network) protocol**. The data is displayed locally on an **OLED screen** and remotely pushed to **ThingSpeak** using Wi-Fi for IoT analytics.

---

## 📦 Features

- Real-time air quality sensing with:
  - **SDS011** for PM2.5 and PM10
  - **DHT22** for Temperature and Humidity
  - **MQ135** for CO2-based Air Quality (ppm)
- **CAN protocol** for communication between transmitting and receiving nodes
- Displays values on **OLED** (SSD1306)
- Sends data to **ThingSpeak** IoT cloud via Wi-Fi
- Displays condition: *“Air Quality is Good”* or *“Not Good”* on OLED

---

## 🛠️ Hardware Used

- ESP32 Dev Kit
- SDS011 Dust Sensor
- DHT22 Temperature and Humidity Sensor
- MQ135 Gas Sensor
- MCP2515 CAN Module with TJA1050
- OLED Display (SSD1306, I2C)
- Power Supply, Jumper Wires, Breadboard

---

## 🔌 Connections Overview

### Transmitting Node (ESP32):
| Component      | Pin             |
|----------------|------------------|
| DHT22          | GPIO 4           |
| MQ135          | GPIO 34 (ADC)    |
| SDS011         | GPIO 26 (TX), GPIO 27 (RX) |
| OLED (I2C)     | SDA/SCL (Default)|
| CAN Module     | SPI: GPIO 5 (CS) |

### Receiving Node (ESP32):
| Component      | Pin             |
|----------------|------------------|
| CAN Module     | SPI: GPIO 5 (CS) |

---



