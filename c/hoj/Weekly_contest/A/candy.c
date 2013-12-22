#include <stdio.h>
int main()
{
	int i, n, candies[1024 * 128], average, sum, count;

	while (scanf("%d", &n) && n >= 0) {
		if (!n) {
			printf("0\n");
			continue;
		}
		for (i = 0, sum = 0; i < n; i++) {
			scanf("%d", candies + i);
			sum += candies[i];
		}
		if (sum % n) {
			printf("-1\n");
			continue;
		}
		average = sum / n;
		for (i = 0, count = 0; i < n; i++)
			if (candies[i] < average)
				count += (average - candies[i]);
		printf("%d\n", count);
	}
	return 0;
}

