#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LEN (7)
#define MAX_NUM (100)
char dict[MAX_NUM][MAX_LEN];
char sorted_dict[MAX_NUM][MAX_LEN];
int cmp_char(const void *a, const void *b);
int cmp_str(const void *a, const void *b);
int main()
{
	char input[MAX_LEN];
	int i, num_words = 0, count;

	while (scanf("%s", input) && input[0] != 'X')
		strcpy(dict[num_words++], input);
	qsort(dict, num_words, sizeof(dict[0]), cmp_str);
	for (i = 0; i < num_words; i++) {
		strcpy(sorted_dict[i], dict[i]);
		qsort(sorted_dict[i], strlen(sorted_dict[i]),
				sizeof(char), cmp_char);
	}
	while (scanf("%s", input) && input[0] != 'X') {
		qsort(input, strlen(input),
				sizeof(char), cmp_char);
		for (i = 0, count = 0; i < num_words; i++)
			if (strcmp(input, sorted_dict[i]) == 0) {
				printf("%s\n", dict[i]);
				count++;
			}
		if (!count)
			printf("NOT A VALID WORD\n");
		printf("******\n");
	}
	return 0;
}
int cmp_char(const void *a, const void *b)
{
	return *(char *)a - *(char *)b;
}
int cmp_str(const void *a, const void *b)
{
	return strcmp((char *)a, (char *)b);
}

