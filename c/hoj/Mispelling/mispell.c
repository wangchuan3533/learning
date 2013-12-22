#include <stdio.h>
#include <string.h>
int main()
{
	int n, i, j, k;
	char buf[128];

	scanf("%d", &n);
	for (i = 0; i < n; i++) {
		scanf("%d %s", &j, buf);
		k = strlen(buf);
		buf[j - 1] = '\0';
		printf("%d %s%s\n", i + 1, buf, buf + j);
	}
	return 0;
}
