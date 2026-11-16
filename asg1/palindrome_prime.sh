#!/bin/bash

# Return success when the input reads the same in either direction.
is_palindrome() {
    local value=$1
    local reversed=""
    local index

    for ((index = ${#value} - 1; index >= 0; index--)); do
        reversed+="${value:index:1}"
    done

    [[ $value == "$reversed" ]]
}

# Return success when the supplied non-negative integer is prime.
is_prime() {
    local value=$1
    local divisor

    value=$((10#$value))
    (( value > 1 )) || return 1

    for ((divisor = 2; divisor <= value / divisor; divisor++)); do
        if (( value % divisor == 0 )); then
            return 1
        fi
    done
    return 0
}

# Read one value and always check it for palindrome status.
read -r -p "Enter a string or number: " value
if [[ -z $value ]]; then
    echo "Error: value must not be empty."
    exit 1
fi

if is_palindrome "$value"; then
    echo "$value is a Palindrome."
else
    echo "$value is not a Palindrome."
fi

# Check primality only when the value contains digits only.
if [[ $value =~ ^[0-9]+$ ]]; then
    if is_prime "$value"; then
        echo "$value is a Prime number."
    else
        echo "$value is not a Prime number."
    fi
fi
