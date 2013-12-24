#include <stdio.h>
#include <string.h>
#define MAX_N (101)
#define INFINITY (0x0f0f0f0f)
int main()
{
	int tbl[MAX_N][MAX_N];
	int i, j, k, n, num, idx, time, min_id, min_time;

	while (scanf("%d", &n) == 1 && n) {
		memset(tbl, 0x0f, sizeof(tbl));
		for (i = 1; i <= n; i++) {
			tbl[i][i] = 0;
			scanf("%d", &num);
			for (j = 0; j < num; j++) {
				scanf("%d %d", &idx, &time);
				tbl[i][idx] = time;
			}
		}
		for (k = 1; k <= n; k++)
			for (i = 1; i <= n; i++)
				for (j = 1; j <= n; j++)
					if (tbl[i][j] > tbl[i][k] + tbl[k][j])
						tbl[i][j] = tbl[i][k] +
								tbl[k][j];
		min_time = INFINITY;
		min_id = 0;
		for (i = 1; i <= n; i++) {
			tbl[i][0] = 0;
			for (j = 1; j <= n; j++)
				if (tbl[i][j] > tbl[i][0])
					tbl[i][0] = tbl[i][j];
			if (tbl[i][0] < min_time) {
				min_time = tbl[i][0];
				min_id = i;
			}
		}
		if (min_id == 0)
			printf("disjoint\n");
		else
			printf("%d %d\n", min_id, min_time);
	}
	return 0;
}

