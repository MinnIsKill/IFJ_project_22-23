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
    ./../../test/parser/parser_test <./$f #| dot -Tpng >$dst.png
    case "${PIPESTATUS[0]}" in
        0)
            echo -e "$f :: ${TCBLACK}${BGCGREEN}SUCCESS${COFF}"
            ;;
        1)
            echo -e "$f :: ${TCWHITE}${BGCRED}LEX ERROR${COFF}"
            ;;
        2)
            echo -e "$f :: ${TCWHITE}${BGCRED}SYNTAX ERROR${COFF}"
            ;;
        3|4|5|6|7|8)
            echo -e "$f :: ${TCWHITE}${BGCRED}SEMANTIC ERROR${COFF}"
            ;;

        99)
            echo -e "$f :: ${TCWHITE}${BGCRED}INTERNAL ERROR${COFF}"
            ;;
        *)
            echo -e "$f :: ${TCWHITE}${BGCRED}OTHER ERROR${COFF} :: ${PIPESTATUS[0]}"
            ;;
    esac
    echo ""
done
