#include <stdio.h>
int main()
{
	int count;
	int n, value;
	while (scanf("%d", &n) == 1) {
		if (n == 1) {
			printf("%d\n", 1);
			continue;
		}
		value = 1;
		count = 1;
		while (value) {
			value = (value * 10 + 1) % n;
			count++;
		}
		printf("%d\n", count);
	}
	return 0;
}
