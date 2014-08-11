#include <set>
#include <unordered_set>
#include <iostream>

#include <stdlib.h>
#include <time.h>

const int TESTN = 12;
int ia[] = {5, 6, 7, 8, 1, 2, 3, 4};

int main(int argc, char** argv)
{
	std::unordered_set<int> uset;
	std::set<int> iset;

	srand(time(NULL));

	for(int i=0; i<TESTN; ++i) {
		int val = rand();
		uset.insert(val);
		iset.insert(val);
	}

	std::cout << "unordered_set:\n";
	for(auto i: uset) {
		std::cout << i << "\n";
	}

	std::cout << "\nset:\n";
	for(auto i: iset) {
		std::cout << i << "\n";
	}

#if 0
	int icount = 1;
	for(auto& i: iset) {
		i = icount++;
	}

	for(auto i: iset) {
		std::cout << i << "\n";
	}
#endif
	return 0;
}
