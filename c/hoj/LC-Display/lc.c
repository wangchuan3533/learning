#include <stdio.h>
#include <string.h>
#define MAX_S (10)
#define MAX_N (8)
char canvas[2 * MAX_S + 3][MAX_N * (MAX_S + 3)];
char encode[] = {0x77, 0x14, 0x6e, 0x3e, 0x1d, 0x3b, 0x7b, 0x16, 0x7f, 0x3f};
char buf[16];
int n, s;
int draw(char digit, int base);
int main()
{
	int i, j;

	while ((scanf("%d %s", &s, buf) == 2) && s) {
		memset(canvas, ' ', sizeof(canvas));
		n = strlen(buf);
		for (i = 0; i < n; i++)
			draw(buf[i], i * (s + 3));
		for (i = 0; i < 2 * s + 3; i++) {
			canvas[i][n * (s + 3) - 1] = '\0';
			printf("%s\n", canvas[i]);
		}
		printf("\n");
	}
	return 0;
}
int draw(char digit, int base)
{
	int i, j;
	char code;

#define draw_x(x, y, len) \
	for (i = 0; i < (len); i++)\
		canvas[(y)][((x) + i + base)] = '-';
#define draw_y(x, y, len) \
	for (i = 0; i < (len); i++)\
		canvas[((y) + i)][((x) + base)] = '|';

	code = encode[digit - '0'];
	if (code & 0x01)
		draw_y(0, 1, s);
	if (code & 0x02)
		draw_x(1, 0, s);
	if (code & 0x04)
		draw_y(s + 1, 1, s);
	if (code & 0x08)
		draw_x(1, s + 1, s);
	if (code & 0x10)
		draw_y(s + 1, s + 2, s);
	if (code & 0x20)
		draw_x(1, 2 * s + 2, s);
	if (code & 0x40)
		draw_y(0, s + 2, s);
}
