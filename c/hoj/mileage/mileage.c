#include <stdio.h>
int main()
{
	int miles, sum;
	char orig[32], dst[32], class[2];
	while (scanf("%s", orig) == 1) {
		if (orig[0] == '#')
			break;
		scanf("%s", dst);
		scanf("%d", &miles);
		scanf("%s", class);
		sum = 0;
		switch (class[0]) {
		case 'F':
			sum += miles * 2;
			break;
		case 'B':
			sum += miles + miles / 2;
			break;
		case 'Y':
			if (miles < 500)
				sum += 500;
			else
				sum += miles;
			break;
		default:
			break;
		}
		while (scanf("%s", orig) == 1) {
			if (orig[0] == '0') {
				printf("%d\n", sum);
				break;
			}
			scanf("%s", dst);
			scanf("%d", &miles);
			scanf("%s", class);
			switch (class[0]) {
			case 'F':
				sum += miles * 2;
				break;
			case 'B':
				sum += miles + miles / 2;
				break;
			case 'Y':
				if (miles < 500)
					sum += 500;
				else
					sum += miles;
				break;
			default:
				break;
			}
		}
	}
}

