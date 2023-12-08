/*
 *  sysinfo.cpp - App to show useful debugging/stats info
 *
 *  Abstract:
 *      Defines a simple task that shows battery status,
 *      active network info, or other useful debugging or
 *      status info about the console.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Arduino.h>

#include "GameMan.h"
#include "hardware.h"
#include "graphics.h"
#include "sysinfo.h"

SysInfo::SysInfo()
  : GMTask("SYSINFO") {
}

const String SysInfo::appName = "SysInfo";

void SysInfo::setup(bool rsvp) {
  // Nothing to do here for now
  dprintln("SysInfo setup called");
}

/*
 *  Compute approximately how much battery life is available.
 *  Prints to the console for debugging; returns the pct from 0..100.
 */
uint8_t SysInfo::getBatteryAvail() {
  uint8_t pct = 100;
  float voltage = analogRead(VBAT_SENSE) / 4096.0 * 7.23;  // ESP32-WROOM-32E with voltage divider

  Serial.println("Battery sense: " + String(voltage));

  // Polynomial courtesy of github.com/G6EJD/LiPo_Battery_Capacity
  pct = 2808.3808 * pow(voltage, 4) - 43560.9157 * pow(voltage, 3) + 252848.5888 * pow(voltage, 2) - 650767.4615 * voltage + 626532.5703;

  if (voltage > 4.19) pct = 100;
  else if (voltage <= 3.50) pct = 0;

  Serial.println("Battery avail: " + String(pct) + "%");
  return pct;
}

/*
 *  For now, a static info display.  Should be extended to have
 *  more info AND allow setting the player tag (in nvram)
 */
void SysInfo::showSystemInfo() {

  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(getCenterX("Sys Info"), 10);
  display.println("Sys Info");
  display.println();

  display.setTextSize(1);
  display.println("Hardware version:");
  display.println("  " + String(HW_VERSION));
  display.println("MAC address:");
  display.println("  " + netTask.getNodeAddr());
  display.println("Battery left:");
  display.println("  " + String(getBatteryAvail()) + "%");
  display.display();

  // A lot more to do here:
  //  allow customizing the player name (and save it to nvs)
  //  show active network players and what they're doing
  //  show available memory!  and task info, if available
  //  show uptime!
}

/*
 * Sys Info main loop
 *
 * Puts some basic stats on the screen.  Waits for any button press
 * to return to the menu.
 *
 * TODO: this is probably where the user should set their name, any
 * other prefs like brightness, sleep time, etc :-)
 */
void SysInfo::run() {
  button_event_t press;

  Serial.println("SysInfo: Task starting");

  showSystemInfo();

  while (!xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {
    delay(500);
  }

  Serial.println("SysInfo: Task complete");
}
