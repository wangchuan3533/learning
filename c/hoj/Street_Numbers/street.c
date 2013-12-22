#include <stdio.h>

int main()
{
	int x = 3, y = 1;
	int x1, y1;
	int i;
	for (i = 0; i < 10; i++) {
		x1 = 3 * x + 8 * y;
		y1 = x + 3 * y;
		printf("%10d%10d\n", y1, (x1 - 1) >> 1);
		x = x1;
		y = y1;
	}
	return 0;
}

