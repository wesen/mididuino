#ifndef CONSOLE_H__
#define CONSOLE_H__

class Console {
public:
	Console() {
	}

	virtual ~Console() {
	}

	/** true iff --debug option specified **/
	static bool debugMode;
	
	/** added quietMode mainly to silence statusOut when using javascript console */
	static bool quietMode;
	
	/** printf 'fmt, ...' if debugMode **/
	virtual void vDebugOut(const char *fmt, va_list args);
	virtual void debugOut(const char *fmt, ...);
	virtual void debugFlush();

	/** printf 'fmt, ...' **/
	virtual void vErrorOut(const char *fmt, va_list args);
	virtual void errorOut(const char *fmt, ...);
	virtual void errorFlush();

	/** printf 'fmt, ...' to status file descriptor (currently stdout) **/
	virtual void vStatusOut(const char *fmt, va_list args);
	virtual void statusOut(const char *fmt, ...);
	virtual void statusFlush();
};

extern Console *console;

#endif /* CONSOLE_H__ */
