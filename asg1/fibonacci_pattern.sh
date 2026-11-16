#!/bin/bash

# Read and validate the requested number of terms and pattern rows.
read -r -p "Enter a positive integer N: " n
if [[ ! $n =~ ^[0-9]+$ ]] || (( 10#$n < 1 )); then
    echo "Error: N must be a positive whole number."
    exit 1
fi
n=$((10#$n))

# Print the first N Fibonacci numbers using an iterative loop.
echo "First $n Fibonacci terms:"
first=0
second=1
for ((index = 0; index < n; index++)); do
    printf "%s" "$first"
    if (( index < n - 1 )); then
        printf " "
    fi
    next=$((first + second))
    first=$second
    second=$next
done
printf "\n"

# Print a right-angled triangle with N rows using nested loops.
echo "Star pattern:"
for ((row = 1; row <= n; row++)); do
    for ((star = 1; star <= row; star++)); do
        printf "*"
        if (( star < row )); then
            printf " "
        fi
    done
    printf "\n"
done
