
#include <Arduino.h>
#include "hardware.h"

// for now, debug print on serial console...
void showBattery() {
  uint8_t pct = 100;
  float voltage = analogRead(VBAT_SENSE) / 4096.0 * 7.23;  // ESP32-WROOM-32E with voltage divider

  Serial.println("Battery sense: " + String(voltage));

  // Polynomial courtesy of github.com/G6EJD/LiPo_Battery_Capacity
  pct = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;

  if (voltage > 4.19) pct = 100;
  else if (voltage <= 3.50) pct = 0;

  Serial.println("Battery avail: " + String(pct) + "%");
}

