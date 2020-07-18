#include <Arduino.h>
#include <ESP8266WebServer.h>
// #include <ESP8266WebServerSecure.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#define DOMO_DEBUG
#define DOMO_SPEED 9600
//#define USE_WDT
//#define WDT_TIME WDTO_8S
#define DEVICE_TYPE "ESP_ALL"
#define DEVICE_VERSION "1"

#define HTTP_REST_PORT 80
#define WIFI_RETRY_DELAY 500
#define MAX_WIFI_INIT_RETRY 20
#define WIFI_REINTENT_AFTER_SETUP 1000*60*5  // 5 Minutes reintent if ssid is set
#define MQTT_REINTENT_AFTER_SETUP 1000*60*1  // 1 Minutes reintent if ssid is set

#include <common_initial.h>
#include "messages.h"
#include "config.h"
#include "modules/sensors.h"
#include "modules/lights_control.h"
#include "memory.h"
#include "controllers.h"

void setup(void) {
    Serial.begin(9600);
    initialGeneric();
    DEB_DO_PRINTLN(MSG_START);

    // Setear estado inicial
    setStatusLed();
    // Leer config de Epprom
    loadConfig();
    // Inicializo WiFi
    initWifi();
    // Inicializo Sensores
    initSensors();

    // Config rest server routing
    config_rest_server_routing();

    DEB_DO_PRINTLN("HTTP REST Server Started");
}

void loop(void) {
    // Set status leds
    setStatusLed();

    // Check WiFi Connection
    if (status.status != 'C' && String(config.ssid) != "") {
      reconnect_wifi();
    }

    // update sensors
    updateSensors();

    // Handle Requests
    http_rest_server.handleClient();
    // http_rest_server_ssh.handleClient();

    if (!mqttClient.connected()) {
      reconnect();
    }
    mqttClient.loop();
}
