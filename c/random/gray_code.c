#include <stdio.h>
#include <inttypes.h>
void print_2(uint16_t v)
{
    int i;
    for (i = 0; i < 16; i++)
        printf("%u", (v >> (15 - i)) & 1);
    printf("\n");
}
int main()
{
    uint16_t v = 0;
    int i, offset, count;
    do {
        print_2(v);
        //printf("%d\n", v);
        count = 0;
        for (i = 15, offset = 15; i >= 0; i--) {
            if ((v >> i) & 1) {
                count++;
            }
            if (count % 2 == 0) {
                offset = i;
            }
        }
        v ^= (1 << offset);
    } while (v > 0);
}
