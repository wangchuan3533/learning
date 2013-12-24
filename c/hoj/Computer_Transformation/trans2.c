#include <stdio.h>

int main()
{
	int i, j, count_00[1001], count_1 = 1, count_01 = 1;

	count_00[0] = count_00[1] = 0;
	for (i = 2; i < 20; i++) {
		count_00[i] = count_01;
		count_01 = count_1 + count_00[i - 1];
		count_1 <<= 1;
		printf("%d\n", count_00[i]);
	}
	return 0;
}
