#!/bin/sh
sed 's/;//g' lfsr.list |\
awk \
'
BEGIN {
    printf("int[4][] lfsr_map = {\n")
    for (i = 0; i < 5; i++) {
        printf("    {0, 0, 0, 0},\n")
    }
}

{
    if (NF == 7) {
        printf("    {%s, %s, %s, %s},\n", $4, $5, $6, $7)
    } else if (NF == 5) {
        printf("    {%s, %s, %s, %s},\n", $2, $3, $4, $5)
    }
}

END {
    printf("};\n")
}
'

