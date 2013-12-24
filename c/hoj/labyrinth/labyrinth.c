#include <stdio.h>
#include <string.h>

#define MAX_C (1000)
#define MAX_R (1000)

int dfs(int row, int col, int father);
int calc_dist(int index);

char labyrinth[MAX_R][MAX_C];
struct node {
	int col;
	int row;
	int father;
};
struct node tree[MAX_C * MAX_R];
int num_node, C, R;
int dist[MAX_C * MAX_R];
int max_dist;
int main()
{
	int T, i, j;
	scanf("%d", &T);
	for (;T > 0; T--) {
		scanf("%d %d", &C, &R);
		for (i = 0; i < R; i++)
			scanf("%s", labyrinth[i]); 
		for (i = 0; i < R; i++) {
			for (j = 0; j < C; j++)
				if (labyrinth[i][j] == '.')
					break;
			if (j < C)
				break;
		}
		max_dist = 0;
		num_node = 0;
		dfs(i, j, -1);
		printf("Maximum rope length is %d.\n", max_dist);
	}
	return 0;
}

int dfs(int row, int col, int father)
{
	int i, cur_dist;
	int index = num_node++;
	tree[index].row = row;
	tree[index].col = col;
	tree[index].father = father;
	labyrinth[row][col] = 'v';
	for (i = 0; i < index; i++)
		dist[i] = 0;
	for (i = tree[index].father, cur_dist = 1; i != -1; i = tree[i].father)
		dist[i] = cur_dist++;
	for (i = 0; i < index; i++) {
		cur_dist = calc_dist(i);
		if (cur_dist > max_dist)
			max_dist = cur_dist;
	}
	if (row - 1 > 0 && labyrinth[row - 1][col] == '.')
		dfs(row - 1, col, index);
	if (row + 1 < R && labyrinth[row + 1][col] == '.')
		dfs(row + 1, col, index);
	if (col - 1 > 0 && labyrinth[row][col - 1] == '.')
		dfs(row, col - 1, index);
	if (col + 1 < C && labyrinth[row][col + 1] == '.')
		dfs(row, col + 1, index);
	return 0;
}

int calc_dist(int index)
{
	if (dist[index])
		return dist[index];
	dist[index] = calc_dist(tree[index].father) + 1;
	return dist[index];
}

