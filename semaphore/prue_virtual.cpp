#include <stdio.h>

class Foo
{
public:
	virtual void show() = 0;
};

class Doo : public Foo
{
	void show()
	{
		printf("Doo::show()\n");
	}
};

void Foo::show()
{
	printf("Foo::show()\n");
}

// test

int main(int argc, char **argv)
{
	// Foo f; //
	// Foo *p = new Foo(); //

	Foo *pf = new Doo();
	pf->show();
	return 0;
}
