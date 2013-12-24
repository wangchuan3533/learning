#include <stdio.h>
#include <string.h>
int main()
{
	char buf[1024];
	int len, n;

	scanf("%d", &n);
	while (n--) {
		scanf("%s", buf);
		len = strlen(buf);
		while (len)
			printf("%c", buf[--len]);
		printf("\n");
	}
	return 0;
}


