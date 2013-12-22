#include <stdio.h>
int main()
{
	int n, s, d;

	scanf("%d", &n);
	while (n--) {
		scanf("%d%d", &s, &d);
		if (s < d) {
			printf("impossible\n");
			continue;
		} else if (s == d) {
			printf("%d 0\n", s);
			continue;
		}
		if ((s + d) % 2 == 0)
			printf("%d %d\n", (s + d) >> 1, (s - d) >> 1);
		else
			printf("impossible\n");
	}
	return 0;
}

