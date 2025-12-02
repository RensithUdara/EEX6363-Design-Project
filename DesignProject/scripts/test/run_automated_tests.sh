#!/bin/bash
# Automated Testing Suite for Design Project Compiler
# Tests lexical, syntax, semantic, and code generation phases

set -e  # Exit on first error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Project paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
COMPILER="$PROJECT_ROOT/build/compiler"
TEST_DIR="$PROJECT_ROOT/tests"
EXAMPLES_DIR="$TEST_DIR/examples"
UNIT_DIR="$TEST_DIR/unit"
REPORTS_DIR="$PROJECT_ROOT/test_reports"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Design Project Automated Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"
echo "Project Root: $PROJECT_ROOT"
echo "Test Directory: $TEST_DIR"
echo "Compiler: $COMPILER"
echo ""

# Create reports directory
mkdir -p "$REPORTS_DIR"

# Function to run a single test
run_test() {
    local test_file="$1"
    local test_name="$2"
    local expected_result="$3"  # "pass" or "fail"
    
    echo -n "Testing $test_name... "
    ((TOTAL_TESTS++))
    
    # Run the compiler and capture output
    local output
    local exit_code
    
    cd "$PROJECT_ROOT"
    output=$($COMPILER "$test_file" 2>&1) || exit_code=$?
    
    # Determine if test passed based on expected result and actual exit code
    local test_passed=false
    
    if [[ "$expected_result" == "pass" && ($exit_code -eq 0 || -z "$exit_code") ]]; then
        test_passed=true
    elif [[ "$expected_result" == "fail" && $exit_code -ne 0 ]]; then
        test_passed=true
    fi
    
    # Report result
    if $test_passed; then
        echo -e "${GREEN}PASS${NC}"
        ((PASSED_TESTS++))
        echo "$test_name: PASS" >> "$REPORTS_DIR/test_summary.txt"
    else
        echo -e "${RED}FAIL${NC}"
        ((FAILED_TESTS++))
        echo "$test_name: FAIL (Exit code: $exit_code)" >> "$REPORTS_DIR/test_summary.txt"
        echo "Output: $output" >> "$REPORTS_DIR/test_failures.txt"
    fi
}

# Function to test specific compiler phases
test_phase() {
    local phase_name="$1"
    local test_file="$2"
    local expected_result="$3"
    
    echo -e "\n${YELLOW}=== Testing $phase_name Phase ===${NC}"
    run_test "$test_file" "$phase_name" "$expected_result"
}

# Initialize report files
echo "Design Project Automated Test Results" > "$REPORTS_DIR/test_summary.txt"
echo "Generated: $(date)" >> "$REPORTS_DIR/test_summary.txt"
echo "============================================" >> "$REPORTS_DIR/test_summary.txt"
echo "" > "$REPORTS_DIR/test_failures.txt"

# Check if compiler exists
if [[ ! -f "$COMPILER" ]]; then
    echo -e "${RED}Error: Compiler not found at $COMPILER${NC}"
    echo "Please run 'make' to build the compiler first."
    exit 1
fi

echo -e "${BLUE}=== PHASE 1: Basic Functionality Tests ===${NC}"

# Test 1: Simple valid program
if [[ -f "$EXAMPLES_DIR/simple_test.txt" ]]; then
    run_test "$EXAMPLES_DIR/simple_test.txt" "Simple Valid Program" "pass"
fi

# Test 2: Constructor syntax (the fix we implemented)
if [[ -f "$PROJECT_ROOT/constructor_test.txt" ]] || [[ -f "$EXAMPLES_DIR/constructor_test.txt" ]]; then
    # Create constructor test if it doesn't exist
    cat > "$PROJECT_ROOT/temp_constructor_test.txt" << 'EOF'
class TestClass {
    public constructor();
    public func getValue() => integer;
};

implement TestClass {
    constructor() {
        local temp : integer;
        temp := 1;
        write(temp);
    }
    
    func getValue() => integer {
        return(42);
    }
};
EOF
    run_test "$PROJECT_ROOT/temp_constructor_test.txt" "Constructor Syntax Test" "pass"
    rm -f "$PROJECT_ROOT/temp_constructor_test.txt"
fi

# Test 3: Minimal valid program
if [[ -f "$EXAMPLES_DIR/minimal_test.txt" ]]; then
    run_test "$EXAMPLES_DIR/minimal_test.txt" "Minimal Valid Program" "pass"
fi

echo -e "\n${BLUE}=== PHASE 2: Error Handling Tests ===${NC}"

# Test 4: Syntax error detection
if [[ -f "$EXAMPLES_DIR/error_test.txt" ]]; then
    run_test "$EXAMPLES_DIR/error_test.txt" "Syntax Error Detection" "fail"
fi

# Test 5: Create a test with deliberate syntax error
cat > "$PROJECT_ROOT/temp_syntax_error_test.txt" << 'EOF'
class BadClass {
    public func missingArrow() integer;  // Missing => 
};
EOF
run_test "$PROJECT_ROOT/temp_syntax_error_test.txt" "Deliberate Syntax Error" "fail"
rm -f "$PROJECT_ROOT/temp_syntax_error_test.txt"

echo -e "\n${BLUE}=== PHASE 3: Comprehensive Language Feature Tests ===${NC}"

