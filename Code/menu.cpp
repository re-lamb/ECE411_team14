/*
 *  menu.cpp - Main menu and program launcher
 *
 *  Abstract:
 *      Runs the main task, which allows selection of a game/program
 *      to run from a simple graphical menu.  It also listens for
 *      connection requests from other players and can launch a game
 *      in response.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Fonts/FreeSans9pt7b.h>

#include "GameMan.h"
#include "graphics.h"
#include "button.h"
#include "network.h"

// For each app on the menu
#include "about.h"
#include "sysinfo.h"
#include "tictactoe.h"

MenuTask::MenuTask()
  : GMTask("MENU") {
}

/*
 * Set up the menu data structures.  Called ONCE at startup.
 */
void MenuTask::setup(bool rsvp) {

  Serial.println("menu: Task initializing");

  strcpy(currentApp, "MENU");

  // Build the array in the order to be displayed
  items[0].prog = new AboutBox;
  items[0].icon = about16_bmp;
  strncpy(items[0].progName, AboutBox::appName.c_str(), MENU_MAX_CHARS);

  items[1].prog = new SysInfo;
  items[1].icon = info16_bmp;
  strncpy(items[1].progName, SysInfo::appName.c_str(), MENU_MAX_CHARS);

  items[2].prog = new TicTacToe;
  items[2].icon = ttt16_bmp;
  strncpy(items[2].progName, TicTacToe::appName.c_str(), MENU_MAX_CHARS);

  // TBD
  items[3].prog = NULL;
  items[3].icon = bship16_bmp;
  strncpy(items[3].progName, "Battleship", MENU_MAX_CHARS);

  items[4].prog = NULL;
  items[4].icon = about16_bmp;
  strncpy(items[4].progName, "MazeWar!", MENU_MAX_CHARS);
}

/*
 *  Wait for the network to initialize, then set up our packet
 *  queue to receive RSVP requests from other GM nodes.
 */
bool MenuTask::startNetwork() {

  dprint("menu: Setting up RSVP queue");

  // At system startup, task launch times might not be deterministic
  // so wait for the network to report itself open for business
  while (!netTask.isRunning()) {
    delay(100);
    dprint(".");
  }
  dprintln();

  // Ok!  Set up our packet queue
  rsvpId = netTask.createQueue(4);

  // Add a filter for our packet type
  if (netTask.addFilter(rsvpId, GM_RSVP) < 0) {
    // If queue or filter add failed, no network; bail out
    Serial.println("menu: Could not initialize the network!");
    return false;
  }

  // Now grab our actual handle for reading packets!
  rsvpQ = netTask.getHandle(rsvpId);
  if (rsvpQ == 0) {
    Serial.println("menu: Invalid RSVP queue handle!");
    return false;
  }

  return true;
}

/*
 * Clear the screen and paint the menu.
 */
