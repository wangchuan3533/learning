#include <stdio.h>
int main()
{
	int i, j, N;
	int first, next, max, flag, tmp, coaches[1000];

	while (scanf("%d", &N) && N) {
		while (scanf("%d", &first) && first) {
			coaches[0] = first;
			for (i = 1; i < N; i++)
				scanf("%d", coaches + i);
			max = 1;
			flag = 1;
			for (i = 0; i < N - 2; i++) {
				if (max >= coaches[i] - 1)
					continue;
				tmp = coaches[i];
				for(j = i + 1; j < N;) {
					while (coaches[j] > coaches[i]
							&& j < N) j++;
					if (j == N)
						break;
					if (coaches[j] > tmp) {
						flag = 0;
						break;
					} else {
						tmp = coaches[j++];
					}
				}
				if (!flag)
					break;
				max = coaches[i] - 1;
			}
			if (flag)
				printf("Yes\n");
			else
				printf("No\n");
		}
		printf("\n");
	}
	return 0;
}

