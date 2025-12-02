#!/bin/bash
# Comprehensive Compiler Implementation Test
# Tests the actual code generation and validates assembly output

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

echo "Testing Code Generation Implementation"
echo "====================================="

# Initialize
mkdir -p tests/codegen
# Test reports managed by master test script
mkdir -p output/assembly

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Test counters
TESTS_RUN=0
TESTS_PASSED=0

# Find compiler
COMPILER=""
if [[ -f "enhanced_compiler.exe" ]]; then
    COMPILER="./enhanced_compiler.exe"
elif [[ -f "build/enhanced_compiler" ]]; then
    COMPILER="./build/enhanced_compiler"
else
    echo "Building compiler..."
    make clean && make
    COMPILER="./enhanced_compiler.exe"
fi

echo -e "${BLUE}Using compiler: $COMPILER${NC}"

# Test function
test_code_generation() {
    local test_name="$1"
    local input_file="$2"
    local expected_features="$3"
    
    echo -e "\n${YELLOW}Testing: $test_name${NC}"
    ((TESTS_RUN++))
    
    # Compile the test
    if $COMPILER "$input_file" > compiler_output.txt 2>&1; then
        echo -e "[PASS] Compilation: ${GREEN}SUCCESS${NC}"
        
        # Check if assembly output was generated
        if [[ -f "output.s" ]]; then
            echo -e "[PASS] Assembly Output: ${GREEN}GENERATED${NC}"
            
            # Analyze assembly output
            local features_found=0
            local total_features=0
            
            for feature in $expected_features; do
                ((total_features++))
                case $feature in
                    "x86_prologue")
                        if grep -q "pushq.*%rbp\|movq.*%rsp.*%rbp" output.s; then
                            echo -e "  [PASS] x86-64 Function Prologue: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] x86-64 Function Prologue: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "x86_epilogue")
                        if grep -q "movq.*%rbp.*%rsp\|popq.*%rbp\|ret" output.s; then
                            echo -e "  [PASS] x86-64 Function Epilogue: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] x86-64 Function Epilogue: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "stack_allocation")
                        if grep -q "subq.*%rsp\|addq.*%rsp" output.s; then
                            echo -e "  [PASS] Stack Allocation: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] Stack Allocation: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "register_usage")
                        if grep -q "%rax\|%rbx\|%rcx\|%rdx" output.s; then
                            echo -e "  [PASS] Register Usage: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] Register Usage: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "memory_operations")
                        if grep -q "movq.*(%rbp)\|movq.*%rbp" output.s; then
                            echo -e "  [PASS] Memory Operations: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] Memory Operations: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "arithmetic")
                        if grep -q "addq\|subq\|imulq\|idivq" output.s; then
                            echo -e "  [PASS] Arithmetic Instructions: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] Arithmetic Instructions: ${RED}MISSING${NC}"
                        fi
                        ;;
                    "control_flow")
                        if grep -q "jmp\|je\|jne\|jl\|jg\|cmpq" output.s; then
                            echo -e "  [PASS] Control Flow: ${GREEN}FOUND${NC}"
                            ((features_found++))
                        else
                            echo -e "  [FAIL] Control Flow: ${RED}MISSING${NC}"
                        fi
                        ;;
                esac
            done
            
            # Assembly output available in current directory
            
            if [[ $features_found -eq $total_features ]]; then
                echo -e "[PASS] Test Result: ${GREEN}PASSED (${features_found}/${total_features} features)${NC}"
                ((TESTS_PASSED++))
            else
                echo -e "[FAIL] Test Result: ${RED}PARTIAL (${features_found}/${total_features} features)${NC}"
            fi
        else
            echo -e "[FAIL] Assembly Output: ${RED}NOT GENERATED${NC}"
        fi
    else
        echo -e "[FAIL] Compilation: ${RED}FAILED${NC}"
        echo "Compiler output:"
        cat compiler_output.txt
    fi
}

