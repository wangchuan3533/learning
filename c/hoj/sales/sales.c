#include <stdio.h>
#include <stdlib.h>
int cmp(const void *a, const void *b);
int main()
{
	int i, j, n;
	int customer[1000], sum_a, sum_b, time_a, time_b;
	float average;
	
	while (scanf("%d", &n) == 1) {
		for (i = 0; i < n; i++)
			scanf("%d", customer + i);
		qsort(customer, n, sizeof(int), cmp);
		sum_a = sum_b = 0;
		time_a = time_b = 0;
		for (i = 0; i < n; i++) {
			if (sum_b >= sum_a) {
				time_a += sum_a;
				sum_a += customer[i];
			} else {
				time_b += sum_b;
				sum_b += customer[i];
			}
		}
		time_a += time_b;
		average = (float)time_a / (float)n;
		printf("%.3f\n", average);
	}
	return 0;
}

int cmp(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}
