# Makefile for TMA03:   Parser with Semantic Analyzer  
# ============================================================
# This Makefile builds a complete   parsing system with:
# - Hand-written Recursive Descent parser (  parsing method)
# - AST generation via recursive function calls
# - Two-phase semantic analysis (symbol tables + type checking) 
# - Comprehensive error reporting and recovery

# Compiler and tools
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -DTMA03_TOP_DOWN_PARSER
LEX = flex

# Directories
SRC_DIR = .
BUILD_DIR = build

# Source files
LEX_FILE = scanner.l
AST_SRC = ast.c
SEMANTIC_SRC = semantic.c

# Generated files
LEX_OUTPUT = lex.yy.c

# Object files
LEX_OBJ = $(BUILD_DIR)/lex.yy.o
AST_OBJ = $(BUILD_DIR)/ast.o
SEMANTIC_OBJ = $(BUILD_DIR)/semantic.o

# Target executable
TARGET = $(BUILD_DIR)/semantic_analyzer

# Test files
VALID_TESTS = valid1.txt valid2.txt valid3.txt valid4.txt valid5.txt
ERROR_TESTS = error1.txt error2.txt error3.txt error4.txt error5.txt
ALL_TESTS = $(VALID_TESTS) $(ERROR_TESTS)

# Additional test files
SEMANTIC_TESTS = test_error_semantic.txt

# Error handler source and object files
ERROR_HANDLER_SRC = error_handler.c
ERROR_HANDLER_OBJ = $(BUILD_DIR)/error_handler.o

# Comprehensive error test program
ERROR_TEST_TARGET = $(BUILD_DIR)/test_comprehensive_errors

# Banner target
banner:
	@echo "======================================================="
	@echo "     TMA03: Parser + Semantic Analyzer      "
	@echo "======================================================="
	@echo "Building Recursive Descent parser (hand-written)..."
	@echo "Features: AST generation + Two-phase semantic analysis"
	@echo "======================================================="

# Default target - builds TMA03 parser with semantic analyzer
all: banner $(TARGET)
	@echo ""
	@echo "✓ TMA03 Parser with Semantic Analyzer built successfully!"
	@echo "✓ Hand-written recursive descent parser ready for parsing"
	@echo ""
	@echo "Usage: $(TARGET) <input_file.txt>"
	@echo "Output: <input_file>.derivation, <input_file>.symboltable, <input_file>.semanticerrors"

# Create build directory
$(BUILD_DIR):
	@echo "Creating build directory..."
	mkdir -p $(BUILD_DIR)

# Generate lexer file 
$(LEX_OUTPUT): $(LEX_FILE) | $(BUILD_DIR)
	@echo "Generating lexical analyzer with Flex..."
	$(LEX) -o $(LEX_OUTPUT) $(LEX_FILE)
	@echo "✓ Lexer generated: $(LEX_OUTPUT)"

# Compile object files
$(LEX_OBJ): $(LEX_OUTPUT) | $(BUILD_DIR)
	@echo "Compiling lexical analyzer..."
	$(CC) $(CFLAGS) -c $(LEX_OUTPUT) -o $(LEX_OBJ)

$(AST_OBJ): $(AST_SRC) ast.h | $(BUILD_DIR)
	@echo "Compiling AST and symbol table management..."
	$(CC) $(CFLAGS) -c $(AST_SRC) -o $(AST_OBJ)

$(SEMANTIC_OBJ): $(SEMANTIC_SRC) ast.h error_handler.h | $(BUILD_DIR)
	@echo "Compiling   parser with semantic analyzer (main program)..."
	$(CC) $(CFLAGS) -c $(SEMANTIC_SRC) -o $(SEMANTIC_OBJ)

$(ERROR_HANDLER_OBJ): $(ERROR_HANDLER_SRC) error_handler.h ast.h | $(BUILD_DIR)
	@echo "Compiling comprehensive error handler..."
	$(CC) $(CFLAGS) -c $(ERROR_HANDLER_SRC) -o $(ERROR_HANDLER_OBJ)

# Link executable - TMA03   Parser with Semantic Analyzer
$(TARGET): $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(ERROR_HANDLER_OBJ) | $(BUILD_DIR)
	@echo "Linking TMA03   parser executable..."
	@# Try linking with -lfl first, fall back to -ll if not available (macOS compatibility)
	@$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(ERROR_HANDLER_OBJ) -lfl 2>/dev/null || \
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(ERROR_HANDLER_OBJ) -ll 2>/dev/null || \
	$(CC) $(CFLAGS) -o $(TARGET) $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(ERROR_HANDLER_OBJ)
	@echo "✓ TMA03   parser built successfully: $(TARGET)"

