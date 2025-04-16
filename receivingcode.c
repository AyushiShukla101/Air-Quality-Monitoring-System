#include <SPI.h>
#include <mcp_can.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SPI_CS_PIN 5
MCP_CAN CAN(SPI_CS_PIN);

// Wi-Fi Credentials
const char* ssid = "Gundeti";
const char* password = "Reyansh@26";

// ThingSpeak API Key
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "V2U3S1E00TEBB8TQ";

void setup() {
    Serial.begin(115200);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nWi-Fi Connected");




    // Initialize CAN Module
    if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
        Serial.println("CAN Module Initialized Successfully!");
    } else {
        Serial.println("CAN Module Initialization Failed!");
        while (1);
    }
    CAN.setMode(MCP_NORMAL);
}

void loop() {
    long unsigned int rxId;
    unsigned char len = 0;
    unsigned char rxBuf[8];

    if (CAN.checkReceive() == CAN_MSGAVAIL) {
        CAN.readMsgBuf(&rxId, &len, rxBuf);

        float pm25 = rxBuf[0];
        float pm10 = rxBuf[1];
        float temperature = rxBuf[2];
        float humidity = rxBuf[3];
        float airQuality = (rxBuf[4] << 8) | rxBuf[5];

        Serial.printf("Received: PM2.5=%.1f, PM10=%.1f, Temp=%.1f, Humidity=%.1f, AQ=%.1f\n", pm25, pm10, temperature, humidity, airQuality);

        // Send Data to ThingSpeak
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            String url = String(server) + "?api_key=" + apiKey + "&field1=" + pm25 + "&field2=" + pm10 + "&field3=" + temperature + "&field4=" + humidity + "&field5=" + airQuality;
            http.begin(url);
            int httpCode = http.GET();
            http.end();
        }
    }
}