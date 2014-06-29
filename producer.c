#include <stdio.h>
#include <pthread.h>
#include <string.h>   // for memset
#include <stdlib.h>  // for rand
#include <time.h>    // for time

#ifdef WIN32
	#include <windows.h>
	#define sleep(s) Sleep((s)*1000)
	#define msleep(ms) Sleep(ms)
#else
	#include <unistd.h>
	#define msleep(ms) usleep((ms)*1000)
#endif

typedef int item_t;

#define BUFMAX 4
#define CONSOME_LATENCY 500
#define PRODUCE_LATENCY 1000
// #define CONSOME_LATENCY rand()%1000
// #define PRODUCE_LATENCY rand()%1000

// for synchronization:
pthread_mutex_t lock;
pthread_cond_t cond_produce;
pthread_cond_t cond_consume;

// for buffer and it's operation:
item_t buffer[BUFMAX];
volatile int counter=0;
volatile int out=0;
volatile int in=0;

/*
consumer activity:
	1. wait item(if item queue is empty).
	2. get item from queue.
	3. consume it.
	4. signal producer.
*/
void *consumerProc(void* args)
{
	item_t item;
	size_t latency = (size_t)args;
	// for(;;) {
	int i; for(i=0; i<10; i++) {
		pthread_mutex_lock(&lock);
		
		// 1. wait item(if item queue is empty).
		while( counter == 0 ) {
			printf("  [CONSUMER] buffer is EMPTY, I am waiting for ITEM.\n");
			pthread_cond_wait(&cond_produce, &lock);
		}
		
		// 2. get item from queue.
		item = buffer[out];
		out = (out+1) % BUFMAX;
		counter--;
		
		pthread_mutex_unlock(&lock);
		
		// 3. consume it.
		printf("  [CONSUMER] I am consumming item %d...\n", item);
		msleep(latency);
		
		// 4. signal producer.
		printf("  [CONSUMER] Hey! I was consumed item %d.\n", item);
		pthread_cond_signal(&cond_consume);
	}
	return 0;
}

/*
producer activity:
	1. wait slot(if buffer is full).
	2. produce item.
	3. put it to buffer.
	4. signal consumer.
*/
void *producerProc(void* args)
{
	item_t item;
	size_t latency = (size_t)args;
	static int item_count = 0;
	// for(;;) {
	int i; for(i=0; i<10; i++) {
		pthread_mutex_lock(&lock);
		
		// 1. wait slot(if buffer is full).
		while( counter == BUFMAX ) {
			printf("[producer] buffer is FULL, I am waiting for SLOT.\n");
			pthread_cond_wait(&cond_consume, &lock);
		}
		
		// 2. produce item.
		item = ++item_count;
		printf("[producer] I am producing item %d...\n", item);
		msleep(latency);
		
		// 3. put it to buffer.
		buffer[in] = item;
		in = (in+1) % BUFMAX;
		counter++;
		
		pthread_mutex_unlock(&lock);
		
		// 4. signal consumer.
		printf("[producer] Hey! I was produced item %d.\n", item);
		pthread_cond_signal(&cond_produce);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	pthread_t consumer;
	pthread_t producer;

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond_consume, NULL);
	pthread_cond_init(&cond_produce, NULL);
	
	// srand(time(NULL));
	memset(buffer, -1, sizeof(buffer));
	
	pthread_create(&producer, NULL, producerProc, (void*)PRODUCE_LATENCY);
	pthread_create(&consumer, NULL, consumerProc, (void*)CONSOME_LATENCY);
	
	msleep(100);

	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond_consume);
	pthread_cond_destroy(&cond_produce);
	return 0;
}
