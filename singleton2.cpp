#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

pid_t gettid()
{
	return syscall(SYS_gettid);
}

struct Foo
{
	int pad;
	Foo() { printf("Foo_%p create in %d...\n", this, gettid());	}
	~Foo(){ printf("Foo_%p destroy in %d...\n", this, gettid()); }
};

template<class T>
class Singleton
{
public:
	static T* getInstance()
	{
		printf("in getInstance...\n");

		pthread_once(&once_, init);
		return pInst_;
	}

private:
	static void init()
	{
		pInst_ = new T();
		::atexit(destroy);
	}

	static void destroy()
	{
		delete pInst_;
	}

	static T* pInst_;
	static pthread_once_t once_;
};

template<class T>
T* Singleton<T>::pInst_ = NULL;

template<class T>
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

pthread_t thread;

void* foo(void* args)
{
	sleep(1);
	Foo* pf = Singleton<Foo>::getInstance();
	printf("[foo] getInstance: %p\n", pf);

	Foo* pf2 = Singleton<Foo>::getInstance();
	printf("[foo] getInstance: %p\n", pf2);

	return NULL;
}

int main()
{
	pthread_create(&thread, NULL, foo, NULL);

	sleep(2);
	Foo* pf = Singleton<Foo>::getInstance();
	printf("[main] getInstance: %p\n", pf);

	Foo* pf2 = Singleton<Foo>::getInstance();
	printf("[main] getInstance: %p\n", pf2);

	pthread_join(thread, NULL);
	return 0;
}

