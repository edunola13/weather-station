#ifndef ModuleSensors_h
#define ModuleSensors_h

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <sensors/HumTempDHT.h>
#include <sensors/AnalogSensor.h>
#include <sensors/BMPSensor.h>

struct WeatherIntConfig {
  long refreshTime = 1000 * 10;  // 10 Segundos
  long lastRefreshTime = 0;
} weatherIntConfig;

struct WeatherStatus {
  bool status = false;
} weatherStatus;

HumTempDHT humTemp(D6, 11);  // DHT11
RainSensor rainSen(A0);  // Rain Sensor
BMPSensor bmpSen(0);  // BMP180 Sensor

void initSensors() {
  // Nothing to do
}

void updateSensors() {
  if(millis() - weatherIntConfig.lastRefreshTime >= weatherIntConfig.refreshTime || millis() - weatherIntConfig.lastRefreshTime < 0) {
    // HumTempDHT
    humTemp.updateSensor();
    // Rain Sensor
    rainSen.updateSensor();
    // BMP Sensor
    bmpSen.updateSensor();

    // Check WeatherStatus
    weatherStatus.status = !humTemp.getErrorRead();

    if (weatherStatus.status) {
      StaticJsonDocument<255> jsonBuffer;
      char JSONmessageBuffer[255];

      jsonBuffer["humedity"] = humTemp.getHumedad();  // percentage
      jsonBuffer["temperature_dht"] = humTemp.getTemperatura();  // centigrados
      jsonBuffer["rain"] = rainSen.getLevel();  // percentage
      jsonBuffer["altitude"] = bmpSen.getAltitude();  // meters
      jsonBuffer["pressure"] = bmpSen.getPressureHPa();  // hPa
      jsonBuffer["temperature"] = bmpSen.getTemperatura();  // centigrados
      serializeJson(jsonBuffer, JSONmessageBuffer);

      mqttClient.publish("/esp/hum_temp", JSONmessageBuffer);
    }

    // Actualizo el tiempo
    weatherIntConfig.lastRefreshTime = millis();
  }
}

#endif
