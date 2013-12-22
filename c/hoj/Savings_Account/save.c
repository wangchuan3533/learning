#include <stdio.h>
int main()
{
	int i;
	double initial, interest, target;

	while (scanf("%lf %lf %lf", &initial, &interest, &target) == 3) {
		interest = 1.0 + interest / 100.0;
		for (i = 0; initial < target; i++)
			initial *= interest;
		printf("%d\n", i);
	}
	return 0;
}
