#include <stdio.h>
int main()
{
	int i, N, K;
	scanf("%d", &N);
	for (i = 0; i < N; i++) {
		scanf("%d", &K);
		if (K % 5 == 0) K++;
		K -= (K - K % 5) / 5;
		printf("%d\n", K);
	}
	return 0;
}
