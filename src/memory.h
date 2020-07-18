
// CONFIG
void loadConfig() {
  EEPROM.begin(512);
  uint8_t load = 0;
  int pos = 0;
  EEPROM.get(0, load);
  pos = 1;
  if(load == 1) {
    EEPROM.get(pos, config);
    pos += sizeof(struct Config);

    int altitude;
    EEPROM.get(pos, altitude);
    bmpSen.setAltitude(altitude);
    pos += sizeof(altitude);
  } else {
     DEB_DO_PRINTLN(NO_CONF);
  }
  DEB_DO_PRINTLN(OK_CONF);
  EEPROM.end();
}

void saveConfig() {
  int pos= 0;
  EEPROM.begin(512);

  EEPROM.put(pos++, 1);
  EEPROM.put(pos, config);
  pos += sizeof(struct Config);

  EEPROM.put(pos, bmpSen.getAltitude());
  pos += sizeof(bmpSen.getAltitude());

  EEPROM.commit();
  DEB_DO_PRINTLN(OK_SAVE_CONF);
}
