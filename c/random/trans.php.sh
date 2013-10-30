#!/bin/sh
sed 's/;//g' lfsr.list |\
awk \
'
BEGIN {
    printf("$lfsr_map = array(\n")
}

{
    if (NF == 7) {
        printf("    %s => array(%s, %s, %s, %s),\n", $1, $4, $5, $6, $7)
    } else if (NF == 5) {
        printf("    %s => array(%s, %s, %s, %s),\n", $1, $2, $3, $4, $5)
    }
}

END {
    printf(");\n")
}
'

