#!/bin/bash
# Test script for enhanced Design Project compiler

echo "=============================================="
echo "Design Project Compiler - Enhanced Testing"
echo "=============================================="
echo

# Test 1: Basic compilation with enhanced output
echo "Test 1: Enhanced Assembly Generation"
echo "--------------------------------------"
./build/compiler -o enhanced_output.s test_simple.txt
echo "Generated: enhanced_output.s"
echo

# Test 2: Check the enhanced assembly content
echo "Test 2: Assembly Content Preview"
echo "--------------------------------"
head -30 enhanced_output.s
echo "... (truncated)"
echo

# Test 3: Compiler with verbose stats
echo "Test 3: Verbose Statistics"
echo "-------------------------"
./build/compiler -v --show-stats test_simple.txt
echo

echo "=============================================="
echo "Design Project Testing Complete!"
echo "=============================================="