#include <stdio.h>
#include <string.h>
#include <assert.h>
int main()
{
	int i, n, root, len;
	char buf[1024];

	while (scanf("%s", buf) && buf[0] != '0') {
		len = strlen(buf);
		assert(len < 1023);
		for (i = 0, root = 0; i < len; i++)
			root += buf[i] - '0';
		while (root > 9) {
			for (n = 0; root;) {
				n += root % 10;
				root /= 10;
			}
			root = n;
		}
		printf("%d\n", root);
	}
	return 0;
}

