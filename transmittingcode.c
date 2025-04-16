#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <mcp_can.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <SDS011.h>
#include <Arduino.h>
// OLED Display Setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT22 Sensor
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// MQ135 Sensor
#define MQ135_PIN 34
#define RL_VALUE 10.0 // Load resistance in kΩ
#define CLEAN_AIR_RATIO 3.6 // Rs/R0 ratio in clean air

float R0 = 6.67; // Manually calibrated R0 based on clean air Rs readings

// SDS011 Sensor
HardwareSerial mySerial(1);
SDS011 my_sds;
#define SDSG1 26
#define SDSG2 27
// MCP2515 CAN Module
#define SPI_CS_PIN 5
MCP_CAN CAN(SPI_CS_PIN);

// Global Sensor Variables
float pm25, pm10, temperature, humidity;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize OLED
    if (!display.begin(SSD1306_BLACK, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    display.clearDisplay();

    // Initialize DHT Sensor
    dht.begin();

    // Initialize SDS011
mySerial.begin(9600, SERIAL_8N1, SDSG1, SDSG2);  // Initialize UART1 with RX=16, TX=17

    my_sds.begin(SDSG1, SDSG2);

    // Initialize CAN
    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
        Serial.println("CAN Module Initialized Successfully!");
    } else {
        Serial.println("CAN Module Initialization Failed!");
        while (1);
    }
    CAN.setMode(MCP_NORMAL);
}

void loop() {
    // Read SDS011 Data
    int status = my_sds.read(&pm25, &pm10);
    pm25=pm25/10;
    pm10=pm10/10;
    // Read DHT22 Data
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    // Read MQ135 Sensor
    //airQuality = analogRead(MQ135_PIN);
    float Rs = getResistance();
    float airQuality = getPPM();
    

    // Display Data on OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("PM2.5: %.1f ug/m3\nPM10: %.1f ug/m3\nTemp: %.1f C\nHumidity: %.1f%%\nAQ: %.1f\n", pm25, pm10, temperature, humidity, airQuality);
   display.setCursor(0, 40);
    if( pm25 < 30.00 && pm10 < 50.00 && airQuality < 400.00){
      display.printf("Air Quality is Good");
      }
    else{
      display.printf("Air Quality is not Good");
      }

     display.display();
    // Transmit Data via CAN
  int airQualityInt = (int)airQuality;  // Convert float to integer
byte canData[8] = { (byte)pm25, (byte)pm10, (byte)temperature, (byte)humidity, (byte)(airQualityInt / 256), (byte)(airQualityInt % 256), 0, 0 };

    CAN.sendMsgBuf(0x100, 0, 8, canData);
    Serial.println("Data sent");
    delay(2000);  // Update every 2 seconds
}

// Function to read ADC and calculate Rs
float getResistance() {
    int adc_value = analogRead(MQ135_PIN);
    float voltage = adc_value * (3.3 / 4095.0); // Convert ADC to voltage
    if (voltage == 0) return 999999; // Avoid division by zero
    float Rs = ((3.3 * RL_VALUE) / voltage) - RL_VALUE;
    return Rs;
}

// Function to calculate CO2 PPM using the improved formula
float getPPM() {
    float Rs = getResistance();
    float ratio = Rs / R0; // Rs/R0 ratio
    float ppm = 1000 * pow(ratio, -1.5); // Updated formula for CO2
    return ppm;
}