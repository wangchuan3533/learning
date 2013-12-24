#include <stdio.h>
#define MAX_R (1000)
short matrix[MAX_R][MAX_R];
int main()
{
	int r, i, j;

	while (scanf("%d", &r) == 1) {
		for (i = 0; i < r; i++)
			for (j = 0; j <= i; j++)
				scanf("%d", &(matrix[i][j]));
		for (i = r - 2; i >= 0; i--)
			for (j = 0; j <= i; j++)
				if (matrix[i + 1][j] > matrix[i + 1][j + 1])
					matrix[i][j] += matrix[i + 1][j];
				else
					matrix[i][j] += matrix[i + 1][j + 1];
		printf("%d\n", matrix[0][0]);
	}
	return 0;
}

