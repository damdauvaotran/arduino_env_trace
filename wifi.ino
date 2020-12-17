#include <Arduino.h>
#include <string>

#include <Wire.h>
#include <Adafruit_MLX90614.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define USE_SERIAL Serial
#define DHTTYPE    DHT11




const char* SSID = "gg yasou";
const char* PASSWORD = "asdfghjkl";
const char* SOCKET_IP = "192.168.43.162";
const int SOCKET_PORT = 8000;
const int DELAY_TIME = 1000;


ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


uint32_t delayMS;
unsigned long prevtime = 0;

void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}

void update(const char * payload, size_t length) {
  USE_SERIAL.printf("updated: %s\n", payload);
}

void setup() {
  USE_SERIAL.begin(115200);

  USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(SSID, PASSWORD);

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }
  webSocket.on("event", event);
  webSocket.on("update", update);
  webSocket.begin(SOCKET_IP, SOCKET_PORT);
  prevtime = millis();

  // Initial sensor
  mlx.begin();
}

void loop() {
  webSocket.loop();
  if (millis() - prevtime > DELAY_TIME) {

    prevtime = millis();

    // Print sensor info

    float objectTemp = mlx.readObjectTempC();
    float ambientTemp = mlx.readAmbientTempC();
    Serial.print("Ambient = "); Serial.print(ambientTemp);
    Serial.print("*C\tObject = "); Serial.print(objectTemp); Serial.println("*C");
    Serial.println();
    char buff[128];
    sprintf(buff, "{\"objectTemp\":%f,\"ambientTemp\":%f}", objectTemp, ambientTemp);
    webSocket.emit("message", buff);

  }
}
