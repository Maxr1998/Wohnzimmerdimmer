#include <ArduinoOTA.h>

#include "ESPAsyncWebServer.h"
#include "FS.h"

#include "main.hpp"
#include "server.hpp"

void setup() {
    pinMode(TX_PIN, OUTPUT);
    digitalWrite(TX_PIN, HIGH); // Turn off the on-board LED

    Serial.begin(115200);
    setup_wifi();
    connect_wifi();
    setup_ota();

    SPIFFS.begin();
    setup_server();
}

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.hostname("Wohnzimmerdimmer");
    Serial.println();
}

void connect_wifi() {
    Serial.print("Connecting to ");
    Serial.println(SSID);
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
        digitalWrite(TX_PIN, HIGH);
        delay(100);
        digitalWrite(TX_PIN, LOW);
    }
    Serial.println("\r#### CONNECTED ####          ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup_ota() {
    ArduinoOTA.setHostname("Wohnzimmerdimmer");
    ArduinoOTA.setPassword(OTA_PWD);
    ArduinoOTA.setPort(8266);
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        String pstring = "Progress: ";
        pstring += (progress / (total / 100));
        pstring += "%\r";
        s_print(pstring);
    });
    ArduinoOTA.onError([](ota_error_t error) {
        s_print("Error[" + String(error) + "]: ");
        if (error == OTA_AUTH_ERROR)         s_println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)   s_println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) s_println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) s_println("Receive Failed");
        else if (error == OTA_END_ERROR)     s_println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("OTA updater initialized");
}

void loop() {
    ArduinoOTA.handle();
}