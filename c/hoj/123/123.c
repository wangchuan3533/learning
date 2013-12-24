#include <stdio.h>
int main()
{
	int x, y;
	while (scanf("%d%d", &x, &y) != EOF)
		printf("%d\n", 6 - x - y);
	return 0;
}