void MenuTask::redrawMenu() {
  int16_t x1, y1;
  uint16_t w, h;

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
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.fillRect(0, 0, display.width(), display.height(), HALF_BRIGHT);
  display.drawRect(1, 1, display.width() - 2, display.height() - 2, BLACK);  // fixme

  // Set a nice menu font, then compute its height, center the header
  display.setFont(&FreeSans9pt7b);
  display.getTextBounds(MENU_HEADER, 0, 0, &x1, &y1, &w, &h);

  // Save/compute based on the font loaded
  fontHeight = h;
  boxHeight = fontHeight < MENU_ICON_SZ ? MENU_ICON_SZ + 2 : fontHeight + 2;

  // Center and print the header
  display.setCursor((display.width() - w) / 2, fontHeight + MENU_BORDER);
  display.println(MENU_HEADER);
  display.drawFastHLine(2, fontHeight + MENU_Y_OFFSET, display.width() - 3, BLACK);

  // Skip header + border; leave room above/below for highlight
  y1 = MENU_Y_OFFSET + fontHeight + boxHeight;

  for (int i = 0; i < MENU_MAX_ITEMS; i++) {

    // Save computed Y position to save effort later
    yPos[i] = y1 - 1;

    // Off the bottom?
    if (y1 > display.height() - MENU_BORDER) break;

    // Draw the mini icon
    if (items[i].icon != NULL)
      display.drawBitmap(MENU_BORDER, y1 - MENU_ICON_SZ, items[i].icon, MENU_ICON_SZ, MENU_ICON_SZ, WHITE);

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

  if (selected < 0 || selected >= MENU_MAX_ITEMS) {
    dprintf("menu: selection %d out of range!\n", selected);
    return;
  }

  if (on) {
    display.fillRect(MENU_X_OFFSET - 1, yPos[selected] - boxHeight + 2,
                     display.width() - MENU_X_OFFSET - MENU_BORDER, boxHeight, BLACK);
  } else {
    display.fillRect(MENU_X_OFFSET - 1, yPos[selected] - boxHeight + 2,
                     display.width() - MENU_X_OFFSET - MENU_BORDER, boxHeight, HALF_BRIGHT);
  }

  // Redraw the string
  display.setCursor(MENU_X_OFFSET, yPos[selected]);
  display.print(items[selected].progName);
  display.display();
}

char *MenuTask::getCurrentApp() {
  return currentApp;
}

/*
 *  MenuTask main loop
 *
 *  This is the main program for GameMan.  It's a task that runs the menu and
 *  starts up and closes down apps/games as they are selected by the user.  It
 *  never exits; when an app is started up, it basically sleeps in the background
 *  waiting for the app to signal completion, then it cleans up, resets the screen
 *  and allows another program to be chosen.
 *
 *  RSVP requests from other nodes when the menu is active trigger a dialog that
 *  lets the user accept or reject the connection.  If accepted, the menu invokes
 *  the requested program with the "rsvp" flag set so the app can start up in
 *  guest mode and synchronize with the requester.  RSVPs when another app is
 *  running are ignored/rejected (for now).
 */
void MenuTask::run() {

  bool appRunning = false;
  bool guest = false;
  button_event_t press;
  TaskHandle_t app;

  Serial.printf("menu: Task starting up on core %d\n", xPortGetCoreID());
  delay(10);

  if (!startNetwork()) {
    Serial.println("menu: RSVP service not available");
  }

  redrawMenu();
  showSelected(true);

  for (;;) {

    if (!appRunning) {

      // Humans are slow, so tune this here for ~ 25-50ms between polls
      delay(50);

      // We're in the foreground, so grab button events
      if (xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {

        if (press.action == btnReleased || press.action == btnRepeat) {

          // Got one! De-highlight the current selection...
          showSelected(false);

          switch (press.id) {
            case BTN_UP:
              selected += (selected > 0) ? -1 : 0;
              break;

            case BTN_DN:
              selected += (selected < MENU_NUM_ITEMS) ? 1 : 0;
              break;

            case BTN_B:
              // button B is "yes"/doit -- ignore repeats
              if (press.action != btnRepeat) {
                dprintf("Selected item %d!\n", selected);
                if (items[selected].prog != NULL)
                  appRunning = true;
              }
              break;
          }

          // Re-draw it even if it didn't change
          showSelected(true);
        }
      }

      // Check the network queue for RSVP events
      if (rsvpQ) {

        gm_packet_t pkt;
        iff_packet_t rsvp;

        if (xQueueReceive(rsvpQ, &(pkt), (TickType_t)0)) {
          Serial.println("menu: Received RSVP packet");

          // Sanity check...
          if (pkt.pktType != GM_RSVP || pkt.length != sizeof(iff_packet_t)) {
            dprintf("BAD type (%d) or payload length (%d)\n", pkt.pktType, pkt.length);
          } else {
            // Copy it out
            memcpy(&rsvp, pkt.payload, pkt.length);
            dprintf("Invite from %s to play %s\n", rsvp.who, rsvp.what);
            // run the dialog
            // for now, just say YES; update the payload and ship it back
            // assume we have the same programs but a findProg(rsvp.what) to make sure
            //
            //netTask.sendTo();
          }
        }
      }

    } else {

      // User selected a program to run!
      dprintf("menu: Launching %s\n", items[selected].progName);

      // todo: some kind of animation to fade the menu or blink the selection
      // a couple of times to start the transition?
      GMTask *app = items[selected].prog;
      strncpy(currentApp, items[selected].progName, MENU_MAX_CHARS);

      // Call the setup() method in case the program needs to do any
      app->setup(guest);

      // Launch the program in its own task and save the handle
      app->start();

      // Now loop in the background until it completes
      while (appRunning) {

        // Toss any RSVP packets that arrive, since we're busy...
        if (rsvpQ) {
          gm_packet_t pkt;

          if (xQueueReceive(rsvpQ, &(pkt), (TickType_t)0)) {
            dprintln("menu: Ignored RSVP (busy)");
          }
        }
        
        delay(250);                       // reduce frequency to save battery :-)
        appRunning = app->isRunning();    // there are better ways to do this
      }

      // Properly close down the (suspended) task
      app->end();

      // We have control again, so reset and repaint the screen
      strcpy(currentApp, "MENU");
      guest = false;
      redrawMenu();
      showSelected(true);

      // Clear the queue of residual events and start polling the buttons again
      xQueueReset(buttonEvents);
    }
  }
}
