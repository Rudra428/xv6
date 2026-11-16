#!/bin/bash

# Check that all three required values were supplied.
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 PRINCIPAL RATE TIME"
    exit 1
fi

# Accept ordinary decimal values for the principal, rate, and time.
number_pattern='^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$'
for value in "$1" "$2" "$3"; do
    if [[ ! $value =~ $number_pattern ]]; then
        echo "Error: principal, rate, and time must be numeric."
        exit 1
    fi
done

# Calculate simple interest and the total amount.
awk -v principal="$1" -v rate="$2" -v time="$3" 'BEGIN {
    interest = principal * rate * time / 100
    printf "Simple Interest = %g\n", interest
    printf "Total Amount = %g\n", principal + interest
}'
