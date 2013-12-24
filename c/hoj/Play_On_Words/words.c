#include <stdio.h>
#include <string.h>
#define num_letter (26)
int table[num_letter + 1][num_letter + 1];
int visited[num_letter + 1];
int dfs(int index)
{
	int i;

	visited[index] = 1;
	for (i = 1; i <= num_letter; i++)
		if ((table[index][i] || table[i][index]) && !visited[i])
			dfs(i);
	return 0;
}
int main()
{
	int T, N, i, j, cnt_head, cnt_tail, cnt_other, len;
	char buf[1024];
	
	scanf("%d", &T);
	for (i = 0; i < T; i++) {
		scanf("%d", &N);
		memset(table, 0x00, sizeof(table));
		memset(visited, 0x00, sizeof(visited));
		for (j = 0; j < N; j++) {
			scanf("%s", buf);
			len = strlen(buf);
			table[buf[0] - 'a' + 1][buf[len - 1] - 'a' + 1]++;
			table[buf[0] - 'a' + 1][0]++;
			table[0][buf[len - 1] - 'a' + 1]++;
		}
		for (j = 1; table[j][0] == 0; j++) ;
		dfs(j);
		for (j = 1; j <= num_letter; j++)
			if ((table[j][0] || table[0][j]) && !visited[j])
				break;
		if (j <= num_letter) {
			printf("The door cannot be opened.\n");
			continue;
		}
		cnt_head = 0;
		cnt_tail = 0;
		cnt_other = 0;
		for (j = 1; j <= num_letter; j++)
			switch (table[j][0] - table[0][j]) {
			case 0:
				break;
			case 1:
				cnt_head++;
				break;
			case -1:
				cnt_tail++;
				break;
			default:
				cnt_other++;
				break;
			}
		if (cnt_other) {
			printf("The door cannot be opened.\n");
			continue;
		}
		if (cnt_head == cnt_tail && cnt_head < 2) {
			printf("Ordering is possible.\n");
			continue;
		}
		printf("The door cannot be opened.\n");
	}
	return 0;
}
