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

  String buf = "Version " + String(GM_VERSION);
  int16_t y = display.height() / 2;

  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(SSD1327_WHITE);
  display.setCursor(getCenterX("GameMan!"), y - 30);
  display.print("GameMan!");

  display.setTextSize(1);
  display.setCursor(getCenterX(buf.c_str()), y - 10);
  display.print(buf);
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

  const char *msg = "Press any button";
  bool blinkOn = false;
  button_event_t press;

  Serial.println("Starting AboutBox task");

  showAboutBox();

  display.setTextSize(1);
  int16_t x = getCenterX(msg);

  while (!xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {

    blinkOn = !blinkOn;
    display.setCursor(x, display.height() - 10);
    display.setTextColor(blinkOn ? SSD1327_WHITE : SSD1327_BLACK);
    display.print(msg);
    display.display();

    delay(500);
  }

  Serial.println("AboutBox task complete, exiting");
}
