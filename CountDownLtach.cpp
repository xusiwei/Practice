#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

class Mutex
{
	friend class Condition;
	// noncopyable
	Mutex(const Mutex&);
	Mutex& operator=(const Mutex&);

public:
	Mutex(){ pthread_mutex_init(&mutex_, NULL); }
	~Mutex() { pthread_mutex_destroy(&mutex_); }

	void lock() { pthread_mutex_lock(&mutex_); }
	void unlock() { pthread_mutex_unlock(&mutex_); }

private:
	pthread_mutex_t mutex_;
};

class MutexGuard
{
	// noncopyable
	MutexGuard(const MutexGuard&);
	MutexGuard& operator=(const MutexGuard&);

public:
	MutexGuard(Mutex& mutex) : mutex_(mutex) { mutex_.lock(); }
	~MutexGuard() { mutex_.unlock(); }

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

class CountdownLtach
{
	// noncopyable
	CountdownLtach(const CountdownLtach&);
	CountdownLtach& operator=(const CountdownLtach&);

public:
	CountdownLtach() : count_(0), countLock_(), cond_(countLock_) {}
	CountdownLtach(int count) : count_(count), countLock_(), cond_(countLock_) {}

	void wait() { 
		MutexGuard _l(countLock_);
		while(count_ > 0) {
			cond_.wait();
		}
	}

	void countDown() {  
		MutexGuard _l(countLock_);
		if(--count_ == 0) cond_.notifyAll();
	}
private:
	int   count_;
	Mutex countLock_;
	Condition cond_;
};

class Thread
{
	// noncopyable
	Thread(const Thread&);
	Thread& operator=(const Thread&);

public:
	Thread() : tid_(0) {}

	virtual void run() = 0;
	
	void start() {
		pthread_create(&thread_, NULL, Thread::Entry, this);
	}

	void join() {
		pthread_join(thread_, NULL);
	}

	pid_t getTid() {
		return tid_;
	}

private:
	static void* Entry(void* pthis) {
		Thread* pInst = static_cast<Thread*>(pthis);

		pInst->tid_ = getpid();
		pInst->run();

		return NULL;
	}

	pid_t     tid_;
	pthread_t thread_;
};



CountdownLtach cdLatch(2);

class TestThread : public Thread
{
	void run()
	{
		printf("TestThread %d %lu\n", getpid(), pthread_self());
		for(int i=0; i<2; i++) {
			sleep(1);
			printf("counting down...\n");
			cdLatch.countDown();
		}
	}
};

int main(int argc, char **argv)
{
	Thread* pThread = new TestThread();

	printf("[main] thread: %d %lu\n", getpid(), pthread_self());

	pThread->start();

	printf("[main] wait for count down to zero!\n");
	cdLatch.wait();
	printf("[main] count down finish!\n");

	pThread->join();

	delete pThread;
	return 0;
}

