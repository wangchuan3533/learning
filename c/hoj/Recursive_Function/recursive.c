#include <stdio.h>

int main()
{
	int i, a, b, c, n, m, f;

	while (scanf("%d%d", &n, &m) && m) {
		if (n < 4) {
			while (n < 0)
				n += m;
			printf("%d\n", (n % m));
			continue;
		}
		a = 1;
		b = 2;
		c = 3;
		for (i = 4; i <= n; i++) {
			f = (a + c) % m;
			a = b;
			b = c;
			c = f;
		}
		printf("%d\n", f);
	}
	return 0;
}

