#include <stdio.h>
#include <string.h>
#define MAX_N (200)
#define MAX_K (30)
int z[MAX_N][MAX_N];
int co[MAX_K][MAX_N];
int po[MAX_N];
int n, k; 
int c_z(int x, int y);
int c_c();
int main()
{
	int i, count, cu_c;
	count = 1;
	while (1) {
		scanf("%d %d", &n, &k);
		if (n == 0) break;
		for (i = 0; i < n; i++)
			scanf("%d", po + i);
		memset(&(z[0][0]), 0xff, sizeof(z));
		memset(&(co[0][0]), 0xff, sizeof(co));
		c_c();
		cu_c = ~(1 << 31);
		for (i = 0; i <= n - k; i++)
			if (co[k - 1][i] < cu_c)
				cu_c = co[k - 1][i];
		printf("Chain %d\n", count++);
		printf("Total distance sum = %d\n\n", cu_c);

	}
	return 0;

}
int c_z(int x, int y)
{
	int i, sum = 0;
	if (z[x][y] != -1)
		return z[x][y];
	for (i = x + 1; i < y; i++) {
		if (po[y] - po[i] > po[i] - po[x])
			sum += (po[i] - po[x]);
		else
			sum += (po[y] - po[i]);
	}
	z[x][y] = sum;
	return sum;
}
int c_c()
{
	int i, j, m;
	int d, cu_m;
	for (i = 0; i < k; i++) {
		if (i == 0) {
			co[i][n - 1] = 0;
			for (j = n - 2; j >= k - 1; j--)
				co[i][j] = \
				co[i][j + 1] + (n - 1 - j) *\
				(po[j + 1] - po[j]);
			continue;
		}
		for (j = n - 1 - i; j >= k - 1 - i; j--) {
			cu_m = ~(1 << 31);
			for (m = j + 1; m <= n - i; m++) {
				d = c_z(j, m) + co[i - 1][m];
				if (d < cu_m)
					cu_m = d;
			}
			co[i][j] = cu_m;
		}
	}
	for (i = 0; i <= n - k; i++)
		for (j = 0; j < i; j++)
			co[k - 1][i] += (po[i] - po[j]);
	return 0;
}
