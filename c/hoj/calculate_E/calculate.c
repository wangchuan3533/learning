#include <stdio.h>

int main()
{
	int i, n = 2;
	double e = 2.5;

	printf("n e\n- -----------\n");
	printf("0 1\n1 2\n2 2.5\n");
	for (i = 3; i < 10; i++) {
		n *= i;
		e += 1.0 / (double)n;
		printf("%d %.9lf\n", i, e);
	}
	return 0;
}

