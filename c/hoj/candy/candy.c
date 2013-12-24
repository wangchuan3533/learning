#include <stdio.h>
int main()
{
	int packet, sum, ave_old, ave_new, diff_high, diff_low;
	int n, i, j;

	while (scanf("%d", &n) == 1 && n >= 0) {
		sum = 0;
		ave_old = 0;
		diff_high = 0;
		diff_low = 0;
		for (i = 0; i < n; i++) {
			scanf("%d", &packet);
			sum += packet;
			ave_new = sum / (i + 1);
			diff_high += i * (ave_old - ave_new);
			diff_low += i * (ave_new - ave_old);
			if (packet > ave_new)
				diff_high += packet - ave_new;
			else
				diff_low += ave_new - packet;
			ave_old = ave_new;
			printf("%d ", ave_old);
		}
		printf("\n%d %d\n", diff_high, diff_low);
		if (diff_high == diff_low)
			printf("%d\n", diff_high);
		else
			printf("-1\n");
	}
	return 0;
}
