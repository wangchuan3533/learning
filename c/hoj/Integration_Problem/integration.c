#include <stdio.h>
#include <math.h>
#define PI (3.1415926535898)
int main()
{
	double rate;
	double a;

	rate = 1.0 + PI / 3.0 - sqrt(3.0);
	while (scanf("%lf", &a) == 1) {
		a = a * a * rate;
		printf("%.3lf\n", a);
	}
	return 0;
}
