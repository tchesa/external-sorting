#!/bin/bash
echo 'Quicksort Externo' > quicksort_externo.dat
for i in `seq 1 3`;
do
    echo "$i" >> quicksort_externo.dat
    for j in 100 1000 10000 100000 471705;
    do
        ./ordena 4 $j $i >> quicksort_externo.dat
    done
done
