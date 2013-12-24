#include <stdio.h>
#include <time.h>
int main()
{
	int time1, time2;
	int y, m, d, h, mi, s, period;
	char unit[16];
	time_t now;
	struct tm *tm_now;
	char *str_now;
	time(&now);
	tm_now = gmtime(&now);
	str_now = asctime(tm_now);
	printf("%s\n", str_now);
	y = tm_now->tm_year + 1900;
	m = tm_now->tm_mon + 1;
	d = tm_now->tm_mday;
	h = tm_now->tm_hour;
	mi = tm_now->tm_min;
	s = tm_now->tm_sec;
	now = calc_time(y, m, d, h, mi, s);
	tm_now = gmtime(&now);
	str_now = asctime(tm_now);
	printf("%s\n", str_now);
	while (scanf("%d %d %d %d %d %d", &y, &m, &d, &h, &mi, &s) != EOF) {
		time1 = calc_time(y, m, d, h, mi, s);
		scanf("%d %d %d %d %d %d", &y, &m, &d, &h, &mi, &s);
		time2 = calc_time(y, m, d, h, mi, s);
		scanf("%d", &period);
		scanf("%s", unit);
		switch (unit[0]) {
		case 's':
			break;
		case 'm':
			if (unit[1] == 'i') period *= 60;
			else period *= (30 * 24 * 60 * 60);
			break;
		case 'h':
			period *= (60 * 60);
			break;
		case 'd':
			period *= (24 * 60 * 60);
			break;
		case 'y':
			period *= (365 * 24 * 60 * 60);
			break;
		}
		period = (time2 - time1) / period;
		printf("%d\n", period);
	}
	return 0;
}

int calc_time(int y, int m, int d, int h, int mi, int s)
{
	int days[13] = {0, 0, 31, 59, 90, 120, 151,
			181, 212, 243, 273, 304, 334};
	int time = 0;
	if (y % 4 == 0 && m > 2) time += (24 * 60 * 60);
	y -= 1970;
	time +=  (y * 365 * 24 * 60 * 60);
	time += ((y + 1) / 4 * 24 * 60 * 60);
	time += ((days[m] + d - 1) * 24 * 60 * 60);
	time += (((h * 60) + mi) * 60 + s);
	return time;
}


