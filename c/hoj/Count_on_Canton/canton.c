#include <stdio.h>

int main()
{
	int n, k;

	while (scanf("%d", &n) == 1) {
		for (k = n >> 1; ((k * (k + 1)) >> 1) > n; 
