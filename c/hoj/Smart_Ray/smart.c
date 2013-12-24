#include <stdio.h>
#include <string.h>
int main()
{
	int a, b, n, len, prod;
	char buf[128];

	scanf("%d", &n);
	while (n--) {
		scanf("%s", buf);
		len = strlen(buf);
		a = buf[len - 1] - '0';
		scanf("%s", buf);
		len = strlen(buf);
		if (len < 2)
			b = buf[len - 1] - '0';
		else
			b = 10 * (buf[len - 2] - '0') + buf[len - 1] - '0';
		b = ((b + 3) % 4) + 1; 
		for (prod = 1; b > 0; b--)
			prod *= a;
		prod %= 10;
		printf("%d\n", prod);
	}
	return 0;
}
