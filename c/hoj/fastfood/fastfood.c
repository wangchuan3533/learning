#include <stdio.h>
#include <string.h>
#define MAX_N (200)
#define MAX_K (30)
int distance[MAX_N][MAX_N];
int min_cost[MAX_K][MAX_N];
int positions[MAX_N];
int n, k; 
int calc_distance(int x, int y);
int calc_cost();
int main()
{
	int i, count, cur_cost;
	count = 1;
	while (1) {
		scanf("%d %d", &n, &k);
		if (n == 0) break;
		for (i = 0; i < n; i++)
			scanf("%d", positions + i);
		memset(&(distance[0][0]), 0xff, sizeof(distance));
		memset(&(min_cost[0][0]), 0xff, sizeof(min_cost));
		calc_cost();
		cur_cost = ~(1 << 31);
		for (i = 0; i <= n - k; i++)
			if (min_cost[k - 1][i] < cur_cost)
				cur_cost = min_cost[k - 1][i];
		printf("Chain %d\n", count++);
		printf("Total distance sum = %d\n\n", cur_cost);

	}
	return 0;

}
int calc_distance(int x, int y)
{
	int i, sum = 0;
	if (distance[x][y] != -1)
		return distance[x][y];
	for (i = x + 1; i < y; i++) {
		if (positions[y] - positions[i] > positions[i] - positions[x])
			sum += (positions[i] - positions[x]);
		else
			sum += (positions[y] - positions[i]);
	}
	distance[x][y] = sum;
	return sum;
}
int calc_cost()
{
	int i, j, m;
	int d, cur_min;
	for (i = 0; i < k; i++) {
		if (i == 0) {
			min_cost[i][n - 1] = 0;
			for (j = n - 2; j >= k - 1; j--)
				min_cost[i][j] = \
				min_cost[i][j + 1] + (n - 1 - j) *\
				(positions[j + 1] - positions[j]);
			continue;
		}
		for (j = n - 1 - i; j >= k - 1 - i; j--) {
			cur_min = ~(1 << 31);
			for (m = j + 1; m <= n - i; m++) {
				d = calc_distance(j, m) + min_cost[i - 1][m];
				if (d < cur_min)
					cur_min = d;
			}
			min_cost[i][j] = cur_min;
		}
	}
	for (i = 0; i <= n - k; i++)
		for (j = 0; j < i; j++)
			min_cost[k - 1][i] += (positions[i] - positions[j]);
	return 0;
}
