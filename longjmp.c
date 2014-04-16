#include <stdio.h>
#include <setjmp.h>

int main()
{
	jmp_buf jb;

	int ret = setjmp(jb);
	if(0 == ret)
		printf("first return!\n");
	else
		printf("other return! %d\n", ret);
	
	if(ret > 3) return 0;
	longjmp(jb, ret+1);

	return 0;	
}
