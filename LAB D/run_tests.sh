#!/bin/bash

# Compile the program
make clean
make

# Function to run a test and display results
run_test() {
    echo "Running $1"
    eval "$2" > "$3"
    echo "Output:"
    cat "$3"
    echo -e "\nExpected Output:"
    echo -e "$4\n"
}

# Test 0: Basic Command-line Arguments Printing
run_test "Test 0: Basic Command-line Arguments Printing" \
         "./multi arg1 arg2 arg3" \
         "output_test_0.txt" \
         "4\nmulti\narg1\narg2\narg3"

# Test 1A: Printing a Multi-Precision Integer
run_test "Test 1A: Printing a Multi-Precision Integer" \
         "./multi" \
         "output_test_1A.txt" \
         "4f440201aa"

# Test 1B: Reading a Multi-Precision Integer
run_test "Test 1B: Reading a Multi-Precision Integer" \
         "echo '4f440201aa' | ./multi -I" \
         "output_test_1B.txt" \
         "4f440201aa"

# Test 2: Addition of Multi-Precision Integers
run_test "Test 2: Addition of Multi-Precision Integers" \
         "./multi" \
         "output_test_2.txt" \
         "4f440201aa\n4f44030201aa\n4f9347040354"

# Test 3: Pseudo-Random Number Generator (PRNG)
run_test "Test 3: Pseudo-Random Number Generator (PRNG)" \
         "./multi -R" \
         "output_test_3.txt" \
         "Varies depending on the PRNG implementation"

# Test 4: Full Program Integration
run_test "Test 4: Full Program Integration" \
         "./multi -I" \
         "output_test_4.txt" \
         "Your expected integrated output based on the input"

# Clean up
make clean
