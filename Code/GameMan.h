/*
 *  GameMan.h - External symbols from the main sketch
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

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


// Set to 0 to remove extraneous serial output
#define DEBUG 1

#if DEBUG
#define dprint(...)    Serial.print(__VA_ARGS__)
#define dprintf(...)   Serial.printf(__VA_ARGS__)
#define dprintln(...)  Serial.println(__VA_ARGS__)
#else
#define dprint(...)
#define dprintf(...)
#define dprintln(...)
#endif

#endif