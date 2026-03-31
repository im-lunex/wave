#!/usr/bin/env bash

echo "Testing single quote functionality:"
echo ""

echo "Test 1: echo 'hello    world'"
echo 'hello    world' | build/shell
echo ""

echo "Test 2: echo hello    world"
echo "hello    world" | build/shell
echo ""

echo "Test 3: echo 'hello''world'"
echo "hello''world" | build/shell
echo ""

echo "Test 4: echo hello''world"
echo "hello''world" | build/shell
echo ""

echo "Test 5: cat with quoted filenames containing spaces"
# Create test files
echo "content1" > "/tmp/file name"
echo "content2" > "/tmp/file name with spaces"
echo "cat '/tmp/file name' '/tmp/file name with spaces'" | build/shell
echo ""

# Cleanup
rm -f "/tmp/file name" "/tmp/file name with spaces"