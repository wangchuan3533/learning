#include <stdio.h>
#include <stdint.h>
int main(void)
{
    uint32_t lfsr = 0x1234, card_no;
    unsigned bit;
    unsigned period = 0;

    do {
        bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
        lfsr = ((lfsr >> 1) | (bit << 22)) & 0x7fffff;
        ++period;
        card_no = 10000000 - lfsr;

        //printf("bit=%d, lfsr=%d, period=%d\n", bit, lfsr, period);
        printf("%u\n", card_no);
    }
    while (lfsr != 0x1234 && period < 1000000);

    return 0;
}
