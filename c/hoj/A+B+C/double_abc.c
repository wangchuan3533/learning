#include <stdio.h>
int main()
{
	int a, b, c;
	double sum;
	while (scanf("%d %d %d", &a, &b, &c) == 3) {
		sum = (double)a + (double)b + (double)c;
		printf("%.0lf\n", sum);
	}
	return 0;
}