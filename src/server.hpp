#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#define LED_PIN 4

void s_print(const char *msg, bool newline);
void s_print(const char *msg);
void s_println(const char *msg);
void s_print(String msg);
void s_println(String msg);
void s_print(int msg);
void s_println(int msg);

void setup_server();

String processor(const String& var);
void socket_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void on_404(AsyncWebServerRequest *request);
#endif