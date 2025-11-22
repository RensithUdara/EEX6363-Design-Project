#!/bin/bash
# Project Structure Verification Script
# Verifies that all files are properly organized and includes are correct

echo "========================================"
echo "Design Project Structure Verification"
echo "========================================"

# Change to project root
cd "$(dirname "$0")"

# Check if key directories exist
echo "Checking directory structure..."
directories=(
    "src/core"
    "src/lexical" 
    "src/syntax"
    "src/semantic"
    "src/codegen"
    "src/utils"
    "include/core"
    "include/lexical"
    "include/syntax" 
    "include/semantic"
    "include/codegen"
    "include/utils"
    "tests/unit"
    "tests/integration"
    "tests/examples"
    "tests/outputs"
    "scripts/build"
    "scripts/test"
    "docs/design"
    "docs/testing"
    "build"
    "output/assembly"
    "output/reports"
)

missing_dirs=0
for dir in "${directories[@]}"; do
    if [ -d "$dir" ]; then
        echo "✓ $dir"
    else
        echo "✗ Missing: $dir"
        ((missing_dirs++))
    fi
done

echo ""
echo "Checking key source files..."
source_files=(
    "src/core/enhanced_compiler.c"
    "src/core/ast.c"
    "src/lexical/scanner.l"
    "src/semantic/semantic.c"
    "src/semantic/semantic_integration.c"
    "src/codegen/codegen.c"
    "src/codegen/register_allocator.c"
    "src/utils/error_handler.c"
    "src/utils/output_generator.c"
)

missing_sources=0
for file in "${source_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file"
    else
        echo "✗ Missing: $file"
        ((missing_sources++))
    fi
done

echo ""
echo "Checking key header files..."
header_files=(
    "include/core/ast.h"
    "include/semantic/semantic_integration.h"
    "include/codegen/codegen.h"
    "include/codegen/register_allocator.h"
    "include/utils/error_handler.h"
    "include/utils/output_generator.h"
)

missing_headers=0
for file in "${header_files[@]}"; do
    if [ -f "$file" ]; then
        echo "✓ $file"
    else
        echo "✗ Missing: $file"
        ((missing_headers++))
    fi
done

echo ""
echo "Checking build configuration..."
if [ -f "Makefile" ]; then
    echo "✓ Makefile present"
    # Check if Makefile has proper paths
    if grep -q "SRC_DIR = src" Makefile && grep -q "INCLUDE_DIR = include" Makefile; then
        echo "✓ Makefile configured for new structure"
    else
        echo "✗ Makefile may need updates for new structure"
    fi
else
    echo "✗ Missing: Makefile"
fi

echo ""
echo "Checking scripts..."
scripts=(
    "scripts/build/build.bat"
    "scripts/build/comprehensive_build.sh"
    "scripts/test/run_tests.sh"
    "scripts/test/test_compiler.bat"
)

missing_scripts=0
for script in "${scripts[@]}"; do
    if [ -f "$script" ]; then
        echo "✓ $script"
    else
        echo "✗ Missing: $script"
        ((missing_scripts++))
    fi
done

echo ""
echo "========================================"
echo "Verification Summary"
echo "========================================"

total_issues=$((missing_dirs + missing_sources + missing_headers + missing_scripts))

if [ $total_issues -eq 0 ]; then
    echo "✓ Project structure verification PASSED"
    echo "✓ All directories, files, and configurations are properly organized"
    echo ""
    echo "Next steps:"
    echo "1. Test build: make clean && make all"
    echo "2. Run tests: make test"
    echo "3. Check structure: make structure"
else
    echo "✗ Project structure verification FAILED"
    echo "✗ Found $total_issues issues:"
    echo "  - Missing directories: $missing_dirs"
    echo "  - Missing source files: $missing_sources" 
    echo "  - Missing header files: $missing_headers"
    echo "  - Missing scripts: $missing_scripts"
    echo ""
    echo "Please resolve these issues before building."
fi

echo ""
echo "For detailed structure information, run: make structure"
echo "For build help, run: make help"