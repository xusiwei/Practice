#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h> // atol
#include <errno.h>
#include <string.h>

#define BSIZE 16
char buff[BSIZE];

int main(int argc, char *argv[])
{
	if(argc < 2) {
	   	printf("Usage: %s fd\n", argv[0]);
		exit(-1);
	}

	for(int i=0; i<argc; ++i)
		printf("%s\n", argv[i]);

	int fdold = atol(argv[1]);
	
	// int fd = fcntl(fdold, F_DUPFD); // int fd = dup(fdold);
	int fd = fcntl(fdold, F_DUPFD, 100); // int fd = dup2(fdold, 100);
	printf("fd: %d\n", fd);

	int fd_flag = fcntl(fd, F_GETFD);
	printf("fd_flag: %x\n", fd_flag);

	int cnt = 0;
	while( cnt = read(fd, buff, BSIZE) )
	{
		if(-1 == cnt) {
			printf("error! %d: %s\n", errno, strerror(errno));
			exit(-2);
		}
		// printf("readed: %d\n", cnt);
		for(int i=0; i<cnt; ++i) {
			printf("%c", buff[i]);
		}
	}

	return 0;
}
