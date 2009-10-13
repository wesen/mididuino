#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "terminal-io.hh"

using namespace std;

TerminalIOClass::TerminalIOClass(const char *name) {
  terminalIO = this;
  rl_event_hook = &TerminalIOClass::readlinePoll;
  rl_readline_name = name;
}

TerminalIOClass::~TerminalIOClass() {
}

LineIOClass *TerminalIOClass::currentIO;
TerminalIOClass *TerminalIOClass::terminalIO = NULL;

int TerminalIOClass::readlinePoll() {
	if (terminalIO->stopRequested) {
		rl_done = 1;
		return 0;
	}

  while (terminalIO->inputQueue.isDataAvailable()) {
    const string *str = terminalIO->inputQueue.getData();
		uint16_t len = str->length();

		bool isNewline = false;
    if (rl_point > 0) {
			printf("...\n\n");
			isNewline = true;
    } else {
			printf("\r");
    }

		const char *ptr = str->c_str();

		for (int i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				isNewline = true;
				break;
			}
		}
		printf("%s", str->c_str());

		delete str;

		fflush(stdout);
		if (isNewline) {
			rl_on_new_line();
			rl_redisplay();
		}
	}
	
		
	return 0;
}

void TerminalIOClass::doWork() {
	while (!stopRequested) {
    char *line = readline("> ");
    if (!line || stopRequested)
      break;
		if (line && *line) {
			add_history(line);
		}
    outputQueue.putData(line);
  }
	rl_reset_after_signal();
}

bool TerminalIOClass::isDataAvailable() {
  return outputQueue.isDataAvailable();
}

void TerminalIOClass::print(const string &str) {
  inputQueue.putData(str);
}

const string *TerminalIOClass::getData() {
  return outputQueue.getData();
}


