#ifndef _GM_GLOBALS_H_
#define _GM_GLOBALS_H_

#include <Adafruit_SSD1327.h>

#include "menu.h"
#include "button.h"
#include "network.h"

// Globals from the .ino
extern Adafruit_SSD1327 display;
extern MenuTask menuTask;
extern ButtonTask buttonTask;
extern NetworkTask netTask;

extern QueueHandle_t buttonEvents;

extern int16_t getCenterX(const char *s);
#endif