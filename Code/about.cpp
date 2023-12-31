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

const String AboutBox::appName = "About";

void AboutBox::setup(bool rsvp) {
  // Nothing to do here for now
  dprintln("AboutBox setup called");
}

void AboutBox::showAboutBox() {

  const char *msg = "Press any button";
  bool blinkOn = false;
  button_event_t press;

  String buf = "Version " + String(GM_VERSION);
  int16_t y = display.height() / 2;

  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(getCenterX("GameMan!"), y - 30);
  display.print("GameMan!");

  display.setTextSize(1);
  display.setCursor(getCenterX(buf.c_str()), y - 10);
  display.print(buf);
  display.display();
  
  display.setTextSize(1);
  int16_t x = getCenterX(msg);

  for (;;) {

    // Use the button timeout of 500ms as the blink rate :-)
    if (xQueueReceive(buttonEvents, &(press), (TickType_t)500)) {
      if (press.action == btnReleased) return;
    }

    blinkOn = !blinkOn;
    display.setCursor(x, display.height() - 10);
    display.setTextColor(blinkOn ? WHITE : BLACK);
    display.print(msg);
    display.display();
  }
}

// rollCredits()
// animate a little scroller and blink the continue message
/*
    Static const String PROGMEM credits[] = {
      " ",
      "Portland State",
      "University",
      "ECE411",
      "Fall, 2023",
      " ",
      "Team 14",
      "~~~~~~~",
      "R. Elliot Lamb",
      "Joe x",
      "A",
      "B",
      "C"
      "~~~~~~~",
      " "
    }
*/

/*
 * About Box main loop
 *
 * Puts the program version and credits on the screen.  Basic
 * test of the menu/app interaction.  Waits for any button press
 * to return to the menu.
 */
void AboutBox::run() {

  dprintln("AboutBox: Task starting");

  showAboutBox();

  dprintln("AboutBox: Task complete");
}
