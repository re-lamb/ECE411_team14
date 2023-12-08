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
 *  Clear screen and print common header
 */
void SysInfo::showHeader() {
  
  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(getCenterX("Sys Info"), 8);
  display.println("Sys Info");
  display.println();
  display.setTextSize(1);
}

/*
 *  For now, a static info display.  Should be extended to have
 *  more info AND allow setting the player tag (in nvram)
 */
int SysInfo::showHWInfo() {
  button_event_t press;
  uint16_t upX, upY;
  uint16_t pctX, pctY;
  TickType_t lastBatt = xTaskGetTickCount();

  showHeader();
  display.println("Address:");
  display.println("  " + netTask.getNodeAddr());
  display.println();
  display.println("Hardware: version " + String(HW_VERSION));
  display.println();
  display.print("Battery:  ");
  pctX = display.getCursorX();
  pctY = display.getCursorY();
  display.println(String(getBatteryAvail()) + "%");
  display.println();
  display.print("Uptime:   ");
  upX = display.getCursorX();
  upY = display.getCursorY();
  display.print(uptime());
  display.setCursor(0, display.height() - 10);
  display.print("                -->");
  display.display();

  for (;;) {

    // Update the uptime while waiting for a button press to exit
    if (xQueueReceive(buttonEvents, &(press), (TickType_t)1000)) {
      if (press.action == btnReleased) {
        switch (press.id) {
          case BTN_RT:  return 2;   // next page
          case BTN_LT:  break;      // ignore
          default:      return 0;   // exit
        }
      }
    }

    display.setCursor(upX, upY);
    display.fillRect(upX, upY, display.width() - upX, 10, BLACK);
    display.print(uptime());
    display.display();

    // Update the battery charge less frequently
    if (elapsed(60000, lastBatt)) {
      display.setCursor(pctX, pctY);
      display.fillRect(pctX, pctY, display.width() - pctX, 10, BLACK);
      display.print(String(getBatteryAvail()) + "%");
      display.display();
      lastBatt = xTaskGetTickCount();
    }
  }
}

int SysInfo::showPlayerInfo() {
  button_event_t press;

  showHeader();
  display.println("Player info/edit");
  display.println("goes here (TBD)");
  display.setCursor(0, display.height() - 10);
  display.print("<--             -->");
  display.display();

  for (;;) {

    // Update the uptime while waiting for a button press to exit
    if (xQueueReceive(buttonEvents, &(press), (TickType_t)1000)) {
      if (press.action == btnReleased) {
        switch (press.id) {
          case BTN_RT:  return 3;   // next page
          case BTN_LT:  return 1;   // prev page
          default:      return 0;   // exit
        }
      }
    }
  }
}


int SysInfo::showNetInfo() {
  button_event_t press;

  showHeader();
  display.println("Network info");
  display.println("goes here (TBD)");
  display.setCursor(0, display.height() - 10);
  display.print("<--");
  display.display();

  for (;;) {

    // Update the uptime while waiting for a button press to exit
    if (xQueueReceive(buttonEvents, &(press), (TickType_t)1000)) {
      if (press.action == btnReleased) {
        switch (press.id) {
          case BTN_RT:  break;      // no next page
          case BTN_LT:  return 2;   // prev page
          default:      return 0;   // exit
        }
      }
    }
  }
}

void SysInfo::showSystemInfo() {
  int page = 1;

  while (page > 0) {
    if (page == 1)      page = showHWInfo();
    else if (page == 2) page = showPlayerInfo();
    else if (page == 3) page = showNetInfo();
  } 
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

  dprintln("SysInfo: Task starting");

  showSystemInfo();

  dprintln("SysInfo: Task complete");
}
