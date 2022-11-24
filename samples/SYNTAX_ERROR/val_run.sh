#!/bin/bash
# author : Jan Lutonský, xluton02

TCWHITE='\033[0;37m'
TCBLACK='\033[0;30m'
BGCRED='\033[41m'
BGCGREEN='\033[42m'
COFF='\033[0m'

for f in ./*.php; do
    dst=${f%.*}  
    echo "START :: $f"
    valgrind --error-exitcode=222 --leak-check=full ./../../compiler <./$f 1>/dev/null 2>/dev/null
    retval=$?
    if [ $retval -eq 222 ]; then
        echo -e "$f :: ${TCWHITE}${BGCRED}failed${COFF} valgrind test"
    else
        echo -e "$f :: ${TCBLACK}${BGCGREEN}NO LEAKS${COFF}"
    fi
    echo ""
done
