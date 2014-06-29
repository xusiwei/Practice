#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAXN 1024
char buff[MAXN];

int main(int argc, char *argv[])
{
	int pipefd[2];

	if(pipe(pipefd) < 0) {
		fprintf(stderr, "pipe error! %d: %s\n", errno, strerror(errno));
		exit(-1);
	}

	pid_t pid = fork();
	if(pid > 0) { // parent
		sprintf(buff, "hello, [process %d]!", pid);
		size_t cnt = write(pipefd[1], buff, MAXN);
		if( !cnt || -1 == cnt ) {
			fprintf(stderr, "write pipe error! %d: %s\n", errno, strerror(errno));
			exit(-2);
		}
		printf("[%d] send: %s\n", getpid(), buff);
	}
	else if(pid == 0) { // child
		memset(buff, -1, sizeof(buff));
		
		size_t cnt = read(pipefd[0], buff, MAXN);
		printf("buff[%d]: %d\n", cnt, buff[cnt]);
		
		if(buff[cnt]) buff[cnt] = '\0';
		printf("[%d] recv: %s\n", getpid(), buff);
	}
	else { // error
		fprintf(stderr, "fork error! %d: %s\n", errno, strerror(errno));
		exit(-4);
	}

	return 0;
}


