/*
 *  task.cpp - Base class for a GameMan app/game program
 *
 *  Abstract:
 *      Defines a C++ class framework that lets us run separate
 *      tasks on the ESP32 using the FreeRTOS plumbing under the
 *      hood.  This lets us use the underlying C libraries to run
 *      each application as its own mostly self-contained program
 *      (like running an Arduino sketch but with real priority
 *      scheduling, a separate stack, and even the ability to peg
 *      to a specific core on the MCU).  This is cobbled together
 *      from reading a zillion online sources and the Espressif
 *      ESP-IDF documentation.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Arduino.h>
#include "task.h"

GMTask::GMTask(const char *name, uint16_t size, uint8_t prio, BaseType_t core) {
  _taskName = name;
  _stackSize = size;
  _priority = prio;
  _coreId = core;
  _taskData = nullptr;    // not currently used
  _taskHandle = nullptr;
}

GMTask::~GMTask() {
}

void GMTask::start() {
  if (_taskHandle != NULL) {
    Serial.printf("Task %s begin() called but already running!?\n", _taskName);
  } else {
    Serial.printf("Task %s starting!\n", _taskName);
    _taskRunning = true;
    
    xTaskCreatePinnedToCore(this->runTask, _taskName, _stackSize, this, _priority, &_taskHandle, _coreId);
  }
}

void GMTask::stop() {
  Serial.printf("Task %s stopping\n", _taskName);
  _taskRunning = false;
  suspend();
}

void GMTask::delay(int ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

bool GMTask::elapsed(int ms, TickType_t since) {
  return ((xTaskGetTickCount() - since) / portTICK_PERIOD_MS) >= ms;
}

bool GMTask::elapsed(int ms, TickType_t since, TickType_t now) {
  return ((now - since) / portTICK_PERIOD_MS) >= ms;
}

void GMTask::suspend() {
  if (_taskHandle) { vTaskSuspend(_taskHandle); }
}

void GMTask::resume() {
  if (_taskHandle) { vTaskResume(_taskHandle); }
}

void GMTask::end() {
  if (_taskHandle == NULL) return;

  TaskHandle_t temp = _taskHandle;
  _taskHandle = NULL;
  vTaskDelete(temp);
}

const char *GMTask::getName() {
  return _taskName;
}

// Can be used to return a result from the task
void *GMTask::getData() {
  return _taskData;
}

TaskHandle_t GMTask::getHandle() {
  return _taskHandle;
}

bool GMTask::isRunning() {
  return (_taskHandle != NULL && _taskRunning);
}
