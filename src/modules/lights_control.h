#ifndef ModuleLights_h
#define ModuleLights_h

#include <actuators/DigitalControl.h>

DigitalControl warningLed(D8);
DigitalControl successLed(D7, 2);

void startWarning() {
  warningLed.set(HIGH);
  successLed.set(LOW);
}

void startSuccess() {
  warningLed.set(LOW);
  successLed.set(100);
}

void setStatusLed() {
  if (status.status == 'I') {
    return startWarning();
  }
  if (status.status == 'C' && WiFi.status() != WL_CONNECTED) {
    return startWarning();
  }
  startSuccess();
}

#endif
