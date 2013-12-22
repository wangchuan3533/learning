#include <stdio.h>
int main()
{
	int m, n, t;

	while (scanf("%d%d", &m, &n) == 2) {
		printf("%10d%10d    ", m, n);
		m %= n;
		while (m) {
			t = n % m;
			n = m;
			m = t;
		}

		if (n == 1) {
			printf("Good Choice\n\n");
		}
		else
			printf("Bad Choice\n\n");
	}
	return 0;
}

