#include <stdio.h>
#include <stdlib.h>
int cmp(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}
int main()
{
	int i, n, num[10];
	scanf("%d", &n);
	while (n--) {
		for (i = 0; i < 10; i++)
			scanf("%d", num + i);
		qsort(num, 10, sizeof(int), cmp);
		for (i = 0; i < 9; i++)
			printf("%d ", num[i]);
		printf("%d\n", num[9]);
	}
	return 0;
}

