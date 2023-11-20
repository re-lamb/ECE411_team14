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
#include <Preferences.h>
#include <Adafruit_SSD1327.h>
#include <WiFi.h>
#include <esp_now.h>

#include "hardware.h"
#include "graphics.h"
#include "task.h"
#include "button.h"
#include "menu.h"
#include "about.h"

/*
 * Globals
 */

// Display (connected through software SPI)
Adafruit_SSD1327 display(128, 128, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Create the main menu / button task
MenuTask menuTask;
ButtonTask buttonTask;

QueueHandle_t buttonEvents;

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

// Courtesy of Espressif: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html#misc
void showTasks() {

  //char writeBuf[1280];    // estimated 32 tasks * 40 bytes
  //vTaskList(writeBuf);    // not available in standard arduino esp-idf build? :-(
  //Serial.println(writeBuf);

  Serial.printf("System says %d tasks are present\n", uxTaskGetNumberOfTasks());
}

/*
 * Show a silly slash screen once at startup.
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

      Serial.print("Logo: ");
      Serial.print(cur_sz);
      Serial.print(" step: ");
      Serial.println(i);
    }

    // the 1.5" oled is a square... assume 0..127 addressible?
    x = (display.width() - cur_sz) / 2;
    y = x;

    display.drawBitmap(x, y, logo, cur_sz, cur_sz, cur_color++);
    display.display();
    delay(dly);
  }

  display.clearDisplay();
  display.display();
}

/*
 * Arduino setup
 */
void setup() {

  char debugData[32];

  // Setup Serial Monitor
  Serial.begin(115200);
  delay(100);

  Serial.println("GameMan v" + String(GM_VERSION) + " initializing");

  // Initialize the button(s)
  buttonTask.setup();

  // Initialize and clear the display
  if (!display.begin(0x3D)) {
    Serial.println("Unable to initialize OLED");
    while (1) yield();
  }

  display.clearDisplay();
  display.display();

  // Put ESP32 into Station mode
  WiFi.mode(WIFI_MODE_STA);

  // Print MAC Address to Serial monitor
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Init the network stack
  if (esp_now_init() != ESP_OK) {
    Serial.println("Unable to initialize ESP NOW");
    while (1) yield();
  } else {
    Serial.println("ESP NOW initialized");
  }

  showBattery();

  showSplash();

  // Build the menu
  menuTask.setup();

  // Create the event queues
  buttonEvents = xQueueCreate(10, sizeof (button_event_t));
  if (buttonEvents == 0) {
    Serial.println("ERROR: could not create button event queue?");
  }
  
  // Start up the background tasks
  buttonTask.start();
  menuTask.start();

  vTaskDelay(1000);  // wait a sec
  showTasks();
}


void loop() {
  // Nothing to do here?  should periodically dump task status!
}
