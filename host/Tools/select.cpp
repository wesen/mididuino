#include <errno.h>

#include "select.hh"

int FDSelect::initSelect(unsigned long timeoutInMs) {
	unsigned long maxFD = 0;
	FD_ZERO(&fds);
	for (std::list<int>::iterator it = fdList.begin(); it != fdList.end(); it++) {
		FD_SET(*it, &fds);
		if (*it > maxFD) {
			maxFD = *it;
		}
	}

	unsigned long secs = timeoutInMs / 1000;
	timeoutInMs -= (secs * 1000);
	unsigned long usecs = (timeoutInMs * 1000);
	tv.tv_sec = secs;
	tv.tv_usec = usecs;
	
	return maxFD;
}

int FDSelect::_select(fd_set *readFds, fd_set *writeFds,
											unsigned long timeoutInMs, bool retry) {
	bool finished = false;
	int selected = -1;
	
	do {
		int maxFD = initSelect(timeoutInMs);
		selected = select(maxFD + 1, readFds, writeFds, NULL,
											(timeoutInMs == INFINITE_TIMEOUT) ? NULL : &tv);
		finished = true;

		/* Even though select() is not supposed to set errno to EAGAIN
		 * (according to the linux man page), it seems that errno can be set
     * to EAGAIN on some cygwin systems. Thus, we need to catch that
     * here.
		 */
		if (retry) {
			if ((selected < 0) && ((errno == EAGAIN) ||
														 (errno == EINTR))) {
				finished = false;
			}
		}
	} while (!finished);

	return selected;
}

int FDSelect::waitRead(unsigned long timeoutInMs, bool retry) {
	return _select(&fds, NULL, timeoutInMs, retry);
}

int FDSelect::waitWrite(unsigned long timeoutInMs, bool retry) {
	return _select(NULL, &fds, timeoutInMs, retry);
}

int FDSelect::waitReadWrite(unsigned long timeoutInMs, bool retry) {
	return _select(&fds, &fds, timeoutInMs, retry);
}

bool FDSelect::isSet(int fd) {
	return FD_ISSET(fd, &fds);
}
