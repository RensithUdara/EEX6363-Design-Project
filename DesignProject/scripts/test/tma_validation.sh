#!/bin/bash
# Comprehensive TMA Validation Tests
# Tests all predefined rules from TMA1, TMA2, and TMA3

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

# Initialize test environment
mkdir -p tests/validation
mkdir -p test_reports/tma_validation

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Compiler detection
COMPILER=""
if [[ -f "compiler.exe" ]]; then
    COMPILER="./compiler.exe"
elif [[ -f "build/compiler" ]]; then
    COMPILER="./build/compiler"
elif [[ -f "build/compiler.exe" ]]; then
    COMPILER="./build/compiler.exe"
else
    echo "Error: Compiler not found. Building..."
    make clean && make
    COMPILER="./compiler.exe"
fi

echo -e "${BLUE}Using compiler: $COMPILER${NC}"

# Test functions
run_test() {
    local test_name="$1"
    local test_file="$2"
    local expected_result="$3"  # "pass" or "fail"
    
    ((TOTAL_TESTS++))
    
    if [[ "$expected_result" == "pass" ]]; then
        if $COMPILER "$test_file" > /dev/null 2>&1; then
            echo -e "✅ ${GREEN}PASS${NC}: $test_name"
            ((PASSED_TESTS++))
            return 0
        else
            echo -e "❌ ${RED}FAIL${NC}: $test_name (should have passed)"
            ((FAILED_TESTS++))
            return 1
        fi
    else
        if ! $COMPILER "$test_file" > /dev/null 2>&1; then
            echo -e "✅ ${GREEN}PASS${NC}: $test_name (correctly rejected)"
            ((PASSED_TESTS++))
            return 0
        else
            echo -e "❌ ${RED}FAIL${NC}: $test_name (should have failed)"
            ((FAILED_TESTS++))
            return 1
        fi
    fi
}

# TMA1 Validation Tests
echo -e "\n${BLUE}=== TMA1 LEXICAL ANALYSIS VALIDATION ===${NC}"

# TMA1.1: Keyword Recognition
cat > tests/validation/tma1_keywords.txt << 'EOF'
class TestKeywords {
    public constructor();
    public func testMethod() => integer;
};

implement TestKeywords {
    constructor() {
        local temp: integer;
        temp := 1;
    }
    
    func testMethod() => integer {
        if (1 > 0) then {
            return 42;
        } else {
            return 0;
        }
    }
};
EOF

run_test "TMA1.1 - All Keywords Recognition" "tests/validation/tma1_keywords.txt" "pass"

# TMA1.2: Operators and Symbols
cat > tests/validation/tma1_operators.txt << 'EOF'
func testOperators() => integer {
    local x, y, z: integer;
    x := 10;
    y := 20;
    z := (x + y) * 2 - 5 / 2;
    
    if (x >= y && z != 0) then {
        return z;
    }
    return 0;
}
EOF

run_test "TMA1.2 - All Operators Recognition" "tests/validation/tma1_operators.txt" "pass"

# TMA1.3: Number Literals
cat > tests/validation/tma1_numbers.txt << 'EOF'
func testNumbers() => integer {
    local a, b, c: integer;
    a := 123;
    b := 0;
    c := 999;
    return a + b + c;
}
EOF

run_test "TMA1.3 - Number Literals" "tests/validation/tma1_numbers.txt" "pass"

# TMA1.4: Identifiers
cat > tests/validation/tma1_identifiers.txt << 'EOF'
class MyClass123 {
    public func myMethod_test() => integer;
};

implement MyClass123 {
    func myMethod_test() => integer {
        local variable_name_123: integer;
        variable_name_123 := 42;
        return variable_name_123;
    }
};
EOF

run_test "TMA1.4 - Valid Identifiers" "tests/validation/tma1_identifiers.txt" "pass"

# TMA1.5: Invalid Character Error
cat > tests/validation/tma1_invalid_char.txt << 'EOF'
func testInvalid() => integer {
    local @invalid_symbol: integer;
    return 1;
}
EOF

run_test "TMA1.5 - Invalid Character Detection" "tests/validation/tma1_invalid_char.txt" "fail"

# TMA2 Validation Tests
echo -e "\n${BLUE}=== TMA2 SYNTAX ANALYSIS VALIDATION ===${NC}"

# TMA2.1: Class Definition Grammar
cat > tests/validation/tma2_class_grammar.txt << 'EOF'
class CompleteClass {
    public constructor();
    public func method1(param1: integer) => integer;
    public func method2() => integer;
};