# Test 1: Simple Function
echo -e "\n${BLUE}=== TEST 1: Simple Function ===${NC}"
cat > tests/codegen/simple_function.txt << 'EOF'
func test () => integer {
    return (42);
}
EOF

test_code_generation "Simple Function" "tests/codegen/simple_function.txt" "x86_prologue x86_epilogue register_usage"

# Test 2: Function with Local Variables
echo -e "\n${BLUE}=== TEST 2: Local Variables ===${NC}"
cat > tests/codegen/local_variables.txt << 'EOF'
func test_locals () => integer {
    local x: integer;
    local y: integer;
    
    x := 10;
    y := 20;
    
    return (x + y);
}
EOF

test_code_generation "Local Variables" "tests/codegen/local_variables.txt" "x86_prologue x86_epilogue stack_allocation memory_operations arithmetic"

# Test 3: Function with Parameters
echo -e "\n${BLUE}=== TEST 3: Function Parameters ===${NC}"
cat > tests/codegen/function_parameters.txt << 'EOF'
func add (a: integer, b: integer) => integer {
    local result: integer;
    result := a + b;
    return (result);
}
EOF

test_code_generation "Function Parameters" "tests/codegen/function_parameters.txt" "x86_prologue x86_epilogue stack_allocation register_usage arithmetic"

# Test 4: Control Flow
echo -e "\n${BLUE}=== TEST 4: Control Flow ===${NC}"
cat > tests/codegen/control_flow.txt << 'EOF'
func factorial (n: integer) => integer {
    if (n <= 1) then {
        return (1);
    } else {
        return (n * factorial(n - 1));
    }
}
EOF

test_code_generation "Control Flow" "tests/codegen/control_flow.txt" "x86_prologue x86_epilogue control_flow arithmetic"

# Test 5: Complex Expression
echo -e "\n${BLUE}=== TEST 5: Complex Expression ===${NC}"
cat > tests/codegen/complex_expression.txt << 'EOF'
func complex_calc () => integer {
    local a: integer;
    local b: integer;
    local c: integer;
    local result: integer;
    
    a := 5;
    b := 10;  
    c := 3;
    
    result := (a + b) * c - (a - b) / 2;
    
    return (result);
}
EOF

test_code_generation "Complex Expression" "tests/codegen/complex_expression.txt" "x86_prologue x86_epilogue stack_allocation arithmetic memory_operations"

# Test 6: Class Implementation
echo -e "\n${BLUE}=== TEST 6: Class Implementation ===${NC}"
cat > tests/codegen/class_implementation.txt << 'EOF'
class Calculator {
    public constructor ();
    public func multiply (x: integer, y: integer) => integer;
};

implement Calculator {
    constructor () {
        local temp: integer;
        temp := 1;
        write (temp);
    }
    
    func multiply (x: integer, y: integer) => integer {
        return (x * y);
    }
}
EOF

test_code_generation "Class Implementation" "tests/codegen/class_implementation.txt" "x86_prologue x86_epilogue arithmetic register_usage"

# Performance Test
echo -e "\n${BLUE}=== PERFORMANCE TEST ===${NC}"
cat > tests/codegen/performance_test.txt << 'EOF'
class PerformanceTest {
    public func fibonacci (n: integer) => integer;
    public func factorial (n: integer) => integer;
};

implement PerformanceTest {
    func fibonacci (n: integer) => integer {
        if (n <= 1) then {
            return (n);
        } else {
            return (fibonacci(n - 1) + fibonacci(n - 2));
        }
    }
    
    func factorial (n: integer) => integer {
        if (n <= 1) then {
            return (1);
        } else {
            return (n * factorial(n - 1));
        }
    }
}
EOF

