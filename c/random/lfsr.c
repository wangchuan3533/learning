#include <stdio.h>
#include <stdint.h>
int main(void)
{
    uint16_t lfsr = 0xACE1u;
    unsigned bit;
    unsigned period = 0;

    do {
        /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
        bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
        lfsr = (lfsr >> 1) | (bit << 15);
        ++period;

        //printf("bit=%d, lfsr=%d, period=%d\n", bit, lfsr, period);
        printf("%u\n", lfsr);
    }
    while (lfsr != 0xACE1u);

    return 0;
}
