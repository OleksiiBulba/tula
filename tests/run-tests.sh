#!/bin/bash

executable="./bin/tula-test"
test_dir="./examples"
output_dir="./tests"
passed=0
failed=0

while IFS= read -r test; do
    base_name=$(basename "${test}" .tula)
    expected="${test/$test_dir/$output_dir}.out"
    output="${test/$test_dir/$output_dir}.actual.out"
    mkdir -p "$(dirname "$output")"
    $executable "$test" > "$output"

    if diff -u "$expected" "$output"; then
        echo "Test $base_name passed"
        ((passed++))
    else
        echo "Test $base_name failed"
        ((failed++))
    fi

    rm "$output"
done < <(find "$test_dir" -name "*.tula")

echo "Tests passed: $passed"
echo "Tests failed: $failed"

if [ "$failed" -ne 0 ]; then
    exit 1
fi

