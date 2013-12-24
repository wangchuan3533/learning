#include <stdio.h>
#include <stdlib.h>
int my_cmp(const void *a, const void *b)
{
	return *((int *)a) - *((int *)b);
}
int main()
{
	int k, i, j, first_case = 1;
	int index[13], map[13];

	while (scanf("%d", &k) && k) {
		if (!first_case)
			printf("\n");
		else
			first_case = 0;
		for (i = 0; i < k; i++)
			scanf("%d", map + i);
		qsort(map, k, sizeof(int), my_cmp);
		for (i = 0; i < 6; i++)
			index[i] = i;
		while (1) {
			printf("%d %d %d %d %d %d\n", map[index[0]],
					map[index[1]], map[index[2]],
					map[index[3]], map[index[4]],
					map[index[5]]);
			for (i = 5; i >= 0; i--)
				if (index[i] < i + k - 6)
					break;
			if (i < 0)
				break;
			index[i]++;
			for (j = i + 1; j < 6; j++)
				index[j] = index[j - 1] + 1;
		}
	}
	return 0;
}
