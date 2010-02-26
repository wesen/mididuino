#include <stdarg.h>
#include <stdio.h>
#include "logging.h"

/* logging and debug functionality */

int debugLevel = 0;

FILE *logDescriptor(log_type_t log) {
  if (log == LOG_ERROR) {
    return stderr;
  } else {
    return stdout;
  }
}

void logPrintType(log_type_t log) {
  switch (log) {
  case LOG_USAGE:
    fprintf(logDescriptor(log), "Usage: ");
    break;
		
  case LOG_INFO:
    fprintf(logDescriptor(log), "INFO: ");
    break;

  case LOG_DEBUG:
    fprintf(logDescriptor(log), "DEBUG: ");
    break;

  case LOG_WARNING:
    fprintf(logDescriptor(log), "WARNING: ");
    break;

  case LOG_PROGRESS:
    fprintf(logDescriptor(log), "PROGRESS: ");
    break;

  case LOG_STATUS:
    fprintf(logDescriptor(log), "STATUS: ");
    break;

  case LOG_ERROR:
    fprintf(logDescriptor(log), "ERROR: ");
    break;

  default:
    fprintf(logDescriptor(log), "INFO: ");
    break;
  }

  fflush(logDescriptor(log));
}

void logString(log_type_t log, char *msg) {
  logPrintType(log);
  fprintf(logDescriptor(log), "%s", msg);
  fflush(logDescriptor(log));
}

void logPrintf(log_type_t log, const char *fmt, ...) {
  va_list lp;
  va_start(lp, fmt);
  logPrintType(log);
  vfprintf(logDescriptor(log), fmt, lp);
  va_end(lp);
  fflush(logDescriptor(log));
}

void logvPrintf(log_type_t log, const char *fmt, va_list lp) {
  logPrintType(log);
  logvPrintfContinue(log, fmt, lp);
}

void logStringContinue(log_type_t log, char *msg) {
  fprintf(logDescriptor(log), "%s", msg);
  fflush(logDescriptor(log));
}


void logPrintfContinue(log_type_t log, const char *fmt, ...) {
  va_list lp;
  va_start(lp, fmt);
  vfprintf(logDescriptor(log), fmt, lp);
  va_end(lp);
  fflush(logDescriptor(log));
}

void logvPrintfContinue(log_type_t log, const char *fmt, va_list lp) {
  vfprintf(logDescriptor(log), fmt, lp);
  fflush(logDescriptor(log));
}

void logHexdump(log_type_t log, unsigned char *buf, int len) {
  int i;
  for (i = 0; i < len; i+=16) {
    int j;
    for (j = i; j < MIN(i+16, len); j++) {
      logPrintfContinue(log, "%2x ", buf[j]);
    }
    logPrintfContinue(log, "\n");
  }
}

void debugString(int level, char *msg) {
  if (level <= debugLevel) {
    logString(LOG_DEBUG, msg);
  }
}

void debugPrintf(int level, const char *fmt, ...) {
  if (level <= debugLevel) {
    va_list lp;
    va_start(lp, fmt);
    logvPrintf(LOG_DEBUG, fmt, lp);
    va_end(lp);
  }
}

void debugvPrintf(int level, const char *fmt, va_list lp) {
  if (level <= debugLevel) {
    logvPrintf(LOG_DEBUG, fmt, lp);
  }
}

void debugStringContinue(int level, char *msg) {
  if (level <= debugLevel) {
    logStringContinue(LOG_DEBUG, msg);
  }
}

void debugPrintfContinue(int level, const char *fmt, ...) {
  if (level <= debugLevel) {
    va_list lp;
    va_start(lp, fmt);
    logvPrintfContinue(LOG_DEBUG, fmt, lp);
    va_end(lp);
  }
}

void debugvPrintfContinue(int level, const char *fmt, va_list lp) {
  if (level <= debugLevel) {
    logvPrintfContinue(LOG_DEBUG, fmt, lp);
  }
}


void debugHexdump(int level, unsigned char *buf, int len) {
  if (level <= debugLevel) {
    logHexdump(LOG_DEBUG, buf, len);
  }
}
