#include <unistd.h>
#include <stdio.h>

#include <pthread.h>

struct Foo
{
	int pad;
	Foo() { printf("Foo_%p create...\n", this);	}
	~Foo(){ printf("Foo_%p destroy...\n", this); }
};

template<class T>
class Singleton
{
public:
	static T* getInstance()
	{
		printf("in getInstance...\n");
		static T t;
		return &t;
	}
};

pthread_t thread;

void* foo(void* args)
{
	sleep(1);
	Foo* pf = Singleton<Foo>::getInstance();
	printf("[foo] getInstance: %p\n", pf);

	return NULL;
}

int main()
{
	pthread_create(&thread, NULL, foo, NULL);

	sleep(2);
	Foo* pf = Singleton<Foo>::getInstance();
	printf("[main] getInstance: %p\n", pf);

	pthread_join(thread, NULL);
	return 0;
}
