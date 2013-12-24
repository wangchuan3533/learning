#include <stdio.h>
int main()
{
	int m, n;
	while (scanf("%d%d", &m, &n) == 2 && m)
		printf("%d\n", 3 * m - n);
	return 0;
}
