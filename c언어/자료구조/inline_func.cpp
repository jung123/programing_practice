#include <stdio.h>

inline void test (int c, int d)
{
	int tmp;
	tmp = c;
	c = d;
	d = tmp;
	printf ("c : %d, d : %d\n", c, d);
}

int main ()
{
	int tmp = 0;
	int a = 0;
	int b = 0;
	test (a, b);
	return 0;
}
