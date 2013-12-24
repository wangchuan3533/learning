#include <stdio.h>
#include <string.h>
#define MAX_NUM (100)
char graph[MAX_NUM + 1][MAX_NUM + 1];
char indexs[MAX_NUM + 1], low[MAX_NUM + 1],
     father[MAX_NUM + 1], gate[MAX_NUM + 1];
int cur_index;
int n;
int main()
{
	int i, j, N, count;
	char line[512], *str;
	while (scanf("%d", &N) == 1 && N) {
		memset(graph, 0x00, sizeof(graph));
		memset(indexs, 0x00, sizeof(indexs));
		memset(low, 0x00, sizeof(low));
		memset(father, 0x00, sizeof(father));
		memset(gate, 0x00, sizeof(gate));
		while (gets(line)) {
			str = line;
			sscanf(str, "%d", &i);
			if(!i) break;
			while (1) {
				while (str[0] >= '0' && str[0] <= '9') str++;
				if (str[0] == '\0') break;
				sscanf(++str, "%d", &j);
				graph[i][j] = 1;
				graph[j][i] = 1;
			}
		}
		cur_index = 0;
		n = N;
		Bsearch(1);
		if (cur_index < N) {
			printf("not network\n");
			continue;
		}
		gate[1]--;
		count = 0;
		for (i = 1; i <= cur_index; i++)
			if (gate[i]) count++;
		printf("%d\n", count);
	}
	return 0;
}

int Bsearch(int r)
{
	int i;
	indexs[r] = ++cur_index;
	low[r] = indexs[r];
	for (i = 0; i <= n; i++)
		if (graph[r][i]) {
			if (!indexs[i]) {
				father[i] = r;
				Bsearch(i);
				if (low[i] >= indexs[r])
					gate[r]++;
				else if(low[i] < low[r])
					low[r] = low[i];
			} else if (father[r] != i) {
				if (indexs[i] < low[r])
					low[r] = indexs[i];
			}
		}
	return 0;
}

