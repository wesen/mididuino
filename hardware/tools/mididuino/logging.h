#ifndef LOGGING_H__
#define LOGGING_H__

#include <stdarg.h>

#define MIN(a, b) ((a > b) ? b : a)

extern int debugLevel;

typedef enum log_type_e {
	LOG_INFO = 0,
	LOG_USAGE, 
	LOG_WARNING,
	LOG_PROGRESS,
	LOG_STATUS,
	LOG_ERROR,
	LOG_DEBUG
} log_type_t;

void logString(log_type_t log, char *msg);
void logPrintf(log_type_t log, const char *fmt, ...);
void logvPrintf(log_type_t log, const char *fmt, va_list lp);

void logStringContinue(log_type_t log, char *msg);
void logPrintfContinue(log_type_t log, const char *fmt, ...);
void logvPrintfContinue(log_type_t log, const char *fmt, va_list lp);

void logHexdump(log_type_t log, unsigned char *buf, int len);

void debugString(int level, char *msg);
void debugPrintf(int level, const char *fmt, ...);
void debugvPrintf(int level, const char *fmt, va_list lp);

void debugStringContinue(int level, char *msg);
void debugPrintfContinue(int level, const char *fmt, ...);
void debugvPrintfContinue(int level, const char *fmt, va_list lp);

void debugHexdump(int level, unsigned char *buf, int len);

#endif /* LOGGING_H__ */
