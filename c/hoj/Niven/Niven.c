#include <stdio.h>
#include <string.h>
char num[128];
char sum[10];

int my_ncmp(char const *num1, char const *num2, int len)
{
	int i = 0;
	while (i < len && num1[i] == num2[i])
		i++;
	if (i == len)
		return 0;
	return (num1[i] - num2[i]);
}

int my_substract(char *num1, char *num2, int len, int base)
{
	int i;
	for (i = len - 1; i >= 0; i--) {
		num1[i] -= num2[i];
		if (num1[i] < 0) {
			num1[i] += base;
			num1[i - 1]--;
		}
	}
}

int main()
{
	int i, j, index, n, len, high_sum, base, tmp;
	while (scanf("%d", &base) == 1 && base) {
		scanf("%s", num);
		len = strlen(num);
		high_sum = 0;
		for (i = 0; i < 10; i++)
			sum[i] = 0;
		for (i = 0; i < len; i++) {
			num[i] -= '0';
			sum[0] += num[i];
			for (j = 0; sum[j] >= base; j++) {
				sum[j] -= base;
				sum[j + 1] += 1;
			}
			if (j > high_sum)
				high_sum = j;
		}
		for (i = 0; i <= high_sum / 2; i++) {
			tmp = sum[i];
			sum[i] = sum[high_sum - i];
			sum[high_sum - i] = tmp;
		}
		index = 0;
		while (index < len - 1 - high_sum) {
			while (my_ncmp(num + index, sum, high_sum + 1) >= 0)
				my_substract(num + index, sum, high_sum + 1, base);
			while (num[index] > 0)
				my_substract(num + index + 1, sum, high_sum + 1, base);
			index++;
		}
		while (my_ncmp(num + index, sum, high_sum + 1) >= 0)
			my_substract(num + index, sum, high_sum + 1, base);
		for (i = index; i < len; i++)
			if (num[i])
				break;
		if (i < len)
			printf("no\n");
		else
			printf("yes\n");
	}
	return 0;
}