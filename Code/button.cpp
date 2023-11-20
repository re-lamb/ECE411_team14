/*
 *  button.cpp - Button manager
 *
 *  Abstract:
 *      Runs the button task so clients get a simple and
 *      consistent event-driven interface.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include "GameMan.h"
#include "button.h"

ButtonTask::ButtonTask()
  : GMTask("BUTTON") {
}

void ButtonTask::setup() {

  Serial.println("Button task initializing");

  // Set the pin modes for each defined button
  pinMode(IO_BTN_A, INPUT_PULLUP);
  pinMode(IO_BTN_B, INPUT_PULLUP);
  pinMode(IO_BTN_C, INPUT_PULLUP);

  pinMode(IO_BTN_N, INPUT_PULLUP);
  pinMode(IO_BTN_E, INPUT_PULLUP);
  pinMode(IO_BTN_S, INPUT_PULLUP);
  pinMode(IO_BTN_W, INPUT_PULLUP);

  // debug
  pinMode(LED_BUILTIN, OUTPUT);
}

// Check ALL the buttons and update the flag words
void ButtonTask::pollButtonState() {

  int now = millis();  // Snapshot the current time -- xGetTimeSinceStart() instead??

  lastButt = curButt;
  curButt = 0;

  for (int i = 0; i < 8; i++) {
    buttons[i].last = buttons[i].current;
    buttons[i].current = digitalRead(buttons[i].pin);

    // Change in state?
    if (buttons[i].current != buttons[i].last) {
      // Reset for repeat delay!
      buttons[i].timeInState = 0;
    } else {
      // Add delta
      buttons[i].timeInState += (now - buttons[i].pollTime);
    }

    buttons[i].pollTime = now;
    curButt |= buttons[i].current ? buttons[i].id : 0;
  }

  // Now compute/send updates if needed
  queueButtonEvents();
}

bool ButtonTask::buttonsChanged() {
  return curButt != lastButt;
}

button_state_t ButtonTask::getButtonState(byte button) {

  if (button < 0 || button > 7) return buttons[7];

  return buttons[button];
}

void ButtonTask::enqueue(buttAction a, byte id) {
  button_event_t e;
  e.action = a;
  e.id = id;
  xQueueSend(buttonEvents, (void *)&e, (TickType_t)0);
}

/*
 * Check each button for UP/DOWN transitions and if so queue an event.
 * If a button is held down for a longer period, check to see if a repeat
 * event should be queued.  If the poll rate is fairly long, no separate
 * debouncing processing is needed.  See button.h for tunables.
 *
 * Currently just drops the latest event if the queue is full...
 */
void ButtonTask::queueButtonEvents() {

  int now = millis();  // Snapshot the current time -- or pass it in?

  /*
   * Transition rules:
   *    up -> down      timeInState reset, generate a pressed event
   *    down, no change if timeInState < repeat rate, no-op for now
   *    down, no change if timeInState > initial delay, generate a repeat event
   *    down, no change while repeating, only send events based on time - initdelay / rate
   *    down -> up      generate released event
   */
  for (int i = 0; i < 8; i++) {

    if (buttons[i].current != buttons[i].last) {
      Serial.printf("Button %d ", i);

      if (buttons[i].current == 0) {
        Serial.println("pressed");
        buttons[i].lastEventTime = now;
        enqueue(btnPressed, buttons[i].id);
      } else {
        Serial.println("released");
        buttons[i].lastEventTime = now;
        enqueue(btnReleased, buttons[i].id);
      }
    } else {
      if (buttons[i].current == 0 && buttons[i].timeInState > BTN_REPEAT_DELAY) {
        int elapsed = now - buttons[i].lastEventTime;

        if (elapsed >= BTN_REPEAT_RATE) {
          Serial.printf("Button %d repeats! (%d ms)\n", i, elapsed);
          buttons[i].lastEventTime = now;
          enqueue(btnRepeat, buttons[i].id);
        }
      }
    }
  }
}

/*
 *  ButtonTask main loop
 *
 *  Track the hardware state of the buttons and queue up events for the
 *  "foreground app" that consumes them.  This task loops forever, polling
 *  the buttons at the (configurable) BTN_POLL_RATE.  Events are posted to
 *  the global GMEventQueue.
 */
void ButtonTask::run() {

  Serial.printf("Button task starting up on core %d\n", xPortGetCoreID());

  for (;;) {

    pollButtonState();

    if (buttonsChanged()) {
      Serial.printf("Button state: %s at %d\n", String(curButt, BIN), millis());

      // debug
      buttLight = !buttLight;
      digitalWrite(LED_BUILTIN, buttLight ? HIGH : LOW);
    }

    vTaskDelay(BTN_POLL_RATE / portTICK_PERIOD_MS);
  }

  Serial.println("ButtonTask exiting");
}
