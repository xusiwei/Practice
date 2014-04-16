#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

jmp_buf jb;
int global;

#define SHOW(fmt, expr) printf(#expr ": " fmt, expr)

int main()
{
	int aval;
	register int rval;
	volatile int vval;
	static int sval;

	global = 10;
	aval = 20;
	rval = 30;
	vval = 40;
	sval = 50;

	int ret = setjmp(jb);

	SHOW("%d\n", ret);
	SHOW("%d\n", global);
	SHOW("%d\n", aval);
	SHOW("%d\n", rval);
	SHOW("%d\n", vval);
	SHOW("%d\n", sval);

	if( ret ) {
		exit(0);
	}

	global++;
	aval++;
	rval++;
	vval++;
	sval++;
	
	longjmp(jb, ret++);
	return 0;
}
