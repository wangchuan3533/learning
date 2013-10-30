#include <stdio.h>
#include <inttypes.h>

#define LFSR_NEXT(cur, bit, map) \
    (((((((cur) >> ((bit) - (map)[(bit)][0])) ^ ((cur) >> ((bit) - (map)[(bit)][1])) ^ ((cur) >> ((bit) - (map)[(bit)][2])) ^ ((cur) >> ((bit) - (map)[(bit)][3]))) \
        & 1u) << ((bit) - 1)) | ((cur) >> 1)) & (0xffffffffffffffff >> (64 - (bit))))

#define LFSR_PREV(cur, bit, map) \
    (((((((cur) >> ((bit) - 1)) ^ ((cur) >> ((bit) - (map)[(bit)][1] - 1)) ^ ((cur) >> ((bit) - (map)[(bit)][2] - 1)) ^ ((cur) >> ((bit) - (map)[(bit)][3] - 1))) \
        & 1u)) | ((cur) << 1)) & (0xffffffffffffffff >> (64 - (bit))))

int lfsr_map[][4]  = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {5, 4, 3, 2},
    {6, 5, 3, 2},
    {7, 6, 5, 4},
    {8, 6, 5, 4},
    {9, 8, 6, 5},
    {10, 9, 7, 6},
    {11, 10, 9, 7},
    {12, 11, 8, 6},
    {13, 12, 10, 9},
    {14, 13, 11, 9},
    {15, 14, 13, 11},
    {16, 14, 13, 11},
    {17, 16, 15, 14},
    {18, 17, 16, 13},
    {19, 18, 17, 14},
    {20, 19, 16, 14},
    {21, 20, 19, 16},
    {22, 19, 18, 17},
    {23, 22, 20, 18},
    {24, 23, 21, 20},
    {25, 24, 23, 22},
    {26, 25, 24, 20},
    {27, 26, 25, 22},
    {28, 27, 24, 22},
    {29, 28, 27, 25},
    {30, 29, 26, 24},
    {31, 30, 29, 28},
    {32, 30, 26, 25},
    {33, 32, 29, 27},
    {34, 31, 30, 26},
    {35, 34, 28, 27},
    {36, 35, 29, 28},
    {37, 36, 33, 31},
    {38, 37, 33, 32},
    {39, 38, 35, 32},
    {40, 37, 36, 35},
    {41, 40, 39, 38},
    {42, 40, 37, 35},
    {43, 42, 38, 37},
    {44, 42, 39, 38},
    {45, 44, 42, 41},
    {46, 40, 39, 38},
    {47, 46, 43, 42},
    {48, 44, 41, 39},
    {49, 45, 44, 43},
    {50, 48, 47, 46},
    {51, 50, 48, 45},
    {52, 51, 49, 46},
    {53, 52, 51, 47},
    {54, 51, 48, 46},
    {55, 54, 53, 49},
    {56, 54, 52, 49},
    {57, 55, 54, 52},
    {58, 57, 53, 52},
    {59, 57, 55, 52},
    {60, 58, 56, 55},
    {61, 60, 59, 56},
    {62, 59, 57, 56},
    {63, 62, 59, 58},
    {64, 63, 61, 60},
};

struct lfsr_config {
    int n_bits;
    uint64_t seed;
};

struct lfsr_segment {
    uint64_t start;
    uint64_t end;
    uint64_t count;
};

void lfsr_gen_segment(int n_bits, uint64_t seed, uint64_t count, struct lfsr_segment *segment) 
{
    uint64_t current = seed;
    segment->start = seed;
    segment->count = 0;
    do {
        current = LFSR_NEXT(current, n_bits, lfsr_map);
        segment->end = current;
        segment->count++;
    } while (segment->count < count);
}

int main()
{
    uint64_t lfsr = 0x1234;
    uint64_t next = LFSR_NEXT(lfsr, 64, lfsr_map);
    uint64_t prev = LFSR_PREV(next, 64, lfsr_map);
    struct lfsr_segment segment;
    printf("next=0x%llx, prev=0x%llx\n", next, prev);
    lfsr_gen_segment(24, lfsr, (1 << 24) - 1, &segment);
    printf("segment[start]=0x%llx, segment[end]=0x%llx, segment[count]=0x%llx\n", segment.start, segment.end, segment.count);
}


