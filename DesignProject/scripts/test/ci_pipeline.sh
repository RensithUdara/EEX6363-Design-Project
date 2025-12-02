#!/bin/bash
# Continuous Integration Testing Script
# Builds and tests the entire project automatically

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$PROJECT_ROOT"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Design Project CI Pipeline${NC}"
echo -e "${BLUE}========================================${NC}"

# Step 1: Clean previous build
echo -e "${YELLOW}Step 1: Cleaning previous build...${NC}"
make clean || echo "Clean failed, continuing..."

# Step 2: Build project
echo -e "${YELLOW}Step 2: Building project...${NC}"
if make; then
    echo -e "${GREEN}✓ Build successful${NC}"
else
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi

# Step 3: Verify compiler exists
echo -e "${YELLOW}Step 3: Verifying compiler...${NC}"
if [[ -f "build/enhanced_compiler" ]]; then
    echo -e "${GREEN}✓ Compiler built successfully${NC}"
else
    echo -e "${RED}✗ Compiler not found${NC}"
    exit 1
fi

# Step 4: Run automated tests
echo -e "${YELLOW}Step 4: Running automated tests...${NC}"
if ./scripts/test/run_automated_tests.sh; then
    echo -e "${GREEN}✓ Tests passed${NC}"
    test_result="PASS"
else
    echo -e "${YELLOW}⚠ Some tests failed${NC}"
    test_result="PARTIAL"
fi

# Step 5: Generate CI report
echo -e "${YELLOW}Step 5: Generating CI report...${NC}"
mkdir -p ci_reports

cat > ci_reports/ci_summary.md << EOF
# Design Project CI Report

**Date:** $(date)
**Branch:** $(git branch --show-current 2>/dev/null || echo "unknown")
**Commit:** $(git rev-parse --short HEAD 2>/dev/null || echo "unknown")

## Build Status
- ✅ Clean: Success
- ✅ Compile: Success  
- ✅ Link: Success
- $([ "$test_result" = "PASS" ] && echo "✅" || echo "⚠️") Tests: $test_result

## Artifacts Generated
- \`build/enhanced_compiler\` - Main compiler executable
- \`test_reports/\` - Test result reports
- \`ci_reports/\` - CI pipeline reports

## Next Steps
$(if [ "$test_result" = "PASS" ]; then
    echo "- Ready for deployment/integration"
    echo "- All quality gates passed"
else
    echo "- Review failed tests in test_reports/"
    echo "- Fix failing test cases before merging"
fi)

EOF

echo -e "${GREEN}✓ CI report generated: ci_reports/ci_summary.md${NC}"

# Step 6: Archive artifacts
echo -e "${YELLOW}Step 6: Archiving artifacts...${NC}"
tar -czf "ci_reports/build_artifacts_$(date +%Y%m%d_%H%M%S).tar.gz" \
    build/ test_reports/ || echo "Archive creation failed"

echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}CI Pipeline Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Build: ${GREEN}SUCCESS${NC}"
echo -e "Tests: $([ "$test_result" = "PASS" ] && echo -e "${GREEN}$test_result${NC}" || echo -e "${YELLOW}$test_result${NC}")"
echo -e "Reports: Available in ci_reports/ and test_reports/"
echo -e "${BLUE}========================================${NC}"

if [ "$test_result" = "PASS" ]; then
    exit 0
else
    exit 1
fi