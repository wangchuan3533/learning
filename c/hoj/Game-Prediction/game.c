#include <stdio.h>
#include <stdlib.h>
#define MAX_N (50)
int my_cmp(const void *a, const void *b)
{
	return *(int *)b - *(int *)a;
}
int main()
{
	int m, n, i, t = 0;
	int cards[MAX_N];
	int max_min;
	int count;

	while ((scanf("%d %d", &m, &n) == 2) && m) {
		for (i = 0; i < n; i++)
			scanf("%d", cards + i);
		qsort(cards, n, sizeof(int), my_cmp);
		max_min = 0;
		count = m * n - cards[0];
		if (count == 0)
			max_min = 1;
		for (i = 1; i < n; i++) {
			count += cards[i - 1] - cards[i] - 1;
			if (i + 1 - count > max_min)
				max_min = i + 1 - count;
		}
		printf("Case %d: %d\n", ++t, max_min);
	}
	return 0;
}
