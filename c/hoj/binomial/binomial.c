#include <stdio.h>
int main()
{
	int i, j, n, k, prod;
	while (scanf("%d%d", &n, &k) && n) {
		if (n - k < k)
			k = n - k;
		prod = 1;
		for (i = 0; i < k; i++) {
			prod *= n - i;
			prod /= i + 1;
		}
		printf("%d\n", prod);
	}
	return 0;
}
