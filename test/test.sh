#!/bin/bash
# Test suite for the Random Number Generator
set -e

BIN="./rng"
PASS=0
FAIL=0

run_test() {
    local desc="$1"
    local cmd="$2"
    echo -n "TEST: $desc ... "
    if eval "$cmd" > /dev/null 2>&1; then
        echo "PASS"
        PASS=$((PASS + 1))
    else
        echo "FAIL"
        FAIL=$((FAIL + 1))
    fi
}

echo "=== RNG Test Suite ==="
echo ""

# Test 1: Basic uniform generation
run_test "Generate 5 uniform numbers" \
    "$BIN -n 5 -l 1 -u 100"

# Test 2: Normal distribution
run_test "Generate normal distribution numbers" \
    "$BIN -n 10 -d normal -m 50 -v 10"

# Test 3: Statistics output
run_test "Show statistics" \
    "$BIN -n 100 -d normal --stats"

# Test 4: File output
run_test "Write output to file" \
    "$BIN -n 10 -o test_output.txt && test -f test_output.txt"

# Test 5: Reproducibility
run_test "Same seed produces same output" \
    "diff <($BIN -n 10 -s 123) <($BIN -n 10 -s 123)"

# Test 6: Help message
run_test "Help message works" \
    "$BIN -h"

# Test 7: Invalid count (should fail)
run_test "Reject invalid count" \
    "! $BIN -n -1 2>&1"

# Test 8: Invalid range (should fail)
run_test "Reject invalid range" \
    "! $BIN -l 100 -u 1 2>&1"

echo ""
echo "=== Results: $PASS passed, $FAIL failed ==="

# Clean up
rm -f test_output.txt

if [ "$FAIL" -gt 0 ]; then
    exit 1
fi
exit 0
