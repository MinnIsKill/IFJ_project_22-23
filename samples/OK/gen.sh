#!/bin/bash
# author : Jan Lutonský, xluton02

for f in ./*.php; do
    dst=${f%.*}  
    ./../../compiler <./$f | dot -Tpng >$dst.png
done
