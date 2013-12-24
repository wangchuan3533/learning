#include <stdio.h>
void main()
{
	int a = ~(1 << 31);
	printf("%d\n", a);
	char fmt[8];
	int n = 9;
	a *= 3;
	//printf("%.0lf\n", a);
	sprintf(fmt, "%%0%dd\n", n);
	printf(fmt, n);
	
}