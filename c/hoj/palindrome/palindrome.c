#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max_num 33333
#define num_primes 4000
#define num_digits 16
int *num;
int *num2;
int find_prime()
{
	int i, j, n;
	num[0] = 2;
	num2[0] = 4;
	num[1] = 3;
	num2[1] = 9;
	n = 2;
	for(i = 5; i < max_num; i+=2) {
		for (j = 1; num2[j] <= i; j++)
			if (i % num[j] == 0)
				break;
		if (num2[j] > i) {
			num[n] = i;
			num2[n++] = i * i;
		}
	}
	return n;
}
int check_prime(int number, int n)
{
	int i;
	for (i = 0; i <= n && num2[i] <= number; i++)
		if (number % num[i] == 0)
			break;
	if (num2[i] > number)
		return 1;
	else
		return 0;
}
int main()
{
	int i, j, k, n, a, b, index, num_inner, max_inner, value_decimal;
	char str_a[num_digits], str_b[num_digits], palindrome[num_digits], tmp[num_digits], fmt[8];
	int len_a, len_b;
	char digit_ends[] = { '1', '3', '7', '9'};
	num = (int *)malloc(num_primes * sizeof(int));
	num2 = (int *)malloc(num_primes * sizeof(int));
	n = find_prime();
	scanf("%d %d", &a, &b);
	sprintf(str_a, "%d", a);
	sprintf(str_b, "%d", b);
	len_a = strlen(str_a);
	len_b = strlen(str_b);
	for (i = len_a; i <= len_b; i++) {
		if (i == 1) {
			for (j = 5; j <= 9; j+=2) {
				if (j < a || j > b)
					continue;
				if (check_prime(j, n))
					printf("%d\n", j);
			}
			continue;
		}
		if (i == 2) {
			for (j = 11; j <= 99; j+=22) {
				if (j < a || j > b)
					continue;
				if (check_prime(j, n))
					printf("%d\n", j);
			}
			continue;
		}
		if(i % 2 == 1) {
			num_inner = i / 2;
			max_inner = 1;
			sprintf(fmt, "%%0%dd\n", num_inner);
			for (j = 0; j < num_inner; j++)
				max_inner *= 10;
			for (index = 0; index < 4; index++) {
				palindrome[0] = digit_ends[index];
				palindrome[i - 1] = digit_ends[index];
				palindrome[i] = '\0';
				for (j = 0; j < max_inner; j++) {
					sprintf(tmp, fmt, j);
					for (k = 1; k <= i / 2; k++) {
						palindrome[k] = tmp[k - 1];
						palindrome[i - 1 - k] = palindrome[k];
					}
					sscanf(palindrome, "%d", &value_decimal);
					if (value_decimal < a)
						continue;
					if (value_decimal > b)
						break;
					if (check_prime(value_decimal, n))
						printf("%d\n", value_decimal);
				}
			}
			continue;
		}
		if ( i % 2 == 0)
			continue;
	}
	free(num);
	free(num2);
	return 0;
}