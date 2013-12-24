#include <stdio.h>

int main()
{
	int i, sum, n, s, t, t0;

	while (scanf("%d", &n) && n > 0) {
		t0 = 0;
		sum = 0;
		for (i = 0; i < n; i++) {
			scanf("%d%d", &s, &t);
			sum += s * (t - t0);
			t0 = t;
		}
		printf("%d miles\n", sum);
	}
	return 0;
}

