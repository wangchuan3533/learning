#include <stdio.h>
int main()
{
	int i, tmp, n;

	while (scanf("%d", &n) == 1 && n) {
		tmp = 1;
		if (n <= 1) tmp = 0;
		for (i = 2; i * i <= n; i++) {
			if (n % i == 0) {
				while ( n % i == 0) {
					n /= i;
					tmp *= i;
				}
				tmp /= i;
				tmp *= (i - 1);
			}
		}
		if (n > 1)
			tmp *= (n - 1);
		printf("%d\n", tmp);
	}
	return 0;
}

