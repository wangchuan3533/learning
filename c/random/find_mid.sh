#!/usr/bin/env bash
./a.out > result.txt
count=`wc -l result.txt | awk '{print $1}'`
mid=`dc -e "$count 2/1+p"`
sed -n "${mid}p" result.txt
