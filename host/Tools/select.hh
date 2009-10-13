#ifndef SELECT_H__
#define SELECT_H__

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#import <list>

class FDSelect {
protected:
	std::list<int> fdList;
	fd_set fds;
	struct timeval tv;

	int initSelect(unsigned long timeoutInMs);
	int _select(fd_set *read, fd_set *write, unsigned long timeoutInMs, bool retry);
	
public:
	FDSelect() { }
	~FDSelect() { }

	void add(int fd) { fdList.push_front(fd); }
	void remove(int fd) { fdList.remove(fd); }

	static const unsigned long INFINITE_TIMEOUT = 0xFFFFFFFF;

	int waitRead(unsigned long timeoutInMs = INFINITE_TIMEOUT, bool retry = false);
	int waitWrite(unsigned long timeoutInMs = INFINITE_TIMEOUT, bool retry = false);
	int waitReadWrite(unsigned long timeoutInMs = INFINITE_TIMEOUT, bool retry = false);

	bool isSet(int fd);
};

#endif /* SELECT_H__ */
