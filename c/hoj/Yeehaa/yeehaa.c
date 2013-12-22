#include <stdio.h>
#include <math.h>
int main()
{
	int i, N;
	double R, n, r, pi, sin_value;

	pi = atan(1.0) * 4.0;
	scanf("%d", &N);
	for (i = 0; i < N; i++) {
		scanf("%lf%lf", &R, &n);
		sin_value = sin(pi / n);
		r = R * sin_value / (1 + sin_value);
		printf("Scenario #%d:\n%.3lf\n\n", i + 1, r);
	}
	return 0;
}

