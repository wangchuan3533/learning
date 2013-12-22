#include <stdio.h>
#include <string.h>
#define max_len (256 + 1)
char buf[max_len];
int check_syntex(char const *str, int start, int end);

int main()
{
	int start, end;
	while (scanf("%s", buf) != EOF) {
		start = 0;
		end = strlen(buf) - 1;
		if (check_syntex(buf, start, end))
			printf("YES\n");
		else
			printf("NO\n");
	}
}

int check_syntex(char const *str, int start, int end)
{
	int tmp;
	if (start == end)
		if (str[start] >= 'p' && str[start] <= 'z')
			return 1;
		else
			return 0;
	switch (str[start]) {
	case 'N':
		return check_syntex(str, start + 1, end);
		break;
	case 'C':
	case 'D':
	case 'E':
	case 'I':
		tmp = start + 1;
		while (tmp < end && (!check_syntex(str, start + 1, tmp)))
			tmp++;
		if (tmp >= end)
			return 0;
		return check_syntex(str, tmp + 1, end);
		break;
	default:
		return 0;
	}
}
