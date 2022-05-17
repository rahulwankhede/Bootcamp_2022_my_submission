#!/bin/bash

declare -i num=$1
declare -i ans=0
while [ $num -ne 0 ]; do
	ans=$((ans + num%10))
	num=$((num/10))
done
echo $ans
