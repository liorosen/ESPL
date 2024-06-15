#!/bin/bash

# Compile the shell
gcc -g -m32 -Wall -o myshell myshellTask3.c LineParser.c

# Function to run a test case
run_test() {
    input="$1"
    expected_output="$2"
    description="$3"

    echo -e "$input" | ./myshell > output.txt

    if cmp -s output.txt "$expected_output"; then
        echo "[PASS] $description"
    else
        echo "[FAIL] $description"
        echo "Expected output:"
        cat "$expected_output"
        echo "Actual output:"
        cat output.txt
    fi

    rm -f output.txt
}

# Prepare test files
echo "This is a test file." > in.txt
echo "This is a test file." > expected_cat_out.txt
echo "/tmp" > expected_cd_out.txt
echo "hello" > expected_hello_out.txt

# Test cases
run_test "cat < in.txt > out.txt\ncat out.txt\nquit" "expected_cat_out.txt" "Test cat with input and output redirection"
run_test "cd /tmp\npwd\nquit" "expected_cd_out.txt" "Test change directory"
run_test "echo hello > out.txt\ncat out.txt\nquit" "expected_hello_out.txt" "Test echo with output redirection"

# Cleanup
rm -f in.txt out.txt expected_cat_out.txt expected_cd_out.txt expected_hello_out.txt
