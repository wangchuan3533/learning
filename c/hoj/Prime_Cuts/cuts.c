#include <stdio.h>
#define MAX_N (1100)
int prime[300], prime2[300];
int n;
int init_prime()
{
	int i, j;
	prime[0] = 1;
	prime2[0] = 1;
	prime[1] = 2;
	prime2[1] = 4;
	n = 2;
	for(i = 3; i < MAX_N; i += 2) {
		for (j = 1; prime2[j] <= i; j++)
			if (i % prime[j] == 0)
				break;
		if (prime2[j] > i) {
			prime[n] = i;
			prime2[n++] = i * i;
		}
	}
	return 0;
}
int main()
{
	int i, j;
	int k, c, count;

	init_prime();
	while (scanf("%d%d", &k, &c) == 2) {
		printf("%d %d:", k, c);
		for (count = 0; prime[count] <= k; count++) ;
		if (!count) {
			printf("\n\n");
			continue;
		}
		if (c > (count >> 1)) {
			for (i = 0; i < count; i++)
				printf(" %d", prime[i]);
			printf("\n\n");
			continue;
		}
		if (count % 2) {
			for (i = (count >> 1) - c + 1, j = 0;
					j < 2 * c - 1; i++, j++)
				printf(" %d", prime[i]);
			printf("\n\n");
			continue;
		}
		for (i = (count >> 1) - c, j = 0;
				j < 2 * c; i++, j++)
			printf(" %d", prime[i]);
		printf("\n\n");
	}
	return 0;
}

