#include <stdio.h>
#include <string.h>
#define MAX_C (1000)
#define MAX_R (1000)
#define swap(a, b) \
do {\
	a ^= b;\
	b ^= a;\
	a ^= b;\
} while(0)

struct node {
	int father;
	int depth;
};
char labyrinth[MAX_R][MAX_C];
struct node tree[MAX_C * MAX_R];
int num_node, max_dist, C, R;
void dfs(int row, int col, int father);

int main()
{
	int T, i, j;
	scanf("%d", &T);
	while (T--) {
		scanf("%d %d", &C, &R);
		for (i = 0; i < R; i++)
			scanf("%s", labyrinth[i]); 
		for (i = 0; i < R; i++) {
			for (j = 0; j < C; j++)
				if (labyrinth[i][j] == '.')
					goto break_out;
		}
break_out:
		max_dist = 0;
		num_node = 0;
		dfs(i, j, -1);
		printf("Maximum rope length is %d.\n", max_dist);
	}
	return 0;
}

void dfs(int row, int col, int father)
{
	int index = num_node++;
	int i, j, sub_count = 0, depth[4] = {0, 0, 0, 0};
	tree[index].father = father;
	labyrinth[row][col] = 'v';
	if (row - 1 > 0 && labyrinth[row - 1][col] == '.') {
		dfs(row - 1, col, index);
		depth[sub_count++] = tree[index].depth;
	}
	if (row + 1 < R && labyrinth[row + 1][col] == '.') {
		dfs(row + 1, col, index);
		depth[sub_count++] = tree[index].depth;
	}
	if (col - 1 > 0 && labyrinth[row][col - 1] == '.') {
		dfs(row, col - 1, index);
		depth[sub_count++] = tree[index].depth;
	}
	if (col + 1 < C && labyrinth[row][col + 1] == '.') {
		dfs(row, col + 1, index);
		depth[sub_count++] = tree[index].depth;
	}
	for (i = 0; i < sub_count - 1; i++)
		for (j = sub_count - 1; j > i; j--)
			if (depth[j] > depth[i])
				swap(depth[j], depth[i]);
	tree[tree[index].father].depth = depth[0] + 1;
	if (depth[1]) {
		if (depth[0] + depth[1] > max_dist)
			max_dist = depth[0] + depth[1];
	} else {
		if (depth[0] > max_dist)
			max_dist = depth[0];
	}
}

