#include <stdio.h>
#include <string.h>
#define MAX_N (1001)
#define MAX_LEN (310)
int main()
{
	int i, j, n, len, c, sum;
	char results[MAX_N][MAX_LEN];
	
	sprintf(results[0], "0");
	sprintf(results[1], "0");
	for (i = 2; i < MAX_N; i++) {
		len = strlen(results[i - 1]);
		c = 0;
		for (j = 0; j < len; j++) {
			sum = results[i - 1][j] - '0' + results[i - 1][j] + c;
			if (sum > '9') {
				sum -= 10;
				c = 1;
			} else
				c = 0;
			results[i][j] = sum;
		}
		if (c) {
			results[i][j++] = '1';
			results[i][j] = '\0';
		} else
			results[i][j] = '\0';
		if (i % 2) {
			for (j = 0; results[i][j] == '0'; j++)
				results[i][j] = '9';
			results[i][j] -= 1;
			if (results[i][j] == '0' && results[i][j + 1] == '\0')
				results[i][j] = '\0';
		}
		else {
			for (j = 0; results[i][j] == '9'; j++)
				results[i][j] = '0';
			if (results[i][j] == '\0') {
				results[i][j++] = '1';
				results[i][j] = '\0';
			} else
				results[i][j] += 1;
		}
	}
	while (scanf("%d", &n) == 1) {
		for (i = strlen(results[n]) - 1; i >= 0; i--)
			printf("%c", results[n][i]);
		printf("\n");
	}
	return 0;
}

