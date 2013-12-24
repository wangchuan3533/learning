#include <stdio.h>
#include <string.h>

int main()
{
	char line[256], c;
	int i, j, num_line, col;

	while (scanf("%d", &col) && col) {
		scanf("%s", line);
		num_line = strlen(line) / col;
		for (i = 0; i < col; i++)
			for (j = 0; j < num_line; j++) {
				if (j % 2)
					c =line[j * col + col - 1 - i];
				else
					c =line[j * col + i];
				printf("%c", c);
			}
		printf("\n");
	}
	return 0;
}

