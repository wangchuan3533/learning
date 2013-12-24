#include <stdio.h>
int main()
{
	int n, zombies, brains;
	scanf("%d", &n);
	while (n--) {
		scanf("%d%d", &brains, &zombies);
		if (brains >= zombies)
			printf("MMM BRAINS\n");
		else
			printf("NO BRAINS\n");
	}
	return 0;
}
