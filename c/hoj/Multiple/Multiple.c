#include <stdio.h>
int main()
{
	int i, j, N, M;
	int digits[10];
	while (scanf("%d", &N) == 1) {
		scanf("%d", &M);
		for (i = 0; i < 10; i++)
			digits[i] = 0;
		for (i = 0; i < M; i++) {
			scanf("%d", &j);
			digits[j] = 1;
		}
	}
	return 0;
}
