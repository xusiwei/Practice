#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string> // for std::string

#define MAXN 1024
char buff[MAXN];

int main(int argc, char *argv[])
{
	int len = 0;

	gets(buff);
	printf("gets len: %d\n", strlen(buff));

	fgets(buff, MAXN, stdin);
	printf("fgets len: %d\n", strlen(buff));

	std::string line;
	std::getline(std::cin, line);
	printf("getline len: %d\n", line.length());

	return 0;
}
