#include <stdio.h>
union uint32{
	unsigned int _int;
	unsigned char _char[4];
};
int main()
{
	union uint32 my;
	int n;
	
	scanf("%d", &n);
	while (n--) {
		scanf("%u", &(my._int));
		printf("0x%02X 0x%02X 0x%02X 0x%02X\n"
			, my._char[0], my._char[1]
			, my._char[2], my._char[3]);
	}
	return 0;
}

