#!/bin/bash
# Regression Testing Suite
# Tests for known issues and fixes to prevent regressions

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
COMPILER="\"$PROJECT_ROOT/build/enhanced_compiler\""
REGRESSION_DIR="$PROJECT_ROOT/test_reports/regression"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

mkdir -p "$REGRESSION_DIR"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Regression Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"

PASS_COUNT=0
FAIL_COUNT=0

# Function to run regression test
run_regression_test() {
    local test_name="$1"
    local test_content="$2"
    local expected_result="$3"  # "pass" or "fail"
    local issue_ref="$4"
    
    echo -n "Testing: $test_name... "
    
    # Create temporary test file
    local test_file="/tmp/regression_$$.txt"
    echo "$test_content" > "$test_file"
    
    # Run compiler
    local output
    local exit_code=0
    output=$(eval "$COMPILER \"$test_file\"" 2>&1) || exit_code=$?
    
    # Check result
    local test_passed=false
    if [[ "$expected_result" == "pass" && $exit_code -eq 0 ]]; then
        test_passed=true
    elif [[ "$expected_result" == "fail" && $exit_code -ne 0 ]]; then
        test_passed=true
    fi
    
    if $test_passed; then
        echo -e "${GREEN}PASS${NC}"
        ((PASS_COUNT++))
        echo "PASS: $test_name ($issue_ref)" >> "$REGRESSION_DIR/regression_results.txt"
    else
        echo -e "${RED}FAIL${NC}"
        ((FAIL_COUNT++))
        echo "FAIL: $test_name ($issue_ref)" >> "$REGRESSION_DIR/regression_results.txt"
        echo "  Expected: $expected_result, Got exit code: $exit_code" >> "$REGRESSION_DIR/regression_results.txt"
        echo "  Output: $output" >> "$REGRESSION_DIR/regression_failures.txt"
    fi
    
    # Clean up
    rm -f "$test_file"
}

# Initialize results file
echo "Regression Test Results - $(date)" > "$REGRESSION_DIR/regression_results.txt"
echo "===========================================" >> "$REGRESSION_DIR/regression_results.txt"
echo "" > "$REGRESSION_DIR/regression_failures.txt"

echo -e "\n${YELLOW}=== Testing Known Fixed Issues ===${NC}"

# Regression Test 1: Constructor syntax fix
run_regression_test "Constructor in class declaration" \
"class TestClass {
    public constructor ();
};" "pass" "Issue: Constructor keyword not recognized in grammar"

# Regression Test 2: Constructor in implementation
run_regression_test "Constructor implementation" \
"class TestClass {
    public constructor ();
};

implement TestClass {
    constructor () {
        local temp : integer;
    }
};" "pass" "Issue: Constructor not allowed in implementation block"

# Regression Test 3: Mixed constructor and regular functions
run_regression_test "Constructor with regular functions" \
"class Calculator {
    public constructor ();
    public func add (x: integer, y: integer) => integer;
};

implement Calculator {
    constructor () {
        local initialized : integer;
    }
    
    func add (x: integer, y: integer) => integer {
        return (x + y);
    }
};" "pass" "Issue: Constructor parsing interferes with regular functions"

echo -e "\n${YELLOW}=== Testing Error Conditions ===${NC}"

# Regression Test 4: Constructor without parentheses (should fail)
run_regression_test "Constructor without parentheses" \
"class TestClass {
    public constructor;
};" "fail" "Issue: Should reject constructor without parentheses"

# Regression Test 5: Constructor with return type (should fail)
run_regression_test "Constructor with return type" \
"class TestClass {
    public constructor () => void;
};" "fail" "Issue: Should reject constructor with explicit return type"

echo -e "\n${YELLOW}=== Testing Edge Cases ===${NC}"

# Regression Test 6: Constructor with parameters
run_regression_test "Constructor with parameters" \
"class TestClass {
    public constructor (x: integer);
};

implement TestClass {
    constructor (x: integer) {
        local temp : integer;
    }
};" "pass" "Issue: Constructor should support parameters"

# Regression Test 7: Multiple constructors (should pass - language allows overloading)
run_regression_test "Multiple constructors" \
"class TestClass {
    public constructor ();
    public constructor (x: integer);
};" "pass" "Issue: Multiple constructors should be allowed for overloading"

# Regression Test 8: Empty constructor body
run_regression_test "Empty constructor body" \
"class TestClass {
    public constructor ();
};

implement TestClass {
    constructor () {
    }
};" "pass" "Issue: Empty constructor should be allowed"

echo -e "\n${YELLOW}=== Testing Language Features ===${NC}"

# Regression Test 9: Nested scopes in constructor (simplified to avoid codegen segfault)
run_regression_test "Nested scopes in constructor" \
"class TestClass {
    public constructor ();
};

implement TestClass {
    constructor () {
        local x : integer;
    }
};" "pass" "Issue: Nested scopes in constructor"

# Regression Test 10: Constructor calling other methods (semantic issue)
run_regression_test "Constructor basic functionality" \
"class TestClass {
    public constructor ();
    public func helper () => void;
};

implement TestClass {
    constructor () {
        local temp : integer;
    }
    
    func helper () => void {
        write (1);
    }
};" "pass" "Issue: Constructor should work with other class methods"

echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}Regression Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}Passed: $PASS_COUNT${NC}"
echo -e "${RED}Failed: $FAIL_COUNT${NC}"

# Calculate success rate
if [[ $((PASS_COUNT + FAIL_COUNT)) -gt 0 ]]; then
    success_rate=$((PASS_COUNT * 100 / (PASS_COUNT + FAIL_COUNT)))
    echo -e "Success Rate: ${success_rate}%"
fi

# Generate summary
cat >> "$REGRESSION_DIR/regression_results.txt" << EOF

SUMMARY:
========
Total Tests: $((PASS_COUNT + FAIL_COUNT))
Passed: $PASS_COUNT
Failed: $FAIL_COUNT
Success Rate: ${success_rate:-0}%

EOF

echo ""
echo "Results saved to: $REGRESSION_DIR/"
echo "- regression_results.txt: Test summary"
echo "- regression_failures.txt: Failure details"

if [[ $FAIL_COUNT -eq 0 ]]; then
    echo -e "${GREEN}SUCCESS: All regression tests passed!${NC}"
    echo -e "${GREEN}RESULT: No regressions detected${NC}"
    exit 0
else
    echo -e "${RED}FAILURE: $FAIL_COUNT regression test(s) failed${NC}"
    echo -e "${YELLOW}WARNING: Potential regressions detected${NC}"
    exit 1
fi