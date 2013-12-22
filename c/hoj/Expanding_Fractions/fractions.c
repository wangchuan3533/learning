#include <stdio.h>
#include <string.h>
int main()
{
	int i, j;
	int num, den, repeat;
	char fractions[1000];
	int hash[1000];

	fractions[0] = '.';
	while (scanf("%d%d", &num, &den) && num) {
		memset(hash, 0x00, den * sizeof(int));
		for (i = 1;; i++) {
			hash[num] = i;
			num *= 10;
			fractions[i] = num / den + '0';
			num %= den;
			if (hash[num] || num == 0)
				break;
		}
		for (j = 0; j <= i; j++)
			if((j + 1) % 50)
				printf("%c", fractions[j]);
			else
				printf("%c\n", fractions[j]);
		if ((i + 1) % 50)
			printf("\n");
		if (num) {
			repeat = i - hash[num] + 1;
			printf("The last %d digits repeat forever.\n", repeat);
		} else
			printf("This expansion terminates.\n");
	}
	return 0;
}

