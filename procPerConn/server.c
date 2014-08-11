#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LISTENQ     5

void do_echo(int);

#define CHECK_FAILED(expr, msg) \
	if(expr) { \
		fprintf(stderr, msg "detail: %s\n", strerror(errno)); \
		exit(errno); \
	}

#define TRACE(fmt, expr) printf(#expr ": " fmt, expr)

int server_port = 0;

int main(int argc, char **argv)
{
	int sock, ret; 
	int connfd;
	int socklen;
	int childpid=0;

	struct sockaddr_in server_addr, client_addr;

	if(argc < 2)
	{
		printf("usage: %s port\n", argv[0]);
		exit(-1);
	}
	server_port = atol(argv[1]);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_FAILED((-1 == sock), "socket error, ");
	TRACE("%d\n", sock);

	// memset(&addr, 0, sizeof(struct sockaddr_in));
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(server_port);

	ret = bind(sock, (const struct sockaddr*)&server_addr, sizeof(server_addr));
	CHECK_FAILED((-1 == ret), "bind error, ");
	TRACE("(bind): %d\n", ret);

	ret = listen(sock, LISTENQ);
	CHECK_FAILED((-1 == ret), "listen error, ");
	TRACE("(listen): %d\n", ret);

	for(;;) {
		socklen = sizeof(client_addr);

		printf("[%d] wait request\n", getpid());
		connfd = accept(sock, (struct sockaddr*)&client_addr, &socklen);
		CHECK_FAILED((-1 == connfd), "accept error, ");

		if((childpid = fork()) == 0 )
		{
			printf("[%d] accept Request at %d.\n", getpid(), connfd);
			close(sock);
			do_echo(connfd);
			exit(0);
		}
		CHECK_FAILED((childpid < 0), "fork error, ");
		close(connfd);
	}

	return 0;
}

void do_echo(int fd)
{
#define LEN 4096
	static char buff[LEN];
	int len;
	while(1) {
		len = read(fd, buff, LEN);
		buff[len] = '\0';
		CHECK_FAILED((-1 == len), "read failed, ");
		printf("[%d] recv(%d) %s\n", getpid(), len, buff);
		if(len == 0) break;

		strcat(buff, "[ack].");
		len = strlen(buff)+1;
		CHECK_FAILED((write(fd, buff, len) != len), "write faild, ");
		printf("[%d] send(%d) %s\n", getpid(), len, buff);
	}
	printf("[%d] session finished!.\n", getpid());
}
