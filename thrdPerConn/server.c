#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LISTEN_MAX  5

#define TRACE(fmt, expr) printf(#expr ": " fmt, expr)
#define CHECK_FAILED(expr, msg) \
	if(expr) { \
		fprintf(stderr, msg "detail: %s\n", strerror(errno)); \
		exit(errno); \
	}

int server_port = 0;

pid_t gettid()
{
	return syscall(SYS_gettid);
}

struct client_info
{
	struct sockaddr_in end;
	int connfd;
};

void* serverRoutine(void* args)
{
	struct client_info* pinfo = NULL; 
#define LEN 4096
	char buff[LEN];
	int len;

	pinfo = (struct client_info*)args;

	printf("IO thread %d\n", gettid());
	while(1) {
		len = read(pinfo->connfd, buff, LEN);
		buff[len] = '\0';
		CHECK_FAILED((-1 == len), "read failed, ");
		printf("[%d] recv(%d) %s\n", gettid(), len, buff);
		if(len == 0) break;

		strcat(buff, "[ack].");
		len = strlen(buff)+1;
		CHECK_FAILED((write(pinfo->connfd, buff, len) != len), "write faild, ");
		printf("[%d] send(%d) %s\n", gettid(), len, buff);
	}
	printf("[%d] session finished on %d.\n", gettid(), pinfo->connfd);

	close(pinfo->connfd);
	free(pinfo);

	return NULL;
}

int main(int argc, char **argv)
{
	int sock, ret;
	struct sockaddr_in addr;

	if(argc < 2)
	{
		printf("usage: %s port\n", argv[0]);
		exit(-1);
	}

	server_port = atol(argv[1]);
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	CHECK_FAILED((sock < 0), "socket() failed!");
	TRACE("sock: %d\n", sock);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(server_port);

	ret = bind(sock, (const struct sockaddr*)&addr, sizeof(addr));
	CHECK_FAILED((ret < 0), "bind() failed!");
	TRACE("bind: %d\n", ret);

	ret = listen(sock, LISTEN_MAX);
	CHECK_FAILED((ret < 0), "listen() failed!");
	TRACE("listen: %d\n", ret);

	for(;;) 
	{
		pthread_t thread;
		int addr_len = 0;
		struct client_info* info = NULL;

		info = (struct client_info*)malloc(sizeof(struct client_info));
		memset(info, 0, sizeof(struct sockaddr_in));
		
		info->connfd = accept(sock, (struct sockaddr*)&info->end, &addr_len);
		CHECK_FAILED((info->connfd < 0), "accept() error!");
		TRACE("[main] accept: %d\n", info->connfd);
//		TRACE("%d\n", addr_len);
//		TRACE("%d\n", sizeof(struct sockaddr_in));

		pthread_create(&thread, NULL, serverRoutine, info);
	}

	return 0;
}


