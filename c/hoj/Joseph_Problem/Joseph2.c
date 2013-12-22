#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max_num (3600)

int primes[max_num];
int primes2[max_num];
int nodes[max_num];

int find_prime()
{
	int i, j, n;
	primes[0] = 2;
	primes2[0] = 4;
	primes[1] = 3;
	primes2[1] = 9;
	n = 2;
	for(i = 5;; i+=2) {
		for (j = 1; primes2[j] <= i; j++)
			if (i % primes[j] == 0)
				break;
		if (primes2[j] > i) {
			primes[n] = i;
			primes2[n++] = i * i;
			if (n >= max_num)
				break;
		}
	}
	return n;
}

int main()
{
	int i, j, m, n, cur;
	find_prime();
	while (scanf("%d", &n) == 1) {
		if (!n)
			break;
		memset(nodes, 0, n * sizeof(int));
		cur = 0;
		for (i = 0; i < n - 1; i++) {
			m = (primes[i] - 1) % (n - i);
			for (j = 0; j < m; j++) {
				cur = (cur + 1) % n;
				while (nodes[cur])
					cur = (cur + 1) % n;
			}
			nodes[cur] = 1;
			cur = (cur + 1) % n;
			while (nodes[cur])
				cur = (cur + 1) % n;
		}
		printf("%d\n", cur + 1);
	}
	return 0;
}
