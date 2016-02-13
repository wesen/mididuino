/*
 * MidiCtrl - Regular tasks
 *
 * (c) 2009 - 2011 - Manuel Odendahl - wesen@ruinwesen.com
 */

#include "Platform.h"
#include "Task.hh"

Task::Task(uint16_t _interval, void (*_taskFunction)()) {
  interval = _interval;
  lastExecution = 0;
  taskFunction = _taskFunction;
  starting = true;
}

void Task::run() {
    if (taskFunction != NULL) {
      taskFunction();
    }
}

void Task::checkTask() {
    uint16_t clock = read_slowclock();
    if (clock_diff(lastExecution, clock) > interval || starting) {
      run();
      lastExecution = clock;
      starting = false;
    }
}
