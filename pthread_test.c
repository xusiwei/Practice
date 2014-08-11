#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

void* foo(void*)
{
	printf("pid: %d, tid: %lu\n", getpid(), pthread_self());
	sleep(1);
//	printf("finish\n");
}

int main(int argc, char **argv)
{
	foo(NULL);
	for(int i=0; i<10; ++i) {
		pthread_t tid;
		pthread_create(&tid, NULL, foo, NULL);
		// printf("[main] tid: %d\n", tid);
	}

	sleep(2);
	return 0;
}
