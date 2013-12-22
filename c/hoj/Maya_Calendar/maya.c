#include <stdio.h>
#include <string.h>
int main()
{
	char *Haab[19] = { "pop", "no", "zip", "zotz", "tzec", "xul", "yoxkin",
		"mol", "chen", "yax", "zac", "ceh", "mac", "kankin", "muan",
		"pax", "koyab", "cumhu", "uayet"}; 
	char *Tzolkin[20] = { "imix", "ik", "akbal", "kan", "chicchan", "cimi",
		"manik", "lamat", "muluk", "ok", "chuen", "eb", "ben", "ix",
		"mem", "cib", "caban", "eznab", "canac", "ahau"}; 
	char month_Haab[16];
	int day_Haab, year_Haab;
	int total, day, month, year, n, i;

	scanf("%d", &n);
	printf("%d\n", n);
	while (n--) {
		scanf("%d. %s %d", &day_Haab, month_Haab, &year_Haab);	
		total = year_Haab * 365;
		for (i = 0; strcmp(Haab[i], month_Haab) != 0; i++) ;
		total += (20 * i);
		total += day_Haab;
		year = total / 260;
		total %= 260;
		month = total % 20;
		day = total % 13 + 1;
		printf("%d %s %d\n", day, Tzolkin[month], year);
	}
	return 0;
}
