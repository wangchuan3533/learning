#include <stdio.h>
int main()
{
	int i, n, max, sum, bet;

	while (scanf("%d", &n) && n) {
		max = 0;
		sum = 0;
		for (i = 0; i < n; i++) {
			scanf("%d", &bet);
			sum += bet;
			if (sum > max)
				max = sum;
			else if (sum < 0)
				sum = 0;
		}
		if (max)
			printf("The maximum winning streak is %d.\n", max);
		else
			printf("Losing steak\n");
	}
	return 0;
}