# Comprehensive error test program
$(ERROR_TEST_TARGET): test_comprehensive_errors.c $(AST_OBJ) $(ERROR_HANDLER_OBJ) | $(BUILD_DIR)
	@echo "Building comprehensive error test program..."
	$(CC) $(CFLAGS) -o $(ERROR_TEST_TARGET) test_comprehensive_errors.c $(AST_OBJ) $(ERROR_HANDLER_OBJ)
	@echo "✓ Comprehensive error test program built: $(ERROR_TEST_TARGET)"

# Test targets for TMA03 semantic analyzer
test: $(TARGET) test-banner test-valid test-error test-semantic
	@echo ""
	@echo "=== TMA03 TEST SUITE COMPLETE ==="
	@echo "All tests completed for   parser with semantic analyzer"

# Test comprehensive error handling system
test-errors: $(ERROR_TEST_TARGET)
	@echo ""
	@echo "=========================================="
	@echo "    COMPREHENSIVE ERROR SYSTEM TEST      "
	@echo "=========================================="
	@echo "Testing all error types: Lexical, Syntax, Semantic"
	@echo ""
	$(ERROR_TEST_TARGET)
	@echo ""
	@echo "✓ Comprehensive error system test completed!"
	@if [ -f comprehensive_error_report.txt ]; then \
		echo "✓ Detailed error report generated: comprehensive_error_report.txt"; \
		echo ""; \
		echo "=== ERROR REPORT PREVIEW ==="; \
		head -20 comprehensive_error_report.txt; \
		echo "... (see comprehensive_error_report.txt for full report)"; \
	fi

test-banner:
	@echo ""
	@echo "=========================================="
	@echo "    TMA03 COMPREHENSIVE TEST SUITE       "
	@echo "=========================================="
	@echo "Testing  parser with two-phase semantic analysis"
	@echo "Features tested: Parsing, AST, Symbol Tables, Type Checking"
	@echo "=========================================="

test-valid: $(TARGET)
	@echo ""
	@echo "=== Testing Valid Programs (Should Pass All Phases) ==="
	@for file in $(VALID_TESTS); do \
		if [ -f $$file ]; then \
			echo "----------------------------------------"; \
			echo "Testing $$file..."; \
			$(TARGET) $$file; \
			echo "✓ Test completed for $$file"; \
			echo ""; \
		fi; \
	done

test-error: $(TARGET)
	@echo ""
	@echo "=== Testing Error Programs (Should Detect Semantic Errors) ==="
	@for file in $(ERROR_TESTS); do \
		if [ -f $$file ]; then \
			echo "----------------------------------------"; \
			echo "Testing $$file..."; \
			$(TARGET) $$file; \
			echo "✓ Error detection test completed for $$file"; \
			echo ""; \
		fi; \
	done

test-semantic: $(TARGET)
	@echo ""
	@echo "=== Testing Additional Semantic Cases ==="
	@for file in $(SEMANTIC_TESTS); do \
		if [ -f $$file ]; then \
			echo "----------------------------------------"; \
			echo "Testing semantic cases in $$file..."; \
			$(TARGET) $$file; \
			echo "✓ Semantic test completed for $$file"; \
			echo ""; \
		fi; \
	done

test-all: $(TARGET)
	@echo "=== Testing All Programs ==="
	@for file in $(ALL_TESTS); do \
		if [ -f $$file ]; then \
			echo "Testing $$file..."; \
			$(TARGET) $$file; \
			echo ""; \
		fi; \
	done

# Individual test with detailed output
test-file: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make test-file FILE=<filename>"; \
		echo "Example: make test-file FILE=valid1.txt"; \
	else \
		echo "=========================================="; \
		echo "TMA03 Individual File Test: $(FILE)"; \
		echo "=========================================="; \
		echo "Running   parser with semantic analysis..."; \
		$(TARGET) $(FILE); \
		echo ""; \
		echo "Generated files:"; \
		@ls -la $(FILE).* 2>/dev/null || echo "No output files generated (compilation failed)"; \
	fi

# Demo target - runs a comprehensive demonstration
demo: $(TARGET)
	@echo "============================================="
	@echo "        TMA03 DEMONSTRATION                  "
	@echo "============================================="
	@echo "Demonstrating   parsing with semantic analysis..."
	@echo ""
	@if [ -f valid1.txt ]; then \
		echo "Running demo on valid1.txt..."; \
		$(TARGET) valid1.txt; \
		echo ""; \
		echo "Check generated files: valid1.txt.derivation, valid1.txt.symboltable, valid1.txt.semanticerrors"; \
	else \
		echo "Demo file valid1.txt not found. Please ensure test files are present."; \
	fi

