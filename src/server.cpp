#include "server.hpp"

#include <EEPROM.h>

#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/socket");

int current = 1023;

void s_print(const char *msg, bool newline) {
    if (newline) {
        Serial.println(msg);
    } else {
        Serial.print(msg);
    }
    ws.textAll(msg);
}

void s_print(const char *msg) {
    s_print(msg, false);
}

void s_println(const char *msg) {
    s_print(msg, true);
}

void s_print(String msg) {
    s_print(msg.c_str());
}

void s_println(String msg) {
    s_print(msg.c_str(), true);
}

void s_print(int msg) {
    s_print(String(msg));
}

void s_println(int msg) {
    s_print(String(msg).c_str(), true);
}

void setup_server() {
    pinMode(LED_PIN, OUTPUT);
    analogWriteFreq(50000);
    EEPROM.begin(4);  // one int
    EEPROM.get(EEPROM_BRIGHTNESS_ADDR, current);
    analogWrite(LED_PIN, current);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/content.html", String(), false, processor);
    });

    server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("brightness")) {
            AsyncWebParameter *p = request->getParam("brightness");
            int value = p->value().toInt() * 1023 / 5;
            if (value != current) {
                String vstring = "New brightness: ";
                vstring += value;
                s_println(vstring);
                analogWrite(LED_PIN, value);
                EEPROM.put(EEPROM_BRIGHTNESS_ADDR, value);
                EEPROM.commit();
                current = value;
            }
        }
        request->send(200, "Success");
    });

    server.onNotFound(on_404);

    ws.onEvent(socket_event);
    server.addHandler(&ws);
    ws.enable(true);

    server.begin();
}

String processor(const String &var) {
    if (var == "CURRENT_VALUE")
        return String(current * 5 / 1023);
    return String();
}

void socket_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        //client connected
        os_printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Connected %u", client->id());
        client->ping();
    } else if (type == WS_EVT_DISCONNECT) {
        //client disconnected
        os_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        //error was received from the other end
        os_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
    } else if (type == WS_EVT_PONG) {
        //pong message was received (in response to a ping request maybe)
        os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
    }
}

// Handle Unknown Request
void on_404(AsyncWebServerRequest *request) {
    request->send(404);
}