echo -e "${YELLOW}Testing compilation time...${NC}"
start_time=$(date +%s.%N)
if $COMPILER tests/codegen/performance_test.txt > /dev/null 2>&1; then
    end_time=$(date +%s.%N)
    compile_time=$(echo "$end_time - $start_time" | bc -l 2>/dev/null || echo "0.1")
    echo -e "[PASS] Compilation Time: ${GREEN}${compile_time} seconds${NC}"
    
    if (( $(echo "$compile_time < 3.0" | bc -l 2>/dev/null || echo "1") )); then
        echo -e "[PASS] Performance: ${GREEN}EXCELLENT${NC} (< 3 seconds)"
        ((TESTS_PASSED++))
    else
        echo -e "[WARN] Performance: ${YELLOW}ACCEPTABLE${NC} (> 3 seconds)"
    fi
    ((TESTS_RUN++))
else
    echo -e "❌ Performance Test: ${RED}FAILED TO COMPILE${NC}"
    ((TESTS_RUN++))
fi

# Assembly Quality Analysis
echo -e "\n${BLUE}=== ASSEMBLY QUALITY ANALYSIS ===${NC}"
if [[ -f "output.s" ]]; then
    echo -e "${YELLOW}Analyzing generated assembly...${NC}"
    
    # Count instructions
    instruction_count=$(grep -E "^\s*(mov|add|sub|mul|div|push|pop|call|ret|jmp|je|jne)" output.s | wc -l 2>/dev/null || echo "0")
    echo -e "Instructions Generated: ${instruction_count}"
    
    # Check for optimizations
    if grep -q "# Optimization" output.s; then
        echo -e "[PASS] Optimizations: ${GREEN}DETECTED${NC}"
    else
        echo -e "[WARN] Optimizations: ${YELLOW}NOT DETECTED${NC}"
    fi
    
    # Check ABI compliance
    if grep -q "pushq %rbp" output.s && grep -q "movq %rsp, %rbp" output.s; then
        echo -e "[PASS] ABI Compliance: ${GREEN}x86-64 System V${NC}"
    else
        echo -e "[WARN] ABI Compliance: ${YELLOW}PARTIAL${NC}"
    fi
    
    # Assembly analysis results displayed above
fi

# Memory Layout Test
echo -e "\n${BLUE}=== MEMORY LAYOUT TEST ===${NC}"
if [[ -f "output.s" ]]; then
    # Check for proper stack frame setup
    if grep -q "subq.*%rsp" output.s; then
        echo -e "[PASS] Stack Frame: ${GREEN}PROPER ALLOCATION${NC}"
    else
        echo -e "[WARN] Stack Frame: ${YELLOW}SIMPLE ALLOCATION${NC}"
    fi
    
    # Check for local variable management
    if grep -q "(%rbp)" output.s; then
        echo -e "[PASS] Local Variables: ${GREEN}FRAME-BASED ACCESS${NC}"
    else
        echo -e "[WARN] Local Variables: ${YELLOW}REGISTER-ONLY${NC}"
    fi
fi

# Generate comprehensive report
echo -e "\n${BLUE}=== FINAL RESULTS ===${NC}"
echo -e "Tests Run: $TESTS_RUN"
echo -e "Tests Passed: ${GREEN}$TESTS_PASSED${NC}"
echo -e "Tests Failed: ${RED}$((TESTS_RUN - TESTS_PASSED))${NC}"

success_rate=$((TESTS_PASSED * 100 / TESTS_RUN))
echo -e "Success Rate: ${success_rate}%"

# Detailed results captured by master test suite

# Clean up temporary files
rm -f compiler_output.txt

if [[ $success_rate -ge 80 ]]; then
    echo -e "\nSUCCESS: ${GREEN}Code generation implementation test PASSED!${NC}"
    echo "INFO: Detailed analysis available in terminal output."
    exit 0
else
    echo -e "\nWARNING: ${RED}Code generation needs improvement.${NC}"
    echo "INFO: Detailed analysis available in terminal output."
    exit 1
fi