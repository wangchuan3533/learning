#include <stdio.h>
#define max_num (1000000000)
int main()
{
	int num[3];
	int high, low, tmp;
	int i;
	while (scanf("%d %d %d", num, num + 1, num + 2) == 3) {
		high = 0;
		low = 0;
		for (i = 0; i < 3; i++) {
			while (num[i] > max_num) {
				num[i] -= max_num;
				high++;
			}
			while (num[i] < (-max_num)) {
				num[i] += max_num;
				high--;
			}
			low += num[i];
			if (low > max_num ) {
				low -= max_num;
				high++;
			}
			if (low < (-max_num)) {
				low += max_num;
				high--;
			}
		}
		if (high == 0) printf("%d\n", low);
		if (high > 0 && low >= 0)
			printf("%d%9d\n", high, low);
		if (high > 0 && low < 0) {
			high--;
			low += max_num;
			if ( high == 0)
				printf("%d\n", low);
			else
				printf("%d%9d\n", high, low);
			continue;
		}
		if (high < 0 && low <= 0) {
			low *= -1;
			printf("%d%9d\n", high, low);
			continue;
		}
		if (high < 0 && low > 0) {
			high++;
			low -= max_num;
			low *= -1;
			if (high == 0)
				printf("-%d", low);
			else
				printf("%d%9d\n", high, low);
			continue;
		}
	}
	return 0;
}