implement CompleteClass {
    constructor() {
        local x: integer;
        x := 100;
        write(x);
    }
    
    func method1(param1: integer) => integer {
        return param1 * 2;
    }
    
    func method2() => integer {
        return 42;
    }
};
EOF

run_test "TMA2.1 - Complete Class Grammar" "tests/validation/tma2_class_grammar.txt" "pass"

# TMA2.2: Function Grammar
cat > tests/validation/tma2_function_grammar.txt << 'EOF'
func simpleFunction() => integer {
    return 1;
}

func functionWithParams(a: integer, b: integer) => integer {
    local result: integer;
    result := a + b;
    return result;
}

func functionWithLocalVars() => integer {
    local x, y, z: integer;
    x := 1;
    y := 2;
    z := x + y;
    return z;
}
EOF

run_test "TMA2.2 - Function Grammar Variations" "tests/validation/tma2_function_grammar.txt" "pass"

# TMA2.3: Statement Grammar
cat > tests/validation/tma2_statements.txt << 'EOF'
func testStatements() => integer {
    local x, y: integer;
    
    // Assignment statements
    x := 10;
    y := x + 5;
    
    // Conditional statements
    if (x > y) then {
        return x;
    } else {
        return y;
    }
    
    // While loops
    while (x > 0) {
        x := x - 1;
    }
    
    return x;
}
EOF

run_test "TMA2.3 - Statement Grammar" "tests/validation/tma2_statements.txt" "pass"

# TMA2.4: Expression Grammar
cat > tests/validation/tma2_expressions.txt << 'EOF'
func testExpressions() => integer {
    local a, b, c, result: integer;
    
    a := 10;
    b := 20;
    c := 5;
    
    // Complex expressions with precedence
    result := (a + b) * c - (a - b) / 2;
    
    // Boolean expressions
    if ((a > b) && (b < c)) || (a == c) then {
        result := result + 1;
    }
    
    return result;
}
EOF

run_test "TMA2.4 - Expression Grammar & Precedence" "tests/validation/tma2_expressions.txt" "pass"

# TMA2.5: Syntax Error Detection - Missing Semicolon
cat > tests/validation/tma2_missing_semicolon.txt << 'EOF'
func testError() => integer {
    local x: integer
    x := 42;
    return x;
}
EOF

run_test "TMA2.5 - Missing Semicolon Error" "tests/validation/tma2_missing_semicolon.txt" "fail"

