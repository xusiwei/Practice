#include <stdio.h>
#include <pthread.h>

pthread_t trd;
pthread_cond_t cond;
pthread_mutex_t mutex;

void* foo(void* args)
{
	// sleep(2);
	pthread_cond_signal(&cond);
	return NULL;
}

int main()
{
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	pthread_mutex_lock(&mutex);
	pthread_create(&trd, NULL, foo, NULL);

	sleep(1);
	
	printf("before wait\n");
	pthread_cond_wait(&cond, &mutex);
	printf("after wait\n");

	pthread_mutex_unlock(&mutex);

	pthread_join(trd, NULL);
	printf("foo thread finish!\n");

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

	return 0;
}

