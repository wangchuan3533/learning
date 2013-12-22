#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max_num (3600)
struct node {
	int index;
	struct node *next;
};

int primes[max_num];
int primes2[max_num];
struct node nodes[max_num];

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
	int i, j, m, n;
	struct node *tmp1, *tmp2;
	find_prime();
	for (i = 0; i < max_num; i++)
		nodes[i].index = i + 1;
	while (scanf("%d", &n) == 1) {
		if (!n)
			break;
		for (i = 0; i < n; i++) {
			nodes[i].next = nodes + i + 1;
		}
		nodes[n - 1].next = nodes;
		tmp2 = nodes;
		for (i = 0; i < n - 1; i++) {
			m = (primes[i] - 1) % (n - i);
			for (j = 0; j < m; j++) {
				tmp1 = tmp2;
				tmp2 = tmp2->next;
			}
			tmp1->next = tmp2->next;
			tmp2 = tmp2->next;
		}
		printf("%d\n", tmp2->index);
	}
	return 0;
}
