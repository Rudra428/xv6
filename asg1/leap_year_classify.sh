#!/bin/bash

# Read and validate the year before applying the leap-year rule.
read -r -p "Enter a year: " year
if [[ ! $year =~ ^[0-9]+$ ]]; then
    echo "Error: year must be a non-negative whole number."
    exit 1
fi
year=$((10#$year))

if (( year % 400 == 0 || (year % 4 == 0 && year % 100 != 0) )); then
    echo "$year is a leap year."
else
    echo "$year is not a leap year."
fi

# Read and validate a signed whole number for classification.
read -r -p "Enter a number: " number
if [[ ! $number =~ ^-?[0-9]+$ ]]; then
    echo "Error: number must be a whole number."
    exit 1
fi
if [[ $number == -* ]]; then
    number=$((-10#${number#-}))
else
    number=$((10#$number))
fi

# Report parity and whether the number is positive, negative, or zero.
if (( number % 2 == 0 )); then
    echo "$number is even."
else
    echo "$number is odd."
fi

if (( number > 0 )); then
    echo "$number is positive."
elif (( number < 0 )); then
    echo "$number is negative."
else
    echo "$number is zero."
fi
