/*
 *  about.cpp - Display the About Box
 *
 *  Abstract:
 *      Show credits/version info about the console.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include "GameMan.h"
#include "graphics.h"
#include "about.h"

AboutBox::AboutBox()
  : GMTask("ABOUT") {
}

void AboutBox::setup() {
  // Nothing to do here for now
  Serial.println("AboutBox setup called");
}

void AboutBox::showAboutBox() {
  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(SSD1327_WHITE);
  display.setCursor(0, 40);
  display.println("GameMan!");

  display.setTextSize(1);
  display.println("Version " + String(GM_VERSION));
  display.display();
}

/*
 * About Box main loop
 *
 * Puts the program version and credits on the screen.  Basic
 * test of the menu/app interaction.  Waits for any button press
 * to return to the menu.
 */
void AboutBox::run() {

  bool blinkOn = false;
  button_event_t press;

  Serial.println("Starting AboutBox task");

  showAboutBox();

  while (!xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {

    blinkOn = !blinkOn;
    display.setCursor(0, display.height() - 10);
    display.setTextColor(blinkOn ? SSD1327_WHITE : SSD1327_BLACK);
    display.print("Press any button");
    display.display();

    delay(500);
  }

  Serial.println("AboutBox task complete, exiting");
}
