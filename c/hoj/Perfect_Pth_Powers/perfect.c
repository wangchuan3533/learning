#include <math.h>
#include <stdio.h>
int main()
{
	int i, x, y, p, prod;

	while (scanf("%d", &x) && x) {
		for (p = 31; p > 0; p--) {
			y = (int)pow((double)x, 1.0 / (double)p);
			while (1) {
				prod = 1;
				for (i = 0; i < p; i++)
					prod *= y;
				if (prod > x)
					break;
				else if (prod == x) {
					printf("%d\n", p);
					p = 0;
					break;
				}
				y++;
			}
		}
	}
	return 0;
}

