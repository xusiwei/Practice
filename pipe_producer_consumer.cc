#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int pipePorducer[2];
int pipeConsumer[2];

#define READ_END  0
#define WRITE_END 1

#define MAXN 1024
char buff[MAXN];

/*
producer activity:
	1. produce item.
	2. wait slot(if buffer is full).
	3. put it to buffer.
	4. signal consumer.
*/
int producer()
{
	for(int i=0; i<10; ++i)	{
		// 1. produce item
		sprintf(buff, "item-%d", i);
		printf("[%d] produced %s\n", getpid(), buff); 

		// 2. wait signal
		// read(pipeConsumer[READ_END]);

		if( -1 == write(pipePorducer[WRITE_END], buff, strlen(buff)+1) ) {
			fprintf(stderr, "write pipe error! %d: %s\n", errno, strerror(errno));
			exit(-1);
		}
	}
}

/*
consumer activity:
	1. wait item(if item queue is empty).
	2. get item from queue.
	3. consume it.
	4. signal producer.
*/
int consumer()
{
	//for( ; ; ) {
	for(int i=0; i<10; ++i) {
		if(-1 == read(pipePorducer[READ_END], buff, MAXN)) {
			fprintf(stderr, "read pipe error! %d: %s\n", errno, strerror(errno));
			exit(-2);
		}
		
		printf("  [%d] consumed %s\n", getpid(), buff);
	}
}

void init()
{
	if(pipe(pipePorducer) < 0) {
		fprintf(stderr, "pipe error! %d: %s\n", errno, strerror(errno));
		exit(-1);
	}

	if(pipe(pipeConsumer) < 0) {
		fprintf(stderr, "pipe error! %d: %s\n", errno, strerror(errno));
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	init();

	pid_t pid = fork();

	if(pid > 0)	 {
		producer();
	}
	else if(!pid) {
		consumer();
	}
	else {
		fprintf(stderr, "fork error!\n");
		exit(-3);
	}

	return 0;
}


