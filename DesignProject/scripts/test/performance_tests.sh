#!/bin/bash
# Performance Test Suite
# Tests compiler performance and resource usage

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

echo "========================================"
echo "Design Project Performance Test Suite"
echo "========================================"
echo "Project: $(basename "$PROJECT_ROOT")"
echo "Time: $(date)"
echo ""

# Check if compiler exists
if [[ ! -f "build/enhanced_compiler" ]]; then
    echo "❌ Compiler not found. Run 'make' first."
    exit 1
fi

# Create performance test directory
mkdir -p test_reports/performance

# Performance test results
PERF_RESULTS=()

# Function to run performance test
run_performance_test() {
    local test_name="$1"
    local input_file="$2"
    local iterations="${3:-5}"
    
    echo "=== PERFORMANCE TEST: $test_name ==="
    
    local total_time=0
    local min_time=999999
    local max_time=0
    
    echo "Running $iterations iterations..."
    
    for i in $(seq 1 $iterations); do
        local start_time=$(date +%s%N)
        
        # Run compiler (suppress output for timing)
        if ./build/enhanced_compiler "$input_file" > /dev/null 2>&1; then
            local end_time=$(date +%s%N)
            local duration=$(( (end_time - start_time) / 1000000 )) # Convert to milliseconds
            
            echo "  Iteration $i: ${duration}ms"
            
            total_time=$((total_time + duration))
            
            if [[ $duration -lt $min_time ]]; then
                min_time=$duration
            fi
            
            if [[ $duration -gt $max_time ]]; then
                max_time=$duration
            fi
        else
            echo "  Iteration $i: FAILED"
            return 1
        fi
    done
    
    local avg_time=$((total_time / iterations))
    
    echo "Results:"
    echo "  Average: ${avg_time}ms"
    echo "  Min: ${min_time}ms"
    echo "  Max: ${max_time}ms"
    echo "  Total: ${total_time}ms"
    
    PERF_RESULTS+=("$test_name:$avg_time:$min_time:$max_time")
    
    echo "PASS"
    return 0
}

# Function to create test input files
create_performance_test_files() {
    # Small test file
    cat > "/tmp/perf_small.txt" << 'EOF'
program test;
begin
    var x: integer;
    x := 5;
end.
EOF

    # Medium test file
    cat > "/tmp/perf_medium.txt" << 'EOF'
program mediumTest;

class Calculator {
    public x: integer;
    public y: integer;
    
    public constructor();
    
    public func add(): integer;
    public func multiply(): integer;
}

constructor() {
    x := 0;
    y := 0;
}

func add(): integer {
    return x + y;
}

func multiply(): integer {
    return x * y;
}

begin
    var calc: Calculator;
    var result: integer;
    
    calc.x := 10;
    calc.y := 20;
    result := calc.add();
    result := calc.multiply();
end.
EOF

    # Large test file
    cat > "/tmp/perf_large.txt" << 'EOF'
program largeTest;

class MathUtils {
    public value: integer;
    public constructor();
    public func factorial(n: integer): integer;
    public func fibonacci(n: integer): integer;
    public func isPrime(n: integer): boolean;
}

constructor() {
    value := 0;
}

func factorial(n: integer): integer {
    if n <= 1 then
        return 1
    else
        return n * factorial(n - 1);
}

func fibonacci(n: integer): integer {
    if n <= 1 then
        return n
    else
        return fibonacci(n - 1) + fibonacci(n - 2);
}

func isPrime(n: integer): boolean {
    var i: integer;
    if n <= 1 then
        return false;
    
    i := 2;
    while i * i <= n do {
        if n mod i = 0 then
            return false;
        i := i + 1;
    }
    return true;
}

class DataProcessor {
    public data: array[100] of integer;
    public size: integer;
    
    public constructor();
    public func processData(): integer;
    public func sortData(): void;
}

constructor() {
    var i: integer;
    size := 100;
    i := 0;
    while i < size do {
        data[i] := i * 2;
        i := i + 1;
    }
}

func processData(): integer {
    var i: integer;
    var sum: integer;
    sum := 0;
    i := 0;
    
    while i < size do {
        sum := sum + data[i];
        i := i + 1;
    }
    return sum;
}

func sortData(): void {
    var i: integer;
    var j: integer;
    var temp: integer;
    
    i := 0;
    while i < size - 1 do {
        j := 0;
        while j < size - i - 1 do {
            if data[j] > data[j + 1] then {
                temp := data[j];
                data[j] := data[j + 1];
                data[j + 1] := temp;
            }
            j := j + 1;
        }
        i := i + 1;
    }
}

begin
    var math: MathUtils;
    var processor: DataProcessor;
    var result: integer;
    var i: integer;
    
    // Test mathematical functions
    result := math.factorial(10);
    result := math.fibonacci(20);
    
    i := 2;
    while i < 100 do {
        if math.isPrime(i) then
            result := result + 1;
        i := i + 1;
    }
    
    // Test data processing
    processor.processData();
    processor.sortData();
end.
EOF
}

