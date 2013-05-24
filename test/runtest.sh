#!/bin/bash
aft=$1
fore=$2
orsa=$3
angle=$4
str1="../../demo_SURF_src/bin/surf -i output1.txt output2.txt $1 $2 -m matches.txt -o $3"
str2="../../DEM/block-matching-2 $1 $2 $3 $4"
str3="../../DEM/test-interpolation outi.png outj.png 40"
echo $str1
# ./${str1}
echo $str2
# ./${str2}
echo $str3
# ./${str3}