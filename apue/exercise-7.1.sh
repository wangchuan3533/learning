#!/usr/bin/env bash

TMPFILE=/tmp/exercise-7.1

echo '#include <stdio.h>'              >  ${TMPFILE}.c
echo 'int main()'                      >> ${TMPFILE}.c 
echo '{'                               >> ${TMPFILE}.c
echo '    printf("hello,world");'      >> ${TMPFILE}.c
echo '}'                               >> ${TMPFILE}.c

cc ${TMPFILE}.c -o ${TMPFILE}
${TMPFILE} > /dev/null
echo $?
#rm -rf ${TMPFILE}*
