#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdio.h>

pid_t gettid()
{
	syscall(SYS_gettid);
}

void* foo(void* args)
{
	int i = (int)args;
	printf("pid: %5d, tid: %5d no: %5d, stack: %p\n", getpid(), gettid(), i, &i);
	for(;;) ++i;
//	printf("finish\n");
}

int main(int argc, char **argv)
{
	int count;
	
	for(;;++count) {
		pthread_t tid;
		pthread_create(&tid, NULL, foo, (void*)count);
		// printf("[main] tid: %d\n", tid);
	}

	printf("total thread: %d\n", count);
	return 0;
}
