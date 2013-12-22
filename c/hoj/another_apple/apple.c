#include <stdio.h>
int main()
{
	int t, n, sum, num, i;
	scanf("%d", &t);
	while (t--) {
		scanf("%d", &n);
		sum = 0;
		for (i = 0; i < n; i++) {
			scanf("%d", &num);
			sum += num;
		}
		if (sum % n == 0)
			printf("YES\n");
		else
			printf("NO\n");
	}
	return 0;
}
