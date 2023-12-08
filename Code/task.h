/*
 *  task.h - Declare the app/game base class
 *
 *  Abstract:
 *      Defines the framework for running a C++ object on its own
 *      ESP32 (FreeRTOS under the hood!) "task".
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_TASK_H_
#define _GM_TASK_H_

class GMTask {
public:
  GMTask(const char *name, uint16_t size = 8192, uint8_t prio = 2, BaseType_t core = APP_CPU_NUM);

  virtual ~GMTask();

  // Called by the menu prior to task launch
  virtual void setup(bool rsvp) = 0;

  // Called by the menu to launch the task
  void start();

  void suspend();
  void resume();
  void stop();
  void end();

  TaskHandle_t getHandle();
  const char *getName();
  void *getData();
  bool isRunning();

protected:
  // Body of the task (override for subclass-specific code,
  // i.e., put the app/game code here!)
  virtual void run() = 0;
  static void delay(int ms);
  static bool elapsed(int ms, TickType_t since);
  static bool elapsed(int ms, TickType_t since, TickType_t now);

private:
  const char *_taskName;
  uint16_t _stackSize;
  uint8_t _priority;
  BaseType_t _coreId;
  void *_taskData;
  TaskHandle_t _taskHandle;

  volatile bool _taskRunning = false;

  /*
 * Create an instance of the C++ object to run on the new task.  This is
 * required because the ESP-IDF library is straight C, so it can't use a
 * class method directly.  So start() calls us with ourselves as the parameter
 * so we can create a new instance and invoke the run() method as the task,
 * which is weird and gross and I'm not sure I or anybody else actually really
 * understands this.
 */
  static void runTask(void *pvParameters) {
    GMTask *pTask = (GMTask *)pvParameters;
    pTask->run();
    pTask->stop();
  }
};

#endif
