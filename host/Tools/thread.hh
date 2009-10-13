#ifndef THREAD_H__
#define THREAD_H__

#include <assert.h>
#include <pthread.h>

#include <string>
#include <queue>
#include <list>

using namespace std;

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTF(str, args...) \
  if (ThreadSynchronization::debug) printf(str, ## args)
#else
#define DEBUG_PRINTF(stder, args...)
#endif

class ThreadSynchronization {
protected:
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_t owner;
	int level;

public:
	static bool debug;

  ThreadSynchronization() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    owner = (pthread_t)-1;
		level = 0;
  }

  virtual ~ThreadSynchronization() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
  }

  void lock() {
    DEBUG_PRINTF("lock %p from thread %p (owner: %p, level %d)\n", this, pthread_self(), owner, level);

#ifdef __APPLE__
		/* apparently can't relock an already self owned lock */
		if (owner == pthread_self()) {
			level++;
			return;
		}
#endif
		
    pthread_mutex_lock(&mutex);
		DEBUG_PRINTF("acquired lock %p from thread %p\n", this, pthread_self());
		level++;
		if (owner != (pthread_t)-1) {
			assert(owner == pthread_self());
		} else {
			owner = pthread_self();
		}
  }

  void unlock() {
    DEBUG_PRINTF("unlock %p from thread %p (owner: %p, level: %d)\n", this, pthread_self(), owner, level);
    assert(owner == pthread_self());
		level--;
		if (level == 0) {
			owner = (pthread_t)-1;
			pthread_mutex_unlock(&mutex);
		}
  }

  void wait() {
    DEBUG_PRINTF("wait %p from thread %p (owner: %p)\n", this, pthread_self(), owner);
    assert(owner == pthread_self());
    pthread_cond_wait(&cond, &mutex);
  }
  
  void signal() {
    DEBUG_PRINTF("signal %p from thread %p (owner: %p)\n", this, pthread_self(), owner);
    assert(owner == pthread_self());
    pthread_cond_signal(&cond);
  }
};

template<class C> class ThreadSafeObject : public ThreadSynchronization {
protected:
  C *obj;

public:
  ThreadSafeObject(C *_obj) : ThreadSynchronization(), obj(_obj) {
  }

  virtual ~ThreadSafeObject() {
  }

  C *getLockedObject() {
    lock();
    return obj;
  }
};

class ThreadedClass {

public:
  ThreadedClass() : stopRequested(false), running(false) {
  }

  ~ThreadedClass() {
  }

  virtual void start() {
    assert(running == false);
    running = true;
    pthread_create(&thread, NULL, &ThreadedClass::startThread, this);
  }

  virtual void stop() {
    assert(running == true);
    running = false;
    stopRequested = true;
    //    pthread_join(&thread, 0);
  }

	bool isRunning() {
		return running;
	}

protected:
  pthread_t thread;
  
  volatile bool stopRequested;
  volatile bool running;

  static void *startThread(void *obj) {
    reinterpret_cast<ThreadedClass *>(obj)->doWork();
  }

  virtual void doWork() {
  }
};

template<class C> class ThreadSafeQueue : public ThreadSafeObject<queue<C> > {
protected:
  queue<C> myQueue;

public:
  ThreadSafeQueue() : ThreadSafeObject<queue<C> >(&myQueue) {
  }

  bool isDataAvailable() {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    bool result = !q->empty();
    ThreadSafeObject<queue<C> >::unlock();
    return result;
  }

  const C *getData() {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    C *result = new C(q->front());
    q->pop();
    ThreadSafeObject<queue<C> >::unlock();
    return result;
  }

  void putData(const C &data) {
    queue<C> *q = ThreadSafeObject<queue<C> >::getLockedObject();
    q->push(data);
    ThreadSafeObject<queue<C> >::unlock();
  }
};

template<class C> class ThreadSafeList : public ThreadSafeObject<list<C> > {
protected:
	list<C> myList;

public:
	ThreadSafeList() : ThreadSafeObject<list<C> > (&myList) {
	}

	void remove(const C &c) {
		list<C> *l = ThreadSafeObject<list<C> >::getLockedObject();
		l->remove(c);
		ThreadSafeObject<list<C> >::unlock();
	}

	void push_front(const C &c) {
		list<C> *l = ThreadSafeObject<list<C> >::getLockedObject();
		l->push_front(c);
		ThreadSafeObject<list<C> >::unlock();
	}
};

#endif /* THREAD_H__ */
