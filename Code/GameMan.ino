/*
 *  GameMan.ino - Top level framework
 *
 *  Abstract:
 *      Initializes the ESP32-WROOM-32E-based hardware for the
 *      retro gaming console.  This includes initializing the
 *      network hardware, button state, and display modules.
 *      It then enters the menu loop and launches games (or any
 *      other menu utility written using the simple framework
 *      provided), returning to the menu when the game exits.
 *
 *      See the attached README.md for an overview of the whole
 *      project and links to other documentation.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Arduino.h>
#include <Adafruit_SSD1327.h>

#include "hardware.h"
#include "graphics.h"
#include "task.h"
#include "button.h"
#include "menu.h"
#include "network.h"
#include "about.h"

/*
 * Globals
 */

// Display (connected through software SPI)
Adafruit_SSD1327 display(128, 128, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Create the main menu / button task
MenuTask menuTask;
ButtonTask buttonTask;
NetworkTask netTask;

QueueHandle_t buttonEvents;

// debug
TickType_t last = 0;

void showTasks() {

  // Courtesy of Espressif: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html#misc
  //char writeBuf[1280];      // estimated 32 tasks * 40 bytes
  //vTaskList(writeBuf);      // not available in standard arduino esp-idf build?
  //Serial.println(writeBuf); // so it doesn't work. :-(

  Serial.printf("System: %d tasks  Stack: %d  Heap: %lu\n",
                uxTaskGetNumberOfTasks(),
                uxTaskGetStackHighWaterMark(NULL),
                (unsigned long)ESP.getFreeHeap());
}

/*
 *  Utility routine that probably ought to be in a graphics library.
 *  Return the X coordinate to display a string centered on the screen
 *  using the current font and size.
 */
int16_t getCenterX(const char *s) {
  int16_t x, y;
  uint16_t w, h;

  display.getTextBounds(s, 0, 0, &x, &y, &w, &h);

  return ((display.width() - w) / 2);
}

/*
 *  Show a silly slash screen once at startup.
 */
void showSplash() {
  /*
    Draw from dark to light, increasing the size from 32->64->128,
    hopefully looking like an animation of the icon rushing toward
    the screen.  Was mostly a test of the graphics library so it
    could be made sexier or simpler (if memory gets tight).
  */
  const uint8_t *bitmaps[] = { maze32_bmp, maze64_bmp, maze128_bmp };
  const uint8_t *logo;
  uint8_t x, y;
  uint8_t cur_sz, cur_color;

  const int dly = 50;
  const int steps = 48;  // 3 icons, 16 steps each?

  // init to force initial update
  cur_sz = 16;
  cur_color = SSD1327_WHITE + 1;

  for (uint8_t i = 0; i < steps; i++) {

    if (cur_color > SSD1327_WHITE) {
      cur_sz *= 2;
      cur_color = SSD1327_BLACK;
      logo = bitmaps[i / 16];

      display.clearDisplay();
    }

    // the 1.5" oled is a square... assume 0..127 addressible?
    x = (display.width() - cur_sz) / 2;
    y = x;

    display.drawBitmap(x, y, logo, cur_sz, cur_sz, cur_color++);
    display.display();
    delay(dly);
  }

  display.setTextSize(1);
  display.setTextColor(DARK_GRAY);
  display.setCursor(getCenterX("[Initializing]"), display.height() - 10);
  display.print("[Initializing]");
  display.display();
  delay(1000);
}

/*
 *  Fade the splash screen and welcome the user before transition
 *  to the menu loop.  Give it a little snazz.
 */
void fadeSplash() {

  uint8_t bmpColor = SSD1327_WHITE;
  uint8_t txtColor = SSD1327_BLACK;
  uint8_t x1, x2, y;

  const int dly = 50;

  display.setTextSize(2);
  x1 = getCenterX("Welcome");
  x2 = getCenterX(netTask.getPlayerName().c_str());
  y = display.height() / 2;

  // Lot of hardcodey bits here, it's just a one off.
  for (int i = 0; i < 24; i++) {
    display.clearDisplay();

    if (i < 16) {
      // Fade out the bitmap
      display.drawBitmap(0, 0, maze128_bmp, 128, 128, bmpColor--);
    }

    if (i >= 8) {
      // Fade in the welcome message
      display.setTextColor(txtColor++);
      display.setCursor(x1, y - 20);
      display.print("Welcome");
      display.setCursor(x2, y);
      display.print(netTask.getPlayerName());
      display.display();
    }

    delay(dly);
  }

  // Let the welcome message linger for a sec; menu will clear it
  // when the rest of the initialization is done
  delay(1000);
}

/*
 * Arduino setup
 */
void setup() {

  // Setup serial port and announce ourselves
  Serial.begin(115200);
  delay(800);

  Serial.println();
  Serial.println("GameMan v" + String(GM_VERSION) + " initializing");

  // Initialize and clear the display
  if (!display.begin(0x3D)) {
    Serial.println("Unable to initialize OLED");
    while (1) yield();
  }

  display.clearDisplay();
  display.display();
  delay(100);

  // Show the splash screen
  showSplash();

  // Initialize the button(s)
  buttonTask.setup(false);

  // Set up the network/player info
  netTask.setup(false);

  // Build the menu
  menuTask.setup(false);

  // Create the event queues
  buttonEvents = xQueueCreate(10, sizeof(button_event_t));
  if (buttonEvents == 0) {
    Serial.println("ERROR: could not create button event queue?");
  }

  // Fade the splash and show the welcome message
  fadeSplash();

  // debug
  showTasks();

  // Start up the background tasks
  netTask.start();
  buttonTask.start();
  menuTask.start();
}


void loop() {
  // Nothing to do here?  should periodically dump task status!
  // Could have this be a battery monitor or something...

  // temporary - debug
  if (((xTaskGetTickCount() - last) / portTICK_PERIOD_MS) >= 10000) {
    showTasks();
    last = xTaskGetTickCount();
  }
}
