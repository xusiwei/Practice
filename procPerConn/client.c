#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ECHO_MSG "hello, server!"

void send_echo(int);

#define CHECK_FAILED(expr, msg) \
	if(expr) { \
		fprintf(stderr, msg "detail: %s\n", strerror(errno)); \
		exit(errno); \
	}

#define TRACE(fmt, expr) printf(#expr ": " fmt, expr)

const char* server_host = NULL;
int server_port = 0;

int main(int argc, char **argv)
{
	int sock, ret; 
	int connfd;
	int socklen;

	struct sockaddr_in server_addr, client_addr;

	if(argc < 3) {
		printf("usage: %s host port\n", argv[0]);
		exit(-1);
	}

	server_host = argv[1];
	server_port = atol(argv[2]);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_FAILED((-1 == sock), "socket error, ");
	TRACE("%d\n", sock);

	// memset(&addr, 0, sizeof(struct sockaddr_in));
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	inet_pton(AF_INET, server_host, &server_addr.sin_addr);

	ret = connect(sock, (const struct sockaddr*)&server_addr, sizeof(server_addr));
	CHECK_FAILED((-1 == ret), "connect error, ");
	TRACE("(connet): %d\n", ret);

	send_echo(sock);

	return 0;
}

void send_echo(int fd)
{
//	char *msg = ECHO_MSG;
#define LEN 4096
	static char msg[LEN];
	int len = strlen(msg)+1;

	while(1) {
		printf("input: ");
		fgets(msg, LEN, stdin); // will get line-end-char

		len = strlen(msg);
		msg[len-1] = '\0';
		CHECK_FAILED((write(fd, msg, len) != len), "write faild, ");
		printf("[c %d] send(%d) %s\n", getpid(), len, msg);

		len = read(fd, msg, LEN);
		CHECK_FAILED((len <= 0), "read error, ");
		printf("[c %d] recv(%d) %s\n", getpid(), len, msg);
	}
}
