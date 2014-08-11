#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/syscall.h>
// #include <boost/noncopyable.hpp>
#include <pthread.h>
#include <stdint.h>

#define dprintf printf

class noncopyable
{
public:
	noncopyable(){}

private:
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
};

#define CHECK_SUCCESS(exp) if(exp != 0) { \
			fprintf(stderr, #exp " check failed!\n"); \
			abort();  \
		}

class Mutex
{
	friend class Condition;
public:
	Mutex() 
	{
		pthread_mutex_init(&mutex_, NULL);
	}

	~Mutex()
	{
		pthread_mutex_destroy(&mutex_);
	}

	void lock() { pthread_mutex_lock(&mutex_); }

	void unlock() { pthread_mutex_unlock(&mutex_); }
private:
	pthread_mutex_t mutex_;
};

class ScopeLock
{
public:
	ScopeLock(Mutex& mutex) : mutex_(mutex) { mutex_.lock(); }
	~ScopeLock() { mutex_.unlock(); }
private:
	Mutex& mutex_;
};

class Condition
{
public:
	Condition(Mutex& mutex) : mutex_(mutex) { pthread_cond_init(&cond_, NULL); } 

	~Condition() { pthread_cond_destroy(&cond_); }

	void wait() { pthread_cond_wait(&cond_, &mutex_.mutex_); }
	
	void notify() { pthread_cond_signal(&cond_); }

	void notifyAll() { pthread_cond_broadcast(&cond_); }
private:
	Mutex& mutex_;
	pthread_cond_t cond_;
};

class Semaphore : public noncopyable 
{
public:
	Semaphore(int32_t count = 1) : count_(count), mutex_(), cond_(mutex_) {}

	void acquire()
	{
		ScopeLock sl(mutex_);
		--count_;
		
		while(count_ < 0) 
		{
			cond_.wait();
		}
	}

	void release()
	{
		{
		ScopeLock sl(mutex_);
		++count_;
		}
		cond_.notify();
		//dprintf("sleepping...\n");
		//sleep(1);
		//dprintf("exit release()\n");
	}
private:
	int32_t   count_;
	Mutex     mutex_;
	Condition cond_;
};

// test
class Thread
{
public:
	virtual void run() = 0;

	virtual ~Thread() {}

	void start() 
	{
		pthread_create(&trd_, NULL, ThreadEntry, this);
	}

	void join()
	{
		pthread_join(trd_, NULL);
	}

	static void* ThreadEntry(void *thiz)
	{
		Thread* pThis = (Thread*)thiz;

		pThis->tid_ = getCurrentTid();
		pThis->run();

		return NULL;
	}

	static pid_t getCurrentTid()
	{
		return syscall(SYS_gettid);
	}
private:
	pid_t     tid_;
	pthread_t trd_;
};

Semaphore sem_buffer(1);
Semaphore sig_comein(0);
char* buffer = NULL;

class Producer : public Thread
{
	void run()
	{
		char buf[128];
	#ifdef FOREVER
		for(int i=0; ;++i)
	#else
		for(int i=0; i<10; ++i)
	#endif
		{
			snprintf(buf, sizeof(buf), "item-%d", i);
			printf("[%5d Producer] produced %s\n", getCurrentTid(), buf);
			
			sem_buffer.acquire();

			buffer = strdup(buf);

			sig_comein.release();
		}
	}
};

class Consumer : public Thread
{
	void run()
	{
	#ifdef FOREVER
		for(;;)
	#else
		for(int i=0; i<10; ++i)
	#endif
		{
			dprintf("  [Consumer] wait for production.\n");
			sig_comein.acquire();
			dprintf("  [Consumer] get it.\n");

			char* item = buffer;

			dprintf("  [Consumer] consumming %s...\n", item);

			sem_buffer.release();
			dprintf("  [Consumer] buffer released.\n");
		}
	}
};

Semaphore tSem(0);

class TestSemaphore : public Thread
{
	void run()
	{
		sleep(1);
		tSem.release();
	}
};

void testSem()
{
	Thread* t = new TestSemaphore();

	t->start();

//	sleep(1);

	tSem.acquire();
	printf("acquire success!\n");
	
	t->join();

	delete t;
}

void testPC()
{
	Thread *producer = new Producer();
	Thread *consumer = new Consumer();

	producer->start();
	consumer->start();

	producer->join();
	consumer->join();

	delete producer;
	delete consumer;
}

int main(int argc, char **argv)
{
	testSem();
	return 0;
}

