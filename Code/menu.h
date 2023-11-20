/*
 *  menu.h - Primary menu task definitions
 *
 *  Abstract:
 *      Definitions for the menu task, which acts as a
 *      launcher and basic UI for the console.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_MENU_H_
#define _GM_MENU_H_

#include "task.h"

#define MENU_MAX_ITEMS  5     // number of items (for now)
#define MENU_MAX_CHARS  15    // room for icon, selection box

#define MENU_NUM_ITEMS  (MENU_MAX_ITEMS - 1)

#define MENU_HEADER     "~~ Menu ~~"
#define MENU_BORDER     3     // 1 pixel hairline
#define MENU_ICON_SZ    16    // 16 x 16 square
#define MENU_X_OFFSET   (MENU_BORDER + MENU_ICON_SZ + 1)
#define MENU_Y_OFFSET   (MENU_BORDER * 2)


typedef struct menuItem {
  char progName[MENU_MAX_CHARS];  // entry name
  const uint8_t *icon;            // teeny icon
  GMTask *prog;                   // game object
} menuItem_t;


class MenuTask : public GMTask {
  public:
    MenuTask();
    void setup() override;
    
  private:
    byte selected = 0;
    int16_t fontHeight = 0;
    int16_t boxHeight = 0;

    menuItem_t items[MENU_MAX_ITEMS];
    int16_t yPos[MENU_MAX_ITEMS];

    void run() override;

    void redrawMenu();
    void showSelected(bool on);
};

#endif
