#!/bin/bash
# Comprehensive test script for TMA03 Semantic Analyzer

echo "==================================="
echo "TMA03 Semantic Analyzer Test Suite"
echo "==================================="
echo

# Check if build directory exists, if not create it
if [ ! -d "build" ]; then
    mkdir -p build
fi

# Function to run semantic analyzer and show results
run_test() {
    local test_file="$1"
    local description="$2"
    
    echo "----------------------------------------"
    echo "Testing: $description"
    echo "File: $test_file"
    echo "----------------------------------------"
    
    if [ ! -f "$test_file" ]; then
        echo "❌ Test file not found: $test_file"
        return 1
    fi
    
    echo "Running semantic analyzer..."
    ./build/semantic_analyzer "$test_file"
    
    local exit_code=$?
    echo
    echo "Exit code: $exit_code"
    
    # Show generated files
    echo "Generated files:"
    if [ -f "${test_file}.derivation" ]; then
        echo "  ✓ ${test_file}.derivation"
        echo "    Derivation (first 10 lines):"
        head -n 10 "${test_file}.derivation" | sed 's/^/      /'
    fi
    
    if [ -f "${test_file}.symboltable" ]; then
        echo "  ✓ ${test_file}.symboltable" 
        echo "    Symbol table:"
        head -n 20 "${test_file}.symboltable" | sed 's/^/      /'
    fi
    
    if [ -f "${test_file}.semanticerrors" ]; then
        echo "  ✓ ${test_file}.semanticerrors"
        echo "    Semantic errors:"
        cat "${test_file}.semanticerrors" | sed 's/^/      /'
    fi
    
    echo
    return $exit_code
}

# Build the project first
echo "Building semantic analyzer..."
echo "==============================="

# Try different build methods
if command -v make >/dev/null 2>&1; then
    echo "Using make..."
    make clean
    make all
    BUILD_STATUS=$?
elif command -v gcc >/dev/null 2>&1; then
    echo "Using manual compilation..."
    
    # Generate parser and lexer files
    if command -v bison >/dev/null 2>&1; then
        echo "Generating parser..."
        bison -d -v -o parser.tab.c parser.y
    elif command -v yacc >/dev/null 2>&1; then
        echo "Generating parser with yacc..."
        yacc -d -o parser.tab.c parser.y
    else
        echo "❌ No parser generator found (bison/yacc)"
        exit 1
    fi
    
    if command -v flex >/dev/null 2>&1; then
        echo "Generating lexer..."
        flex -o lex.yy.c scanner.l
    elif command -v lex >/dev/null 2>&1; then
        echo "Generating lexer with lex..."
        lex scanner.l
        mv lex.yy.c lex.yy.c 2>/dev/null || true
    else
        echo "❌ No lexer generator found (flex/lex)"
        exit 1
    fi
    
    # Compile
    echo "Compiling..."
    gcc -Wall -Wextra -g -std=c99 -c parser.tab.c -o build/parser.tab.o
    gcc -Wall -Wextra -g -std=c99 -c lex.yy.c -o build/lex.yy.o  
    gcc -Wall -Wextra -g -std=c99 -c ast.c -o build/ast.o
    gcc -Wall -Wextra -g -std=c99 -c semantic.c -o build/semantic.o
    
    # Link
    echo "Linking..."
    gcc -Wall -Wextra -g -o build/semantic_analyzer build/parser.tab.o build/lex.yy.o build/ast.o build/semantic.o -lfl 2>/dev/null || \
    gcc -Wall -Wextra -g -o build/semantic_analyzer build/parser.tab.o build/lex.yy.o build/ast.o build/semantic.o
    
    BUILD_STATUS=$?
else
    echo "❌ No compiler found"
    exit 1
fi

if [ $BUILD_STATUS -ne 0 ]; then
    echo "❌ Build failed!"
    exit 1
fi

echo "✅ Build successful!"
echo

# Check if executable exists
if [ ! -f "build/semantic_analyzer" ] && [ ! -f "build/semantic_analyzer.exe" ]; then
    echo "❌ Executable not found after build"
    exit 1
fi

# Set executable name
EXECUTABLE="build/semantic_analyzer"
if [ -f "build/semantic_analyzer.exe" ]; then
    EXECUTABLE="build/semantic_analyzer.exe"
fi

echo "Using executable: $EXECUTABLE"
echo

# Run tests
echo "Running Test Suite"
echo "=================="
echo

# Test valid programs
if [ -f "test_valid_semantic.txt" ]; then
    run_test "test_valid_semantic.txt" "Valid Program with Classes and Inheritance"
fi

if [ -f "valid1.txt" ]; then
    run_test "valid1.txt" "Valid Program 1 (from TMA02)"
fi

if [ -f "valid2.txt" ]; then
    run_test "valid2.txt" "Valid Program 2 (from TMA02)" 
fi

# Test error programs
if [ -f "test_error_semantic.txt" ]; then
    run_test "test_error_semantic.txt" "Program with Semantic Errors"
fi

if [ -f "error1.txt" ]; then
    run_test "error1.txt" "Error Program 1 (from TMA02)"
fi

if [ -f "error2.txt" ]; then
    run_test "error2.txt" "Error Program 2 (from TMA02)"
fi

# Test simple programs to verify basic functionality
echo "Creating additional test programs..."

# Create a simple valid test
cat > simple_valid.txt << 'EOF'
func main() => integer {
    local x : integer;
    local y : float;
    
    x := 10;
    y := 3.14;
    
    if (x > 5) then {
        write(y);
    };
    
    return(x);
}
EOF

# Create a simple error test
cat > simple_error.txt << 'EOF'
func test() => integer {
    local x : integer;
    local y : float;
    
    x := 3.14;  // Type error
    undefinedVar := 5;  // Undefined variable
    return(y);  // Type mismatch
}
EOF

run_test "simple_valid.txt" "Simple Valid Program"
run_test "simple_error.txt" "Simple Error Program"

# Summary
echo "========================================="
echo "Test Suite Complete!"
echo "========================================="
echo
echo "Files generated during testing:"
ls -la *.derivation *.symboltable *.semanticerrors 2>/dev/null || echo "No output files found"

echo
echo "To run individual tests:"
echo "  $EXECUTABLE program.txt"
echo
echo "Output files:"
echo "  program.txt.derivation    - Parse derivation"
echo "  program.txt.symboltable   - Symbol table dump"  
echo "  program.txt.semanticerrors - Semantic error report"
echo