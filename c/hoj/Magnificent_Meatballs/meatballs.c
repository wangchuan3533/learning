#include <stdio.h>
#define MAX_N (30)
int main()
{
	int i, j, N, meatballs[MAX_N], sum, half;
	while (scanf("%d", &N) && N) {
		sum = 0;
		for (i = 0; i < N; i++) {
			scanf("%d", meatballs + i);
			sum += meatballs[i];
		}
		if (sum % 2) {
			printf("No equal partitioning.\n");
			continue;
		}
		half = sum / 2;
		for (i = 0, sum = 0; sum < half; i++)
			sum += meatballs[i];
		if (sum == half)
			printf("Sam stops at position %d and "
				"Ella stops at position %d.\n", i, i + 1);
		else
			printf("No equal partitioning.\n");
	}
	return 0;
}

