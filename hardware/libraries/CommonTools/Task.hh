#ifndef TASK_H__
#define TASK_H__

class Task {
public:
  uint16_t interval;
  uint16_t lastExecution;

  void (*taskFunction)();
  Task(uint16_t _interval, void (*_taskFunction)() = NULL) {
    interval = _interval;
    lastExecution = 0;
    taskFunction = _taskFunction;
  }

  virtual void run() {
    if (taskFunction != NULL)
      taskFunction();
  }

  void check() {
    uint16_t clock = read_slowclock();
    if (clock_diff(lastExecution, clock) > interval) {
      run();
      lastExecution = clock;
    }
  }
};

#endif /* TASK_H__ */
