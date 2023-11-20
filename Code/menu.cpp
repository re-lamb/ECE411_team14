/*
 *  menu.cpp - Main menu and program launcher
 *
 *  Abstract:
 *      Runs the main task, which allows selection of a game/program
 *      to run from a simple graphical menu.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Fonts/FreeSans9pt7b.h>

#include "GameMan.h"
#include "graphics.h"
#include "button.h"
#include "about.h"


MenuTask::MenuTask()
  : GMTask("MENU") {
}

/*
 * Set up the menu data structures.  Called ONCE at startup.
 */
void MenuTask::setup() {

  Serial.println("Menu task initializing");

  // Build the array in the order to be displayed
  strncpy(items[0].progName, "About...", MENU_MAX_CHARS);
  items[0].icon = about16_bmp;
  items[0].prog = new AboutBox;

  // Fake ones for testing
  strncpy(items[1].progName, "Sys Info", MENU_MAX_CHARS);
  items[1].icon = info16_bmp;
  items[1].prog = NULL;

  strncpy(items[2].progName, "Battleship", MENU_MAX_CHARS);
  items[2].icon = NULL;
  items[2].prog = NULL;

  strncpy(items[3].progName, "Tic-Tac-Toe", MENU_MAX_CHARS);
  items[3].icon = NULL;
  items[3].prog = NULL;

  strncpy(items[4].progName, "MazeWar!", MENU_MAX_CHARS);
  items[4].icon = about16_bmp;
  items[4].prog = NULL;

  Serial.println("Menu init complete");
}

/*
 * Clear the screen and paint the menu.
 */
void MenuTask::redrawMenu() {
  int16_t x1, y1;
  uint16_t w, h;

  Serial.println("Entering redraw");
  /*
   *  Format of the main menu:
   *    +----------------+    3 pixel border
   *    |    ~ Menu ~    |    fontHeight
   *    +----------------+    3 pixel border
   *    | [ ] Item 1     |    [] = small 16x16 icon (optional)
   *    |      ...       |
   *    | [ ] Item n     |    // up to 6 lines without scrolling?
   *    +----------------+
   *
   *  Each menu selection is rendered in a box at least 18 pixels high
   *  to accommodate the icon and a thin border, and 104 pixels wide.
   *
   *  TODO: implement scrolling.
   */

  display.clearDisplay();
  display.fillRect(0, 0, display.width(), display.height(), HALF_BRIGHT);
  display.drawRect(1, 1, display.width() - 2, display.height() - 2, SSD1327_BLACK);  // fixme

  // Set a nice menu font, then compute its height, center the header
  display.setFont(&FreeSans9pt7b);
  display.setTextColor(SSD1327_WHITE);  // fixme
  display.getTextBounds(MENU_HEADER, 0, 0, &x1, &y1, &w, &h);

  // Save/compute based on the font loaded
  fontHeight = h;
  boxHeight = fontHeight < MENU_ICON_SZ ? MENU_ICON_SZ + 2 : fontHeight + 2;

  // Center and print the header
  display.setCursor((display.width() - w) / 2, fontHeight + MENU_BORDER);
  display.println(MENU_HEADER);
  display.drawFastHLine(2, fontHeight + MENU_Y_OFFSET, display.width() - 3, SSD1327_BLACK);

  // Skip header + border; leave room above/below for highlight
  y1 = MENU_Y_OFFSET + fontHeight + boxHeight;

  for (int i = 0; i < MENU_MAX_ITEMS; i++) {

    // Save computed Y position to save effort later
    yPos[i] = y1 - 1;

    // Off the bottom?
    if (y1 > display.height() - MENU_BORDER) break;

    // Draw the mini icon
    if (items[i].icon != NULL)
      display.drawBitmap(MENU_BORDER, y1 - MENU_ICON_SZ, items[i].icon, MENU_ICON_SZ, MENU_ICON_SZ, SSD1327_WHITE);

    // Finally, draw the text
    if (items[i].progName != NULL) {
      display.setCursor(MENU_X_OFFSET, yPos[i]);
      display.print(items[i].progName);
    }

    y1 += boxHeight;
  }

  // Highlight the first item
  selected = 0;

  // Paint it!
  display.display();
}

/*
 * Highlight (or un-highlight) the current selection.
 */
void MenuTask::showSelected(bool on) {

  Serial.printf("showSelected called for menu item %d\n", selected);

  if (selected < 0 || selected >= MENU_MAX_ITEMS) {
    Serial.println("HOW?  WTF?");
    return;
  }

  if (on) {
    display.fillRect(MENU_X_OFFSET - 1, yPos[selected] - boxHeight + 2,
                     display.width() - MENU_X_OFFSET - MENU_BORDER, boxHeight, SSD1327_BLACK);
  } else {
    display.fillRect(MENU_X_OFFSET - 1, yPos[selected] - boxHeight + 2,
                     display.width() - MENU_X_OFFSET - MENU_BORDER, boxHeight, HALF_BRIGHT);
  }

  // Redraw the string
  display.setCursor(MENU_X_OFFSET, yPos[selected]);
  display.print(items[selected].progName);
  display.display();
}

/*
 *  MenuTask main loop
 *
 *  This is the main program for GameMan.  It's a task that runs the menu and
 *  starts up and closes down apps/games as they are selected by the user.  It
 *  never exits; when an app is started up, it basically sleeps in the background
 *  waiting for the app to signal completion, then it cleans up, resets the screen
 *  and allows another program to be chosen.
 */
void MenuTask::run() {

  bool appRunning = false;
  button_event_t press;
  TaskHandle_t app;

  Serial.printf("Menu task starting up on core %d\n", xPortGetCoreID());
  delay(10);

  redrawMenu();
  showSelected(true);

  for (;;) {

    if (!appRunning) {

      // We're in the foreground, so grab button events
      if (xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {

        // For now we're only interested in release (button up) events
        if (press.action == btnReleased) {

          // Got one! De-highlight the current selection...
          showSelected(false);

          switch (press.id) {
            case BTN_UP:
            case BTN_A:
              selected += (selected > 0) ? -1 : 0;
              break;

            case BTN_DN:
            case BTN_B:
              selected += (selected < MENU_NUM_ITEMS) ? 1 : 0;
              break;

            case BTN_C:
              Serial.printf("Selected item %d!\n", selected);
              if (items[selected].prog != NULL)
                appRunning = true;
              break;
          }

          // Re-draw it even if it didn't change
          showSelected(true);
        }
      }

      // Wait a while.  Humans aren't that fast, maybe 20-50ms typical?
      delay(50);

    } else {

      // User selected a program to run!
      Serial.printf("About to launch %s\n", items[selected].progName);

      // todo: some kind of animation to fade the menu or blink the selection
      // a couple of times to start the transition?
      GMTask *app = items[selected].prog;

      // Call the setup() method in case the program needs to do any
      app->setup();

      // Launch the program in its own task and save the handle
      app->start();

      // Now loop in the background until it completes
      // (Here we could listen for a special keypress to abort/return if stuck/etc?
      // Like if they hold C down for 3 seconds or something)
      while (appRunning) {
        delay(250);                       // reduce frequency to save battery :-)
        appRunning = !app->isComplete();  // there are better ways to do this
      }

      // We have control again, so repaint the screen
      redrawMenu();
      showSelected(true);

      // Clear the queue of residual events and start polling the buttons again
      xQueueReset(buttonEvents);
    }
  }

  Serial.println("Menu task exiting!");
}
