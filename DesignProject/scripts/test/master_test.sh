#!/bin/bash
# Master Test Runner - Coordinates all testing activities
# Usage: ./master_test.sh [quick|full|ci|unit|regression]

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TEST_MODE="${1:-full}"

cd "$PROJECT_ROOT"

echo -e "${PURPLE}╔════════════════════════════════════════╗${NC}"
echo -e "${PURPLE}║     Design Project Master Test Suite   ║${NC}"
echo -e "${PURPLE}╚════════════════════════════════════════╝${NC}"
echo ""
echo -e "Mode: ${BLUE}$TEST_MODE${NC}"
echo -e "Project: $(basename "$PROJECT_ROOT")"
echo -e "Time: $(date)"
echo ""

# Ensure scripts are executable
chmod +x scripts/test/*.sh

# Initialize master results
mkdir -p test_reports/master
MASTER_LOG="test_reports/master/master_test_log.txt"
echo "Master Test Run - $(date)" > "$MASTER_LOG"
echo "Mode: $TEST_MODE" >> "$MASTER_LOG"
echo "=======================================" >> "$MASTER_LOG"

# Function to run a test suite
run_test_suite() {
    local suite_name="$1"
    local script_path="$2"
    local required="$3"  # true/false
    local output_file="$4"  # optional separate output file
    
    echo -e "\n${YELLOW}▶ Running $suite_name...${NC}"
    echo "Running $suite_name..." >> "$MASTER_LOG"
    
    local start_time=$(date +%s)
    local exit_code=0
    
    # Create output file if specified
    if [[ -n "$output_file" ]]; then
        mkdir -p "$(dirname "$output_file")"
        echo "$suite_name Test Results - $(date)" > "$output_file"
        echo "=========================================" >> "$output_file"
    fi
    
    if [[ -f "$script_path" ]]; then
        if [[ -n "$output_file" ]]; then
            # Run with output redirection
            if "$script_path" 2>&1 | tee -a "$output_file"; then
                local end_time=$(date +%s)
                local duration=$((end_time - start_time))
                echo -e "${GREEN}✓ $suite_name completed successfully (${duration}s)${NC}"
                echo "✓ $suite_name: SUCCESS (${duration}s)" >> "$MASTER_LOG"
                echo "\nTest completed successfully in ${duration}s" >> "$output_file"
                return 0
            else
                local end_time=$(date +%s)
                local duration=$((end_time - start_time))
                echo -e "${RED}✗ $suite_name failed (${duration}s)${NC}"
                echo "✗ $suite_name: FAILED (${duration}s)" >> "$MASTER_LOG"
                echo "\nTest failed after ${duration}s" >> "$output_file"
                
                if [[ "$required" == "true" ]]; then
                    echo -e "${RED}❌ Required test suite failed. Stopping.${NC}"
                    exit 1
                else
                    echo -e "${YELLOW}⚠️ Optional test suite failed. Continuing.${NC}"
                    return 1
                fi
            fi
        else
            # Run without output redirection
            if "$script_path"; then
                local end_time=$(date +%s)
                local duration=$((end_time - start_time))
                echo -e "${GREEN}✓ $suite_name completed successfully (${duration}s)${NC}"
                echo "✓ $suite_name: SUCCESS (${duration}s)" >> "$MASTER_LOG"
                return 0
            else
                local end_time=$(date +%s)
                local duration=$((end_time - start_time))
                echo -e "${RED}✗ $suite_name failed (${duration}s)${NC}"
                echo "✗ $suite_name: FAILED (${duration}s)" >> "$MASTER_LOG"
                
                if [[ "$required" == "true" ]]; then
                    echo -e "${RED}❌ Required test suite failed. Stopping.${NC}"
                    exit 1
                else
                    echo -e "${YELLOW}⚠️ Optional test suite failed. Continuing.${NC}"
                    return 1
                fi
            fi
        fi
    else
        echo -e "${YELLOW}⚠️ $suite_name script not found: $script_path${NC}"
        echo "⚠️ $suite_name: NOT FOUND" >> "$MASTER_LOG"
        return 1
    fi
}

# Function to check prerequisites
check_prerequisites() {
    echo -e "${BLUE}▶ Checking prerequisites...${NC}"
    
    # Check if compiler exists
    if [[ ! -f "build/enhanced_compiler" ]]; then
        echo -e "${YELLOW}⚠️ Compiler not built. Building now...${NC}"
        if make clean && make; then
            echo -e "${GREEN}✓ Compiler built successfully${NC}"
        else
            echo -e "${RED}❌ Failed to build compiler${NC}"
            exit 1
        fi
    else
        echo -e "${GREEN}✓ Compiler found${NC}"
    fi
    
    # Check directories
    mkdir -p test_reports/{unit,regression,integration,master}
    echo -e "${GREEN}✓ Test directories ready${NC}"
}

# Main test execution based on mode
case "$TEST_MODE" in
    "quick")
        echo -e "${BLUE}Running quick test suite...${NC}"
        check_prerequisites
        run_test_suite "Unit Tests" "scripts/test/unit_tests.sh" "true"
        ;;
        
    "unit")
        echo -e "${BLUE}Running unit tests only...${NC}"
        check_prerequisites
        run_test_suite "Unit Tests" "scripts/test/unit_tests.sh" "true" "test_reports/unit/unit_test_output.txt"
        ;;
        
    "regression")
        echo -e "${BLUE}Running regression tests only...${NC}"
        check_prerequisites
        run_test_suite "Regression Tests" "scripts/test/regression_tests.sh" "true" "test_reports/regression/regression_test_output.txt"
        ;;
        
    "integration")
        echo -e "${BLUE}Running integration tests only...${NC}"
        check_prerequisites
        run_test_suite "Integration Tests" "scripts/test/run_automated_tests.sh" "true" "test_reports/integration/integration_test_output.txt"
        ;;
        
    "performance")
        echo -e "${BLUE}Running performance tests only...${NC}"
        check_prerequisites
        run_test_suite "Performance Tests" "scripts/test/performance_tests.sh" "true" "test_reports/performance/performance_test_output.txt"
        ;;
        
    "ci")
        echo -e "${BLUE}Running CI pipeline...${NC}"
        run_test_suite "CI Pipeline" "scripts/test/ci_pipeline.sh" "true"
        ;;
        
    "full"|*)
        echo -e "${BLUE}Running comprehensive test suite...${NC}"
        
        # Check prerequisites first
        check_prerequisites
        
        # Run test suites in order
        SUITE_RESULTS=()
        
        # Core functionality tests
        if run_test_suite "Unit Tests" "scripts/test/unit_tests.sh" "false" "test_reports/unit/unit_test_output.txt"; then
            SUITE_RESULTS+=("Unit:PASS")
        else
            SUITE_RESULTS+=("Unit:FAIL")
        fi
        
        # Regression tests
        if run_test_suite "Regression Tests" "scripts/test/regression_tests.sh" "false" "test_reports/regression/regression_test_output.txt"; then
            SUITE_RESULTS+=("Regression:PASS")
        else
            SUITE_RESULTS+=("Regression:FAIL")
        fi
        
        # Integration tests
        if run_test_suite "Integration Tests" "scripts/test/run_automated_tests.sh" "false" "test_reports/integration/integration_test_output.txt"; then
            SUITE_RESULTS+=("Integration:PASS")
        else
            SUITE_RESULTS+=("Integration:FAIL")
        fi
        
        # Performance tests (if available)
        if run_test_suite "Performance Tests" "scripts/test/performance_tests.sh" "false" "test_reports/performance/performance_test_output.txt"; then
            SUITE_RESULTS+=("Performance:PASS")
        else
            SUITE_RESULTS+=("Performance:FAIL")
        fi
        ;;
esac

# Generate master report
echo -e "\n${PURPLE}╔════════════════════════════════════════╗${NC}"
echo -e "${PURPLE}║            Test Results Summary         ║${NC}"
echo -e "${PURPLE}╚════════════════════════════════════════╝${NC}"

# Count results if we have them
if [[ ${#SUITE_RESULTS[@]} -gt 0 ]]; then
    pass_count=0
    fail_count=0
    
    echo "" >> "$MASTER_LOG"
    echo "FINAL RESULTS:" >> "$MASTER_LOG"
    echo "=============" >> "$MASTER_LOG"
    
    for result in "${SUITE_RESULTS[@]}"; do
        suite=$(echo "$result" | cut -d: -f1)
        status=$(echo "$result" | cut -d: -f2)
        
        if [[ "$status" == "PASS" ]]; then
            echo -e "${GREEN}✓ $suite: PASSED${NC}"
            ((pass_count++))
        else
            echo -e "${RED}✗ $suite: FAILED${NC}"
            ((fail_count++))
        fi
        echo "$suite: $status" >> "$MASTER_LOG"
    done
    
    echo ""
    echo -e "Summary: ${GREEN}$pass_count passed${NC}, ${RED}$fail_count failed${NC}"
    
    # Calculate success rate
    total=$((pass_count + fail_count))
    if [[ $total -gt 0 ]]; then
        success_rate=$((pass_count * 100 / total))
        echo -e "Success Rate: ${success_rate}%"
    fi
    
    echo "" >> "$MASTER_LOG"
    echo "Summary: $pass_count passed, $fail_count failed" >> "$MASTER_LOG"
    echo "Success Rate: ${success_rate:-0}%" >> "$MASTER_LOG"
fi

# Generate HTML dashboard
cat > "test_reports/master/test_dashboard.html" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Design Project Test Dashboard</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            margin: 0; 
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container { 
            max-width: 1200px; 
            margin: 0 auto; 
            background: white; 
            padding: 20px; 
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .header { 
            text-align: center; 
            color: #2c3e50; 
            border-bottom: 3px solid #3498db;
            padding-bottom: 20px;
            margin-bottom: 30px;
        }
        .status-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }
        .status-card {
            padding: 20px;
            border-radius: 8px;
            text-align: center;
            color: white;
            font-weight: bold;
        }
        .pass { background-color: #27ae60; }
        .fail { background-color: #e74c3c; }
        .warn { background-color: #f39c12; }
        .info { background-color: #3498db; }
        .logs { 
            background-color: #ecf0f1; 
            padding: 15px; 
            border-radius: 5px; 
            font-family: monospace;
            white-space: pre-line;
            max-height: 400px;
            overflow-y: auto;
        }
        .timestamp { color: #7f8c8d; font-size: 0.9em; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🧪 Design Project Test Dashboard</h1>
            <p class="timestamp">Generated: $(date)</p>
            <p>Mode: <strong>$TEST_MODE</strong></p>
        </div>
        
        <div class="status-grid">
EOF

# Add status cards based on results
if [[ ${#SUITE_RESULTS[@]} -gt 0 ]]; then
    for result in "${SUITE_RESULTS[@]}"; do
        suite=$(echo "$result" | cut -d: -f1)
        status=$(echo "$result" | cut -d: -f2)
        
        if [[ "$status" == "PASS" ]]; then
            cat >> "test_reports/master/test_dashboard.html" << EOF
            <div class="status-card pass">
                <h3>✅ $suite Tests</h3>
                <p>Status: PASSED</p>
            </div>
EOF
        else
            cat >> "test_reports/master/test_dashboard.html" << EOF
            <div class="status-card fail">
                <h3>❌ $suite Tests</h3>
                <p>Status: FAILED</p>
            </div>
EOF
        fi
    done
else
    cat >> "test_reports/master/test_dashboard.html" << EOF
            <div class="status-card info">
                <h3>ℹ️ Test Mode: $TEST_MODE</h3>
                <p>Check individual test reports</p>
            </div>
EOF
fi

cat >> "test_reports/master/test_dashboard.html" << EOF
        </div>
        
        <h2>📋 Test Log</h2>
        <div class="logs">$(cat "$MASTER_LOG" | head -50)</div>
        
        <h2>📁 Available Reports</h2>
        <ul>
            <li><a href="../unit/unit_test_output.txt">Unit Test Output</a></li>
            <li><a href="../regression/regression_test_output.txt">Regression Test Output</a></li>
            <li><a href="../integration/integration_test_output.txt">Integration Test Output</a></li>
            <li><a href="../performance/performance_test_output.txt">Performance Test Output</a></li>
        </ul>
        
        <p class="timestamp">
            Report Location: test_reports/master/<br>
            For detailed logs, check individual test directories.
        </p>
    </div>
</body>
</html>
EOF

echo ""
echo -e "${BLUE}📊 Reports Generated:${NC}"
echo -e "  • Master Log: ${MASTER_LOG}"
echo -e "  • Dashboard: test_reports/master/test_dashboard.html"
echo -e "  • Individual reports in test_reports/ subdirectories"

# Final exit code
if [[ ${#SUITE_RESULTS[@]} -gt 0 ]]; then
    failed_suites=0
    for result in "${SUITE_RESULTS[@]}"; do
        if [[ "$result" == *":FAIL" ]]; then
            ((failed_suites++))
        fi
    done
    
    if [[ $failed_suites -eq 0 ]]; then
        echo -e "\n${GREEN}🎉 All test suites completed successfully!${NC}"
        exit 0
    else
        echo -e "\n${YELLOW}⚠️ $failed_suites test suite(s) had failures${NC}"
        exit 1
    fi
else
    echo -e "\n${GREEN}✓ Test execution completed${NC}"
    exit 0
fi