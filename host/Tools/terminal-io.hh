#ifndef TERMINAL_IO_H__
#define TERMINAL_IO_H__

#include <string>
#include <queue>
#include <stdarg.h>

#include "console.hh"
#include "thread.hh"

#define TERMINAL_BUF_SIZE 4096

void printListeners(const char *buf);

#define CONSOLE_PRINTF(fmt...) { \
		char buf[TERMINAL_BUF_SIZE];	 \
		snprintf(buf, sizeof(buf), fmt);		 \
		printListeners(buf); \
}

//		TerminalIOClass::printTerminal(buf);			\

class LineIOClass {
public:
  virtual void print(const std::string &str) = 0;
  virtual void vPrint(const char *fmt, va_list args) {
		char buf[4096];
		vsnprintf(buf, sizeof(buf), fmt, args);
		print(std::string(buf));
	};
  virtual bool isDataAvailable()             = 0;
  virtual const string *getData()            = 0;
};

class TerminalIOClass : public ThreadedClass, public LineIOClass {
public:
  ThreadSafeQueue<std::string> inputQueue;
  ThreadSafeQueue<std::string> outputQueue;

	static LineIOClass *currentIO;
  static TerminalIOClass *terminalIO;
	
	static void printTerminal(const std::string &str) {
		if (currentIO != NULL) {
			currentIO->print(str);
		}
	}

	static void vPrintTerminal(const char *fmt, va_list args) {
		if (currentIO != NULL) {
			currentIO->vPrint(fmt, args);
		}
	}
	
	static void printfTerminal(const char *fmt, ...) {
		va_list args;
		va_start(args, fmt);
		vPrintTerminal(fmt, args);
		va_end(args);
	}
	
  TerminalIOClass(const char *name = NULL);
  virtual ~TerminalIOClass();
  
  void print(const std::string &str);
  bool isDataAvailable();
  const string *getData();

  static int readlinePoll();
  virtual void doWork();
};

class TerminalConsole : public Console {
public:
	virtual void vDebugOut(const char *fmt, va_list args) {
		TerminalIOClass::vPrintTerminal(fmt, args);
	}
	virtual void vStatusOut(const char *fmt, va_list args) {
		TerminalIOClass::vPrintTerminal(fmt, args);
	}
	virtual void vErrorOut(const char *fmt, va_list args) {
		TerminalIOClass::vPrintTerminal(fmt, args);
	}
};


#endif /* TERMINAL_IO_H__ */
