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
int num_node, C, R, num_leaves;
int dist[MAX_C * MAX_R];
int leaves[MAX_C * MAX_R];
int max_dist;
int main()
{
	int T, i, j, k, cur_dist;
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
		num_leaves = 0;
		dfs(i, j, -1);
		for (i = 0; i < num_leaves; i++) {
			for (j = tree[leaves[i]].father, cur_dist = 1;
					j != -1; j = tree[j].father)
				dist[j] = cur_dist++;
			if (dist[0] > max_dist)
				max_dist = dist[0];
			for (j = i + 1; j < num_leaves; j++) {
				for (k = leaves[j], cur_dist = 0; dist[k] == 0; k = tree[k].father)
					cur_dist++;
				cur_dist += dist[k];
				if (cur_dist > max_dist)
					max_dist = cur_dist;
			}
			for (j = tree[leaves[i]].father;
					j != -1; j = tree[j].father)
				dist[j] = 0;
			
		}
		printf("Maximum rope length is %d.\n", max_dist);
	}
	return 0;
}

int dfs(int row, int col, int father)
{
	int flag_leaf = 1, index = num_node++;
	tree[index].row = row;
	tree[index].col = col;
	tree[index].father = father;
	labyrinth[row][col] = 'v';
	if (row - 1 > 0 && labyrinth[row - 1][col] == '.') {
		dfs(row - 1, col, index);
		flag_leaf = 0;
	}
	if (row + 1 < R && labyrinth[row + 1][col] == '.') {
		dfs(row + 1, col, index);
		flag_leaf = 0;
	}
	if (col - 1 > 0 && labyrinth[row][col - 1] == '.') {
		dfs(row, col - 1, index);
		flag_leaf = 0;
	}
	if (col + 1 < C && labyrinth[row][col + 1] == '.') {
		dfs(row, col + 1, index);
		flag_leaf = 0;
	}
	if (flag_leaf) {
		leaves[num_leaves++] = index;
		labyrinth[row][col] = 'l';
	}
	return 0;
}

