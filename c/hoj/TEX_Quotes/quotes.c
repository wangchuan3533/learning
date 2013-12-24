#include <stdio.h>
#include <string.h>
int main()
{
	int open = 0;
	char buf[1];

	while (fread(buf, sizeof(char), 1, stdin) == 1) {
		if (buf[0] == '\"') {
			if (open) {
				printf("''");
				open = 0;
			} else {
				printf("``");
				open = 1;
			}
		}
		else
			fwrite(buf, sizeof(char), 1, stdout);
	}
	return 0;
}

