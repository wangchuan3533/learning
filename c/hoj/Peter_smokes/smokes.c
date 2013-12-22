#include <stdio.h>
int main()
{
	int n, k;

	while (scanf("%d%d", &n, &k) == 2) {
		k--;
		if (n % k)
			n += n / k;
		else
			n += (n / k) - 1;
		printf("%d\n", n);
	}
	return 0;
}

