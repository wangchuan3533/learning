#include <stdio.h>

int main()
{
	int i, n, year, month, day, day_of_week;
	int period = 365 * 400 + 97;
	int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	char *week[7] = {"Saturday", "Sunday", "Monday", "Tuesday",
		"Wednesday", "Thursday", "Friday"};

	while (scanf("%d", &n) && n > 0) {
		day_of_week = n % 7;
		year = 2000 + n / period * 400;
		n %= period;
		while (n > 365) {
			if((year % 400 == 0) ||
				(year % 100 != 0 && year % 4 == 0)) {
				year++;
				n -= 366;
			} else {
				year++;
				n -=365;
			}
		}
		if((year % 400 == 0) ||
			(year % 100 != 0 && year % 4 == 0)) {
			days[1] = 29;
			month = 1;
			for (i = 0; n >= days[i]; i++) {
				month++;
				n -= days[i];
			}
			day = n + 1;
		} else {
			if (n == 365) {
				year++;
				n -= 365;
			}
			days[1] = 28;
			month = 1;
			for (i = 0; n >= days[i]; i++) {
				month++;
				n -= days[i];
			}
			day = n + 1;
		}
		printf("%d-%02d-%02d %s\n", year, month,
				day, week[day_of_week]);
	}
	return 0;
}

