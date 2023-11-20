/*
 *  button.h - Button task definitions
 *
 *  Abstract:
 *      Definitions for the button task, which tracks button state and
 *      fires events for presses, repeats, status requests, etc.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_BUTTON_H_
#define _GM_BUTTON_H_

#include "hardware.h"

// Bit positions in the status word
#define BTN_A     0x01
#define BTN_B     0x02
#define BTN_C     0x04
// reserved
#define BTN_UP    0x10
#define BTN_RT    0x20
#define BTN_DN    0x40
#define BTN_LT    0x80

// Timing
#define BTN_POLL_RATE     10    // Season to taste
#define BTN_REPEAT_DELAY  1000  // not too touchy...
#define BTN_REPEAT_RATE   200   // see what feels right

enum buttAction : byte { btnPressed, btnRepeat, btnReleased };

/*
const byte btnPressed = 0x1;
const byte btnRepeat = 0x2;
const byte btnReleased = 0x4;
*/
typedef struct buttState {
  byte id;                      // bit id in status word
  byte pin;                     // pin number
  byte current;                 // state at last poll
  byte last;                    // saved state (simple hysteresis)
  int pollTime;                 // time we last updated
  int timeInState;              // total millis since last transition
  int lastEventTime;            // to pace our repeats
} button_state_t;

typedef struct buttEvent {
  buttAction action;            // what just happened?
  byte id;                      // to which button?
} button_event_t;

class ButtonTask : public GMTask {
  public:
    ButtonTask();
    void setup() override;
    
    bool buttonsChanged();
    button_state_t getButtonState(byte button);

    // register a callback for a specific button or mask?
    // or just use an event queue and pop button events like network packets?

  private:
    button_state_t buttons[8] = {  // state for each button
      { BTN_A, IO_BTN_A, 1, 1, 0, 0, 0 },
      { BTN_B, IO_BTN_B, 1, 1, 0, 0, 0 },
      { BTN_C, IO_BTN_C, 1, 1, 0, 0, 0 },
      { BTN_UP, IO_BTN_N, 1, 1, 0, 0, 0 },
      { BTN_RT, IO_BTN_E, 1, 1, 0, 0, 0 },
      { BTN_DN, IO_BTN_S, 1, 1, 0, 0, 0 },
      { BTN_LT, IO_BTN_W, 1, 1, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0 }
    };

    byte curButt = 0xff;        // status combined into one flag word
    byte lastButt = 0xff;       // for quick tests ("press any key") :-)

    // debug
    bool buttLight = false;

    void run() override;

    void pollButtonState();
    void queueButtonEvents();
    void enqueue(buttAction a, byte id);
};

#endif
