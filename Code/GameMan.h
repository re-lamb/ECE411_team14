#ifndef _GM_GLOBALS_H_
#define _GM_GLOBALS_H_

#include <Adafruit_SSD1327.h>

#include "menu.h"
#include "button.h"

// Globals from the .ino
extern Adafruit_SSD1327 display;
extern MenuTask menuTask;
extern ButtonTask buttonTask;

extern QueueHandle_t buttonEvents;
#endif