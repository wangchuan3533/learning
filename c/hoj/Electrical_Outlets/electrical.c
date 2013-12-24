#include <stdio.h>

int main()
{
	int n, k, sum, num;

	scanf("%d", &n);
	while (n--) {
		scanf("%d", &k);
		sum = 1;
		while (k--) {
			scanf("%d", &num);
			sum += (num - 1);
		}
		printf("%d\n", sum);
	}
	return 0;
}

