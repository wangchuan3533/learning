#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max_num (14)
int results[max_num];
int main()
{
	int i, k, m, n, start, end;
	for (n = 1; n < max_num; n++) {
		for (m = n + 1;; m++) {
			start = 0;
			end = n - 1;
			for (i = 2 * n; i > n; i--) {
				k = (m - 1) % i;
				if (k >= start && k <= end)
					break;
				start = (((start - m) % i) + i) % i;
				end = (((end - m) % i) + i) % i;
			}
			if (i == n) {
				results[n] = m;
				break;
			}
		}
	}
	while (scanf("%d", &k) == 1 && k) {
		printf("%d\n", results[k]);
	}
	return 0;
}
