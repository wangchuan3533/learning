#include <stdio.h>
#include <string.h>
#define MOD (60)
char minutes[MOD];
int main()
{
	int i, d, s;
	int r;
	while (1) {
		scanf("%d %d", &s, &d);
		if (s == 0)
			if (d == 0)
				break;
		else {
			printf("%d\n", 0);
			continue;
		}
		d = (d + 1) % MOD;
		memset(minutes, 0, sizeof(minutes));
		for (i = 1; i <= MOD; i++) {
			r = (s * d) % MOD;
			if (!r) {
				printf("%d\n", i);
				break;
			}
			if (minutes[r]) {
				printf("Impossible\n");
				break;
			} else {
				minutes[r] = 1;
				s = r;
			}
		}

	}
	return 0;
}