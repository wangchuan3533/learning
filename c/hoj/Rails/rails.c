#include <stdio.h>
int main()
{
	int i, j;
	int N, size, first, next, v, stack[1000];
	int flag;

	while (scanf("%d", &N) && N) {
		while (scanf("%d", &first) && first) {
			size = 0;
			for (next = 1; next <= first; next++)
				stack[size++] = next;
			size--;
			flag = 1;
			for (i = 1; i < N; i++) {
				scanf("%d", &v);
				if (v < next) {
					if (size > 0 && stack[size - 1] == v)
						size--;
					else
						flag = 0;
				} else {
					for (; next <= v; next++)
						stack[size++] = next;
					size--;
				}
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


