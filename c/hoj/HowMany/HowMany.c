#include <stdio.h>
#include <string.h>
#define MAX_M (10000)
char remainders[MAX_M];
char line[32];
int main()
{
	int r;
	int N, M, K;
	while (scanf("%d %d", &N, &M) == 2) {
		memset(remainders, 0, M * sizeof(char));
		r = N;
		for (K = 1; K <= M; K++) {
			r %= M;
			if (!r) {
				printf("%d\n", K);
				break;
			}
			if (remainders[r]) {
				printf("%d\n", 0);
				break;
			}
			remainders[r] = 1;
			r = r * 10 + N;
		}
	}
	return 0;
}