# Clean targets
clean:
	@echo "Cleaning TMA03 build files..."
	rm -rf $(BUILD_DIR)
	rm -f $(LEX_OUTPUT) $(YACC_OUTPUT) $(YACC_HEADER)
	rm -f parser.output parser.xml
	rm -f comprehensive_error_report.txt
	@echo "✓ Build files cleaned"

clean-all: clean
	@echo "Cleaning all generated files (including test outputs)..."
	rm -f *.derivation *.symboltable *.semanticerrors
	@echo "✓ All generated files cleaned"

# Clean only test output files
clean-tests:
	@echo "Cleaning test output files..."
	rm -f *.derivation *.symboltable *.semanticerrors
	@echo "✓ Test output files cleaned"

# Debug target with verbose output
debug: CFLAGS += -DDEBUG -O0 -DVERBOSE_PARSING
debug: clean banner $(TARGET)
	@echo "✓ Debug version built with verbose parsing output"

# Release target with optimization  
release: CFLAGS += -O2 -DNDEBUG
release: clean banner $(TARGET)
	@echo "✓ Optimized release version built"

# Development target with additional warnings
dev: CFLAGS += -DDEBUG -O0 -Wconversion -Wunreachable-code -Wshadow
dev: clean banner $(TARGET)
	@echo "✓ Development version built with extra warnings"

# Install target (copies to system location)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/semantic_analyzer

# Help target
help:
	@echo "=========================================="
	@echo "  TMA03   Parser Build System    "
	@echo "=========================================="
	@echo ""
	@echo "BUILD TARGETS:"
	@echo "  all          - Build TMA03 semantic analyzer (default)"
	@echo "  debug        - Build with debug information and verbose output"
	@echo "  release      - Build optimized production version"
	@echo "  dev          - Build development version with extra warnings"
	@echo ""
	@echo "TEST TARGETS:"
	@echo "  test         - Run complete test suite (valid + error + semantic)"
	@echo "  test-valid   - Run tests on valid programs (should pass)"
	@echo "  test-error   - Run tests on error programs (should detect errors)"
	@echo "  test-semantic - Run additional semantic test cases"
	@echo "  test-errors  - Test comprehensive error handling system"
	@echo "  test-file    - Test specific file (usage: make test-file FILE=filename)"
	@echo "  demo         - Run demonstration with sample program"
	@echo ""
	@echo "CLEAN TARGETS:"
	@echo "  clean        - Remove build files only"
	@echo "  clean-all    - Remove build files and test outputs"
	@echo "  clean-tests  - Remove test output files only"
	@echo ""
	@echo "OTHER TARGETS:"
	@echo "  install      - Install to system location (/usr/local/bin)"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "EXAMPLE USAGE:"
	@echo "  make all && make test"
	@echo "  make test-file FILE=myprogram.txt"
	@echo "  make debug && ./build/semantic_analyzer program.txt"

# Status target - shows build and test file status
status:
	@echo "=========================================="
	@echo "        TMA03 PROJECT STATUS             "
	@echo "=========================================="
	@echo ""
	@echo "BUILD STATUS:"
	@if [ -f $(TARGET) ]; then \
		echo "✓ Executable: $(TARGET) (built)"; \
		ls -la $(TARGET); \
	else \
		echo "✗ Executable: $(TARGET) (not built)"; \
	fi
	@echo ""
	@echo "SOURCE FILES:"
	@ls -la *.c *.h *.l *.y 2>/dev/null || echo "Source files not found"
	@echo ""
	@echo "TEST FILES:"
	@echo "Valid tests: $(VALID_TESTS)"
	@echo "Error tests: $(ERROR_TESTS)"
	@echo "Semantic tests: $(SEMANTIC_TESTS)"
	@echo ""
	@echo "Generated parser files:"
	@ls -la parser.tab.* lex.yy.* parser.output 2>/dev/null || echo "No generated parser files"

# Phony targets
.PHONY: all test test-valid test-error test-all test-semantic test-errors test-file demo clean clean-all clean-tests debug release dev install help status banner test-banner

# Dependencies
$(AST_OBJ): ast.h
$(SEMANTIC_OBJ): ast.h
$(ERROR_HANDLER_OBJ): error_handler.h ast.h