#include <stdio.h>
#include <string.h>
#define MAX_VERTEX (1001)
int graph[MAX_VERTEX][MAX_VERTEX];
int vertexs[MAX_VERTEX], low[MAX_VERTEX], father[MAX_VERTEX], SPF[MAX_VERTEX];
int count;
int n;
int Bsearch(int v);
int main()
{
	int i, j, network = 0, none_SPF;
	while (1) {
		n = 0;
		count = 0;
		memset(&(graph[0][0]), 0, sizeof(graph));
		memset(&(vertexs[0]), 0, sizeof(vertexs));
		memset(&(low[0]), 0,sizeof(low));
		memset(&(father[0]), 0,sizeof(father));
		memset(&(SPF[0]), 0,sizeof(SPF));
		scanf("%d", &i);
		if (!i) break;
		while (i) {
			scanf("%d", &j);
			graph[i][j] = 1;
			graph[j][i] = 1;
			if(i > n) n = i;
			if(j > n) n = j;
			scanf("%d", &i);
		}
		if (n) Bsearch(1);
		if (network)
			printf("\n");
		printf("Network #%d\n", ++network);
		if (count < n) {
			printf("  No SPF nodes\n");
			continue;
		}
		none_SPF = 1;
		SPF[1] --;
		for (i = 1; i <= n; i++) {
			if (SPF[i]) {
				printf("  SPF node %d leaves %d subnets\n",\
						i, 1 + SPF[i]);
				none_SPF = 0;
			}
		}
		if (none_SPF)
			printf("  No SPF nodes\n");
	}
	return 0;
}
int Bsearch(int v)
{
	int i;
	vertexs[v] = ++count;
	low[v] = vertexs[v];
	for (i = 1; i <= n; i++)
		if (graph[v][i]) {
			if (!vertexs[i]) {
				father[i] = v;
				Bsearch(i);
				if (low[i] >= vertexs[v])
					SPF[v]++;
				else if(low[i] < low[v] )
					low[v] = low[i];
			} else if(father[v] != i) {
				if (vertexs[i] < low[v])
					low[v] = vertexs[i];
			}
		}
	return 0;
}