# TMA2.6: Syntax Error Detection - Unmatched Braces
cat > tests/validation/tma2_unmatched_braces.txt << 'EOF'
func testError() => integer {
    local x: integer;
    if (x > 0) then {
        return 1;
    
    return 0;
}
EOF

run_test "TMA2.6 - Unmatched Braces Error" "tests/validation/tma2_unmatched_braces.txt" "fail"

# TMA3 Validation Tests  
echo -e "\n${BLUE}=== TMA3 SEMANTIC ANALYSIS VALIDATION ===${NC}"

# TMA3.1: Symbol Table - Valid Declarations
cat > tests/validation/tma3_symbol_table.txt << 'EOF'
class TestClass {
    public func testMethod() => integer;
};

implement TestClass {
    func testMethod() => integer {
        local x: integer;
        local y: integer;
        
        x := 42;
        y := x + 10;
        
        return y;
    }
};

func globalFunction() => integer {
    local z: integer;
    z := 100;
    return z;
}
EOF

run_test "TMA3.1 - Symbol Table Management" "tests/validation/tma3_symbol_table.txt" "pass"

# TMA3.2: Type Checking - Compatible Types
cat > tests/validation/tma3_type_checking.txt << 'EOF'
func testTypes() => integer {
    local intVar1: integer;
    local intVar2: integer;
    local result: integer;
    
    intVar1 := 25;
    intVar2 := 17;
    result := intVar1 + intVar2;
    
    return result;
}
EOF

run_test "TMA3.2 - Type Compatibility" "tests/validation/tma3_type_checking.txt" "pass"

# TMA3.3: Scope Management - Nested Scopes
cat > tests/validation/tma3_scope_management.txt << 'EOF'
func testScopes() => integer {
    local outerVar: integer;
    
    outerVar := 10;
    
    if (outerVar > 0) then {
        local innerVar: integer;
        innerVar := outerVar * 2;
        outerVar := innerVar;
    }
    
    return outerVar;
}
EOF

run_test "TMA3.3 - Scope Management" "tests/validation/tma3_scope_management.txt" "pass"

# TMA3.4: Semantic Error - Undeclared Variable
cat > tests/validation/tma3_undeclared_var.txt << 'EOF'
func testError() => integer {
    local x: integer;
    x := undeclared_variable + 5;
    return x;
}
EOF

run_test "TMA3.4 - Undeclared Variable Error" "tests/validation/tma3_undeclared_var.txt" "fail"

# TMA3.5: Semantic Error - Type Mismatch (if compiler supports multiple types)
cat > tests/validation/tma3_type_mismatch.txt << 'EOF'
func testError() => integer {
    local x: integer;
    x := "string_literal";
    return x;
}
EOF

run_test "TMA3.5 - Type Mismatch Error" "tests/validation/tma3_type_mismatch.txt" "fail"

# Code Generation Tests
echo -e "\n${BLUE}=== CODE GENERATION VALIDATION ===${NC}"

# CG.1: Simple Function Code Generation
cat > tests/validation/cg_simple_function.txt << 'EOF'
func simple() => integer {
    return 42;
}
EOF

run_test "CG.1 - Simple Function Generation" "tests/validation/cg_simple_function.txt" "pass"

# CG.2: Function with Local Variables
cat > tests/validation/cg_local_variables.txt << 'EOF'
func withLocals() => integer {
    local x, y, z: integer;
    
    x := 10;
    y := 20;
    z := x + y;
    
    return z;
}
EOF

run_test "CG.2 - Local Variables Generation" "tests/validation/cg_local_variables.txt" "pass"

# CG.3: Function with Parameters
cat > tests/validation/cg_parameters.txt << 'EOF'
func withParams(a: integer, b: integer) => integer {
    local result: integer;
    result := a * b;
    return result;
}
EOF

run_test "CG.3 - Parameter Handling" "tests/validation/cg_parameters.txt" "pass"

# CG.4: Control Flow Code Generation
cat > tests/validation/cg_control_flow.txt << 'EOF'
func controlFlow(n: integer) => integer {
    if (n <= 0) then {
        return 0;
    } else {
        return n + controlFlow(n - 1);
    }
}
EOF

run_test "CG.4 - Control Flow Generation" "tests/validation/cg_control_flow.txt" "pass"

# Integration Tests
echo -e "\n${BLUE}=== INTEGRATION VALIDATION ===${NC}"

# INT.1: Complete Class Implementation
cat > tests/validation/integration_complete.txt << 'EOF'
class Calculator {
    public constructor();
    public func add(a: integer, b: integer) => integer;
    public func multiply(a: integer, b: integer) => integer;
};

implement Calculator {
    constructor() {
        local msg: integer;
        msg := 1;
        write(msg);
    }
    
    func add(a: integer, b: integer) => integer {
        return a + b;
    }
    
    func multiply(a: integer, b: integer) => integer {
        local result: integer;
        result := a * b;
        return result;
    }
};
EOF

run_test "INT.1 - Complete Class Integration" "tests/validation/integration_complete.txt" "pass"

# Generate comprehensive report
echo -e "\n${BLUE}=== VALIDATION SUMMARY ===${NC}"
echo -e "Total Tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

SUCCESS_RATE=$((PASSED_TESTS * 100 / TOTAL_TESTS))
echo -e "Success Rate: ${SUCCESS_RATE}%"

# Write detailed report
{
    echo "TMA Validation Test Report"
    echo "Generated: $(date)"
    echo "========================="
    echo ""
    echo "Test Summary:"
    echo "Total Tests: $TOTAL_TESTS"
    echo "Passed: $PASSED_TESTS"
    echo "Failed: $FAILED_TESTS"
    echo "Success Rate: ${SUCCESS_RATE}%"
    echo ""
    echo "Test Categories:"
    echo "- TMA1 (Lexical Analysis): 5 tests"
    echo "- TMA2 (Syntax Analysis): 6 tests"  
    echo "- TMA3 (Semantic Analysis): 5 tests"
    echo "- Code Generation: 4 tests"
    echo "- Integration: 1 test"
    echo ""
    echo "Compiler Used: $COMPILER"
} > test_reports/tma_validation/validation_report.txt

if [[ $FAILED_TESTS -eq 0 ]]; then
    echo -e "\n🎉 ${GREEN}All TMA validation tests passed!${NC}"
    exit 0
else
    echo -e "\n⚠️ ${RED}$FAILED_TESTS validation tests failed.${NC}"
    echo "See test_reports/tma_validation/validation_report.txt for details"
    exit 1
fi