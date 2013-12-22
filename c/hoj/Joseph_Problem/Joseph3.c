#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max_num (3502)

int primes[max_num];
int primes2[max_num];
int results[max_num];
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
	int i, j, n;
	find_prime();
	while (scanf("%d", &n) == 1) {
		if (!n)
			break;
		if(!results[n]) {
			for (i = 1, results[n] = 0; i <= n; i++)
				results[n] = (results[n] + primes[n - i]) % i;
		}
		printf("%d\n", results[n] + 1);
	}
	return 0;
}
