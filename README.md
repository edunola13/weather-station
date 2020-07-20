# Weather Station
Meteorological station with ESP8266.
The station is made up of the following sensors DHT11, BMP180, RainSensor. We can add any other sensor.
The station will allow interaction through API Rest and MQTT. By API Rest we could both read and configure and by MQTT we would publish update events.

## API Rest
/info (GET): Get information about the system
/config (GET): Get config about the station. Name, Network, MQTT, Access Key.
/config (PUT): Update config.
/config (POST): Save config in EEPROM.
/weather (GET): Get meteorological info. Temperature, Humidity, Pressure, Altitude, Rain.
/weather (PUT): Update config of station. Altitude.

## MQTT
Publish every 10 seconds the status of station.


