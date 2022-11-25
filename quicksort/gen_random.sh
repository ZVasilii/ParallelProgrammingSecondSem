#!/bin/bash
# Generate a random using `shuf` command
read number

#Print the generated random numbers
for n in `seq "$number"`
do
randomNumber=$(shuf -i 1-100 -n1)
echo $randomNumber
done
