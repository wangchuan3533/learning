#include <stdio.h>
int main()
{
	int i, N, K, sum;
	scanf("%d", &N);
	for (i = 0; i < N; i++) {
		scanf("%d", &K);
		sum = K - K / 6;
		sum -= sum / 25;
		while (calc(sum) >= K) sum--;
		printf("%d\n", sum + 1);
	}
	return 0;
}

int calc(int x)
{
	int m, n, y;
	y = x;
	m = x / 5;
	n = x % 5;
	while (m) {
		y += m;
		x = m + n;
		m = x / 5;
		n = x % 5;
	}
	return y;
}
