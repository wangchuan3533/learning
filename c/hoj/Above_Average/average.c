#include <stdio.h>
#include <string.h>
#define MAX_C (1000)

int main()
{
	int N, C, grades[MAX_C], sum, count;
	int i, j;
	float average, percentage;
	scanf("%d", &N);
	for (i = 0; i < N; i++) {
		sum = 0;
		scanf("%d", &C);
		for (j = 0; j < C; j++) {
			scanf("%d", grades + j);
			sum += grades[j];
		}
		average = (float)sum / (float)C;
		count = 0;
		for (j = 0; j < C; j++)
			if (grades[j] > average)
				count++;
		percentage = (float)count / (float)C * 100.0;
		printf("%.3f%%\n", percentage);
	}
	return 0;
}

