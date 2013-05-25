#!/bin/bash
aft=$1
fore=$2
orsa=$3
angle=$4
str1="../../demo_SURF_src/bin/surf -i output1.txt output2.txt $aft $fore -m matches.txt -o $orsa"
echo $str1
./${str1}
str2="../../DEM/block-matching-pyramid $aft $fore $orsa $angle"
echo $str2
./${str2}
str3="../../DEM/test-interpolation outi.png outj.png 40 `cat shiftLateral.txt`"
echo $str3
./${str3}