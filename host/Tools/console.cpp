#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "console.hh"

bool Console::debugMode = false;
bool Console::quietMode = false;

void Console::vDebugOut(const char *fmt, va_list args) {
	if (debugMode) {
		(void)vfprintf(stderr, fmt, args);
	}
}

void Console::debugOut(const char *fmt, ...) {
	if (debugMode) {
		va_list args;
		va_start(args, fmt);
		vDebugOut(fmt, args);
		va_end(args);
	}
}

void Console::debugFlush() {
	if (debugMode) {
		fflush(stdout);
	}
}

void Console::vErrorOut(const char *fmt, va_list args) {
	vfprintf(stderr, fmt, args);
}

void Console::errorOut(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vErrorOut(fmt, args);
	va_end(args);
}

void Console::errorFlush() {
	fflush(stderr);
}

void Console::vStatusOut(const char *fmt, va_list args) {
	vprintf(fmt, args);
}

void Console::statusOut(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vStatusOut(fmt, args);
	va_end(args);
}

void Console::statusFlush() {
	fflush(stdout);
}

Console *console = NULL;
