#!/bin/bash

my_array=("man" "bear" "pig" "dog" "cat" "sheep")
for animal in ${my_array[@]}; do
	echo $animal
done