# Test 6: Class and inheritance
cat > "$PROJECT_ROOT/temp_class_test.txt" << 'EOF'
class BaseClass {
    public func baseMethod() => void;
};

class DerivedClass isa BaseClass {
    public constructor();
    public func derivedMethod() => integer;
};

implement DerivedClass {
    constructor() {
        local x : integer;
        x := 1;
    }
    
    func derivedMethod() => integer {
        return(10);
    }
    
    func baseMethod() => void {
        write(1);
    }
};
EOF
run_test "$PROJECT_ROOT/temp_class_test.txt" "Class and Inheritance" "pass"
rm -f "$PROJECT_ROOT/temp_class_test.txt"

# Test 7: Arrays and expressions
cat > "$PROJECT_ROOT/temp_array_test.txt" << 'EOF'
func arrayTest() => void {
    local arr : integer[5];
    local i : integer;
    
    arr[0] := 10;
    arr[1] := 20;
    arr[2] := arr[0] + arr[1];
    
    write(arr[2]);
}
EOF
run_test "$PROJECT_ROOT/temp_array_test.txt" "Arrays and Expressions" "pass"
rm -f "$PROJECT_ROOT/temp_array_test.txt"

# Test 8: Control structures
cat > "$PROJECT_ROOT/temp_control_test.txt" << 'EOF'
func controlTest() => void {
    local x : integer;
    local y : integer;
    
    x := 5;
    y := 10;
    
    if (x < y) then {
        write(x);
    } else {
        write(y);
    };
    
    while (x < y) {
        x := x + 1;
        write(x);
    };
}
EOF
run_test "$PROJECT_ROOT/temp_control_test.txt" "Control Structures" "pass"
rm -f "$PROJECT_ROOT/temp_control_test.txt"

echo -e "\n${BLUE}=== PHASE 4: Edge Cases and Error Recovery ===${NC}"

# Test 9: Empty class
cat > "$PROJECT_ROOT/temp_empty_class_test.txt" << 'EOF'
class EmptyClass {
};

implement EmptyClass {
};
EOF
run_test "$PROJECT_ROOT/temp_empty_class_test.txt" "Empty Class" "pass"
rm -f "$PROJECT_ROOT/temp_empty_class_test.txt"

# Test 10: Multiple syntax errors
cat > "$PROJECT_ROOT/temp_multiple_errors_test.txt" << 'EOF'
class BadClass {
    public func method1() integer;  // Missing =>
    public func method2( => void;   // Missing param
    public badtoken;                // Invalid syntax
};
EOF
run_test "$PROJECT_ROOT/temp_multiple_errors_test.txt" "Multiple Syntax Errors" "fail"
rm -f "$PROJECT_ROOT/temp_multiple_errors_test.txt"

echo -e "\n${BLUE}=== PHASE 5: Existing Test Files ===${NC}"

# Run tests on existing example files
if [[ -d "$EXAMPLES_DIR" ]]; then
    for test_file in "$EXAMPLES_DIR"/*.txt; do
        if [[ -f "$test_file" ]]; then
            filename=$(basename "$test_file" .txt)
            # Assume files with "error" in name should fail, others should pass
            if [[ "$filename" == *"error"* ]]; then
                run_test "$test_file" "Existing: $filename" "fail"
            else
                run_test "$test_file" "Existing: $filename" "pass"
            fi
        fi
    done
fi

echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}Test Results Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total Tests: $TOTAL_TESTS"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"

# Calculate success percentage
if [[ $TOTAL_TESTS -gt 0 ]]; then
    success_rate=$((PASSED_TESTS * 100 / TOTAL_TESTS))
    echo -e "Success Rate: ${success_rate}%"
fi

echo ""
echo "Detailed results saved to:"
echo "  - Summary: $REPORTS_DIR/test_summary.txt"
echo "  - Failures: $REPORTS_DIR/test_failures.txt"

# Generate HTML report
cat > "$REPORTS_DIR/test_results.html" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Design Project Test Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background-color: #f0f0f0; padding: 10px; border-radius: 5px; }
        .pass { color: green; font-weight: bold; }
        .fail { color: red; font-weight: bold; }
        .summary { background-color: #e8f4fd; padding: 15px; border-radius: 5px; margin: 10px 0; }
    </style>
</head>
<body>
    <div class="header">
        <h1>Design Project Automated Test Results</h1>
        <p>Generated: $(date)</p>
    </div>
    
    <div class="summary">
        <h2>Summary</h2>
        <p>Total Tests: $TOTAL_TESTS</p>
        <p class="pass">Passed: $PASSED_TESTS</p>
        <p class="fail">Failed: $FAILED_TESTS</p>
        <p>Success Rate: ${success_rate}%</p>
    </div>
    
    <h2>Test Details</h2>
    <pre>$(cat "$REPORTS_DIR/test_summary.txt")</pre>
    
    <h2>Failure Details</h2>
    <pre>$(cat "$REPORTS_DIR/test_failures.txt")</pre>
</body>
</html>
EOF

echo "HTML report: $REPORTS_DIR/test_results.html"

# Final exit code based on results
if [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "\n${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "\n${YELLOW}⚠️  Some tests failed. Check the reports for details.${NC}"
    exit 1
fi