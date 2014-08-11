#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int i;
	unsigned char c;

	printf("Hello, world!\n\b");
	printf("xxx\n");

	for(c=0; c<128; c++)
	{
		printf("%d: ", c);
		for(i=0; i<5; i++)
		{
			printf("%c", c);
			usleep(100);
		}
		printf("\n");	
	}
	return 0;
}
