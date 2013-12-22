#include <stdio.h>
#define MAX_NUM (1000001)
char self[MAX_NUM];
int main()
{
	int i, j, sum;
	i = 1;
	while (i < MAX_NUM) {
		while (self[i]) i++;
		if (i >= MAX_NUM) break;
		printf("%d\n", i);
		for (j = i; j < MAX_NUM;){
			if (self[j]) break;
			self[j] = 1;
			sum = j;
			while (j) {
				sum += (j % 10);
				j /= 10;
			}
			j = sum;
		}
	}
	return 0;
}

