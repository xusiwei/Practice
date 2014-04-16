#include <stdio.h>
#include <unistd.h>

int global = 6;
char buf[] = "a write to stdout!\n";

int main()
{
	int var;
	pid_t pid;

	var = 88;

	if(write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1)
		fprintf(stderr, "write error!");
	printf("before fork\n");

	if( (pid = fork()) < 0 ) {
		fprintf(stderr, "fork error!\n");
	}else if(pid == 0) { // child
		printf("[child] ");
		global++;
		var++;
	}else { // parent
		printf("[parent] ");
		sleep(2);
	}
	printf("pid: %d, ppid: %d, global: %d, var: %d\n", getpid(), getppid(), global, var);
	return 0;	
}


