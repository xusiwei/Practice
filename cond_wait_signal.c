#include <stdio.h>
#include <string.h>
#include <pthread.h>

char* buffer = NULL;

pthread_t thread;
pthread_cond_t cond;
pthread_mutex_t mutex;

void* foo(void* args)
{
	pthread_mutex_lock(&mutex);
	buffer = strdup("hello!");
	pthread_cond_signal(&cond);
	// ...
	sleep(5);
	pthread_mutex_unlock(&mutex);
	

	return NULL;
}

int main()
{
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	pthread_create(&thread, NULL, foo, NULL);

	sleep(1);

	pthread_mutex_lock(&mutex);
	while(NULL == buffer) 
	{
		pthread_cond_wait(&cond, &mutex);
	}
	printf("get %s\n", buffer);
	pthread_mutex_unlock(&mutex);

	pthread_join(thread, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	return 0;
}