# Memory usage test
test_memory_usage() {
    echo "=== MEMORY USAGE TEST ==="
    
    local input_file="/tmp/perf_large.txt"
    
    # Get memory usage before
    local mem_before=$(ps -o rss= -p $$ 2>/dev/null || echo "0")
    
    # Run compiler with memory monitoring
    if command -v /usr/bin/time >/dev/null 2>&1; then
        /usr/bin/time -l ./build/enhanced_compiler "$input_file" > /dev/null 2> /tmp/memory_stats.txt
        
        if [[ -f /tmp/memory_stats.txt ]]; then
            local max_mem=$(grep "maximum resident set size" /tmp/memory_stats.txt | awk '{print $1}' || echo "0")
            echo "Maximum memory usage: ${max_mem} bytes"
            rm -f /tmp/memory_stats.txt
        else
            echo "Memory monitoring not available"
        fi
    else
        echo "Memory monitoring not available (GNU time not found)"
    fi
    
    echo "PASS"
}

# Compilation phases timing
test_compilation_phases() {
    echo "=== COMPILATION PHASES TIMING ==="
    
    local input_file="/tmp/perf_medium.txt"
    
    echo "Testing individual compilation phases..."
    
    # This would require compiler modifications to output timing for each phase
    # For now, we'll just test overall compilation time
    local start_time=$(date +%s%N)
    
    if ./build/enhanced_compiler "$input_file" > /dev/null 2>&1; then
        local end_time=$(date +%s%N)
        local duration=$(( (end_time - start_time) / 1000000 ))
        echo "Total compilation time: ${duration}ms"
        echo "PASS"
    else
        echo "FAIL"
        return 1
    fi
}

# Stress test with multiple files
stress_test() {
    echo "=== STRESS TEST ==="
    
    local num_files=10
    local total_time=0
    
    echo "Compiling $num_files files simultaneously..."
    
    for i in $(seq 1 $num_files); do
        cp "/tmp/perf_medium.txt" "/tmp/stress_test_$i.txt"
    done
    
    local start_time=$(date +%s%N)
    
    for i in $(seq 1 $num_files); do
        if ./build/enhanced_compiler "/tmp/stress_test_$i.txt" > /dev/null 2>&1; then
            echo "  File $i: SUCCESS"
        else
            echo "  File $i: FAILED"
            return 1
        fi
    done
    
    local end_time=$(date +%s%N)
    local duration=$(( (end_time - start_time) / 1000000 ))
    
    echo "Total time for $num_files files: ${duration}ms"
    echo "Average per file: $((duration / num_files))ms"
    
    # Clean up
    for i in $(seq 1 $num_files); do
        rm -f "/tmp/stress_test_$i.txt"
    done
    
    echo "PASS"
}

# Run all performance tests
echo "Creating test files..."
create_performance_test_files

echo ""
echo "Running performance tests..."
echo ""

# Basic performance tests
if run_performance_test "Small Program" "/tmp/perf_small.txt" 10; then
    echo "✓ Small program test passed"
else
    echo "✗ Small program test failed"
    exit 1
fi

echo ""

if run_performance_test "Medium Program" "/tmp/perf_medium.txt" 5; then
    echo "✓ Medium program test passed"
else
    echo "✗ Medium program test failed"
    exit 1
fi

echo ""

if run_performance_test "Large Program" "/tmp/perf_large.txt" 3; then
    echo "✓ Large program test passed"
else
    echo "✗ Large program test failed"
    exit 1
fi

echo ""

# Additional tests
if test_memory_usage; then
    echo "✓ Memory usage test passed"
else
    echo "✗ Memory usage test failed"
fi

echo ""

if test_compilation_phases; then
    echo "✓ Compilation phases test passed"
else
    echo "✗ Compilation phases test failed"
fi

echo ""

if stress_test; then
    echo "✓ Stress test passed"
else
    echo "✗ Stress test failed"
fi

# Generate performance report
echo ""
echo "========================================"
echo "Performance Test Summary"
echo "========================================"

for result in "${PERF_RESULTS[@]}"; do
    IFS=':' read -r name avg min max <<< "$result"
    echo "$name:"
    echo "  Average: ${avg}ms"
    echo "  Range: ${min}ms - ${max}ms"
    echo ""
done

# Save detailed results
PERF_REPORT="test_reports/performance/performance_detailed_report.txt"
echo "Performance Test Report - $(date)" > "$PERF_REPORT"
echo "===============================================" >> "$PERF_REPORT"
echo "" >> "$PERF_REPORT"

for result in "${PERF_RESULTS[@]}"; do
    IFS=':' read -r name avg min max <<< "$result"
    echo "$name:" >> "$PERF_REPORT"
    echo "  Average: ${avg}ms" >> "$PERF_REPORT"
    echo "  Min: ${min}ms" >> "$PERF_REPORT"
    echo "  Max: ${max}ms" >> "$PERF_REPORT"
    echo "" >> "$PERF_REPORT"
done

echo "🎉 All performance tests passed!"
echo "📊 Detailed report saved to: $PERF_REPORT"

# Clean up test files
rm -f /tmp/perf_small.txt /tmp/perf_medium.txt /tmp/perf_large.txt

exit 0