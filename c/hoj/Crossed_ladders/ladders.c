#include <stdio.h>
#include <math.h>

double fun(double x, double y, double w, double c)
{
	double tmp1, tmp2;
	tmp1 = x * x - w * w;
	tmp1 = sqrt(tmp1);
	tmp2 = y * y - w * w;
	tmp2 = sqrt(tmp2);
	tmp2 = tmp1 / tmp2;
	tmp1 = tmp1 / c;
	tmp1 = tmp2 - tmp1 + 1;
	return tmp1;
}
int main()
{
	double x, y, w0, w1, c;
	double mid, value;

	while (scanf("%lf %lf %lf", &x, &y, &c) == 3) {
		if (x < y) {
			w0 = x;
			x = y;
			y = w0;
		}
		w0 = 0.0001;
		w1 = y - 0.0001;
		while (w1 - w0 > 0.0001) {
			mid = (w0 + w1) / 2;
			value = fun(x, y, mid, c);
			if (value < 0)
				w0 = mid;
			else
				w1 = mid;
		}
		printf("%.3lf\n", mid);
	}
	return 0;
}
