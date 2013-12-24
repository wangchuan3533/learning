#include <stdio.h>
#include <string.h>
char cypher[128];
char plain[64];
char substitution[64];
char line[65];
int main()
{
	int i, j, len;
	gets(plain);
	gets(substitution);
	puts(substitution);
	puts(plain);
	len = strlen(plain);
	for (i = 0; i < 128; i++)
		cypher[i] = i;
	for (i = 0; i < len; i++)
		cypher[plain[i]] = substitution[i];
	while (gets(line)) {
		len = strlen(line);
		for (i = 0; i < len; i++)
			line[i] = cypher[line[i]];
		puts(line);
	}
	return 0;
}