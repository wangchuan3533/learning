#include <stdio.h>
#include <string.h>
#define MAX_N (301)
int triangular_sums[MAX_N];

int main()
{
	int i;
	int N, n;
	triangular_sums[0] = 0;
	for (i = 1; i < MAX_N; i++)
		triangular_sums[i] = triangular_sums[i - 1] +
					((i * (i + 1) * (i + 2)) >> 1);
	scanf("%d", &N);
	for (i = 0; i < N; i++) {
		scanf("%d", &n);
		printf("%d %d %d\n", i + 1, n, triangular_sums[n]);
	}
	return 0;
}

