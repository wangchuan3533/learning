#include <stdio.h>
#include <string.h>
#define max_len (1002)
char buf[max_len];
int one_digit[] = {1, 1, 2, 6, 4, 2, 2, 4, 2, 8};
int hash[] = {6, 6, 2, 6, 4, 4, 4, 8, 4, 6};
int calc_factorial(char *num, int start, int end);
int main()
{
	int i, len;
	while (scanf("%s", buf) != EOF) {
		len = strlen(buf);
		for (i = 0; i < len; i++)
			buf[i] -= '0';
		printf("%d\n", calc_factorial(buf, 0, len - 1));
	}
	return 0;
}

int calc_factorial(char *num, int start, int end)
{
	int i, tmp1, tmp2, value;
	if (start == end)
		return one_digit[num[start]];
	value = hash[num[end]];
	for (i = 0, tmp1 = 0; i <= end; i++) {
		tmp2 = (tmp1 * 10 + num[i]) % 5;
		num[i] = (tmp1 * 10 + num[i]) / 5;
		tmp1 = tmp2;
	}
	tmp1 = num[end -1] * 10 + num[end];
	tmp1 %= 4;
	while (tmp1-- > 0)
		value = (value * 8) % 10;
	if (num[start] == 0)
		start++;
	value = (value *calc_factorial(num, start, end)) % 10;
	return value;
}