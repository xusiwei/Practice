#include <stdio.h>
#include <pthread.h>

#define WAIT_DELAY 100
#define SIGNAL_DELAY 100
#define msleep(ms) usleep((ms)*1000)
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

pthread_cond_t cond;
pthread_mutex_t mutex;
pthread_t threads[20];

int signaled = 0;

void* work(void* i)
{
	size_t n = (size_t)i;

	msleep((n+1) * WAIT_DELAY); // 错开pthread_cond_wait，形成先后

	pthread_mutex_lock(&mutex);
	while(!signaled) {
		printf("[thread %lu] wait...\n", n);
		pthread_cond_wait(&cond, &mutex);
	}

	printf("[thread %lu] wake up.\n", n);
	signaled = 0;
	pthread_mutex_unlock(&mutex);
}

int main()
{
	size_t i;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	for(i=0; i<ARRAY_SIZE(threads); ++i) {
		pthread_create(&threads[i], NULL, work, (void*)i);
	}

	msleep( (ARRAY_SIZE(threads)+2) * WAIT_DELAY ); // 等待所有线程cond_wait

	for(i=0; i<ARRAY_SIZE(threads); ++i) {
		pthread_mutex_lock(&mutex);
		signaled = 1;
		pthread_mutex_unlock(&mutex);
		
		pthread_cond_signal(&cond);
		msleep(SIGNAL_DELAY); // 错开 pthread_cond_signal
	}

	msleep(1000); // 等待所有线程“完工”

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);

	return 0;
}
