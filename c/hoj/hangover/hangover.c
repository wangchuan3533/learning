#include <stdio.h>
#define MAX_N (300)

int main()
{
	double length[MAX_N], c;
	int i;
	
	length[0] = 0.0;
	for (i = 1; i < MAX_N; i++)
		length[i] = length[i - 1] + 1.0 / (double)(i + 1);
       while (scanf("%lf", &c) && c > 0.001) {
	       for (i = 0; length[i] < c; i++) ;
	       printf("%d card(s)\n", i);
       }
       return 0;
}
