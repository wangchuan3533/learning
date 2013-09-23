#!/usr/bin/env bash
AB=/usr/local/apache/bin/ab
$AB -n10000 -c1000 http://10.0.1.22:54574/ > ab.txt 2>&1

