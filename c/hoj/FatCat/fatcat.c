#include <stdio.h>
#include <string.h>
#define MAX_M (100)
#define MAX_N (100)
char matrix[MAX_M][MAX_N];
int m, n;
int calc_step(int x0, int y0, int x, int y)
{
	int step = 0;
	if (x0 > x)
		step += x0 - x;
	else
		step += x - x0;
	if (y0 > y)
		step += y0 - y;
	else
		step += y - y0;
	return step;
}
int main()
{
	int i, j, step;
	int x, y, x0, y0, dx, dy;
	int up, down, left, right, max;
	while (scanf("%d %d", &n, &m) == 2) {
		for (i = 0; i < n; i++)
			for (j = 0; j < m; j++)
				scanf("%d", &(matrix[i][j]));
		scanf("%d %d", &x0, &y0);
		x = 0;
		y = 0;
		step = 0;
		while (1) {
			if (calc_step(x0, y0, x, y) <= step) {
				printf("%d\n", step);
				break;
			}
			if (0 == x)
				left = 0;
			else
				left = matrix[i - 1][j];
			if (0 == y)
				down = 0;
			else
				down = matrix[i][j - 1];
			if (n - 1 == x)
				right = 0;
			else
				right = matrix[i + 1][j];
			if (m - 1 == y)
				up = 0;
			else
				up = matrix[i][j + 1];
			max = 0;
			if (left > max) {
				dx = -1;
				dy = 0;
				max = left;
			}
			if (right > max) {
				dx = 1;
				dy = 0;
				max = right;
			}
			if (down > max) {
				dx = 0;
				dy = -1;
				max = down;
			}
			if (up > max) {
				dx = 0;
				dy = 1;
				max = up;
			}
			if (0 == max) {
				printf("Imposible\n");
				break;
			}
			matrix[x][y] = 0;
			x += dx;
			y += dy;
			step++;
		}
	}	
}
