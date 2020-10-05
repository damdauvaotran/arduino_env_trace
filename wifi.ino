 #include <Arduino.h>
#include <string>  

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <SocketIoClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define USE_SERIAL Serial 
#define DHTTYPE    DHT11 




const char* SSID = "tengicungduoc";
const char* PASSWORD = "462667652253";
const char* SOCKET_IP = "192.168.1.104";
const int SOCKET_PORT = 8000;
const int DELAY_TIME = 2000;
const int DHTPIN  = 4; 


ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


DHT_Unified dht(DHTPIN, DHTTYPE);


uint32_t delayMS;
unsigned long prevtime = 0;

void event(const char * payload, size_t length) {
  USE_SERIAL.printf("got message: %s\n", payload);
}

void setup() {
    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP(SSID, PASSWORD);

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }
    webSocket.on("event", event);
    webSocket.begin(SOCKET_IP, SOCKET_PORT);
    prevtime = millis();

    // Initial sensor
    dht.begin();
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    dht.temperature().getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.print  (F("Min delay:  ")); Serial.print(sensor.min_delay);
    Serial.println(F("------------------------------------"));
}

void loop() {
    webSocket.loop();
    if (millis() - prevtime > DELAY_TIME){
        
        prevtime = millis();

        // Print sensor info
        sensors_event_t temp_event;
        sensors_event_t humid_event;
        dht.temperature().getEvent(&temp_event);
        dht.humidity().getEvent(&humid_event);
        if (isnan(temp_event.temperature) || isnan(humid_event.relative_humidity)) {
          Serial.println(F("Error reading temperature and humidity!"));
        }
        else {
          char buff[128];
          sprintf(buff, "{\"temp\":%f,\"humid\":%f}", temp_event.temperature, humid_event.relative_humidity);
          webSocket.emit("message", buff);
          Serial.print(F("Temperature: "));
          Serial.print(temp_event.temperature);
          Serial.print(F("And: "));
          Serial.print(humid_event.relative_humidity);
          Serial.println(F("°C"));
        }
     
    }

    
    
    
}
