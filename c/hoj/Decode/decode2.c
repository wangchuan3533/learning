#include <stdio.h>
#include <string.h>
#define MAX_LEN (10002)
char msg_orig[MAX_LEN * 2], msg_modified[MAX_LEN * 2];
int main()
{
	int n, i;
	char input[3], key, msg, encoded;
	input[2] = '\0';
	scanf("%s", msg_orig);
	scanf("%s", msg_modified);
	n = strlen(msg_orig) / 2;
	input[0] = msg_modified[0];
	input[1] = msg_modified[1];
	sscanf(input, "%X", &encoded);
	key = encoded ^ ' ';
	printf("%02X", key);
	for (i = 0; i < n; i++) {
		input[0] = msg_orig[2 * i];
		input[1] = msg_orig[2 * i + 1];
		sscanf(input, "%X", &encoded);
		msg = key ^ encoded;
		input[0] = msg_modified[2 * (i + 1)];
		input[1] = msg_modified[2 * (i + 1) + 1];
		sscanf(input, "%X", &encoded);
		key = msg ^ encoded;
		printf("%02X", key);
	}	
	return 0;
}