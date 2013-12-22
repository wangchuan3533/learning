#include <stdio.h>
int main()
{
	int T, i, n, sum;

	scanf("%d", &T);
	for (i = 0; i < T; i++) {
		scanf("%d", &n);
		sum = 0;
		while (n) {
			n /= 5;
			sum += n;
		}
		printf("%d\n", sum);
	}
	return 0;
}

