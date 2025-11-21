# Makefile for Design Project - Complete Compiler Implementation
# =============================================================
# This Makefile builds the complete compiler including all phases:
# - Lexical Analysis (from TMA1)
# - Syntax Analysis (from TMA2)  
# - Semantic Analysis (from TMA3)
# - Code Generation (NEW - Design Project)

# Compiler and tools
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT
LEX = flex

# Directories
SRC_DIR = .
BUILD_DIR = build
TMA1_DIR = ../TMA01/question1b
TMA2_DIR = ../TMA02
TMA3_DIR = ../TMA03

# Include paths
INCLUDES = -I$(TMA3_DIR) -I$(TMA2_DIR) -I$(TMA1_DIR)

# Source files
COMPILER_SRC = compiler.c
CODEGEN_SRC = codegen.c
REGISTER_SRC = register_allocator.c

# TMA source files (from previous TMAs)
TMA3_SCANNER = $(TMA3_DIR)/scanner.l
TMA3_AST = $(TMA3_DIR)/ast.c
TMA3_SEMANTIC = $(TMA3_DIR)/semantic.c
TMA3_ERROR = $(TMA3_DIR)/error_handler.c

# Generated files
LEX_OUTPUT = $(BUILD_DIR)/lex.yy.c

# Object files
COMPILER_OBJ = $(BUILD_DIR)/compiler.o
CODEGEN_OBJ = $(BUILD_DIR)/codegen.o
REGISTER_OBJ = $(BUILD_DIR)/register_allocator.o
LEX_OBJ = $(BUILD_DIR)/lex.yy.o
AST_OBJ = $(BUILD_DIR)/ast.o
SEMANTIC_OBJ = $(BUILD_DIR)/semantic.o
ERROR_OBJ = $(BUILD_DIR)/error_handler.o

# All object files
SEMANTIC_INT_OBJ = $(BUILD_DIR)/semantic_integration.o
ALL_OBJS = $(COMPILER_OBJ) $(CODEGEN_OBJ) $(REGISTER_OBJ) $(LEX_OBJ) $(AST_OBJ) $(ERROR_OBJ) $(SEMANTIC_INT_OBJ)

# Target executable
TARGET = $(BUILD_DIR)/compiler

# Test files
TEST_DIR = tests
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.src)
TEST_ASSEMBLIES = $(TEST_SOURCES:.src=.asm)
TEST_LOGS = $(TEST_SOURCES:.src=.log)

# Default target
all: banner $(TARGET)

# Banner
banner:
	@echo "========================================"
	@echo "Design Project - Complete Compiler Build"
	@echo "========================================"
	@echo "Building complete compiler with:"
	@echo "  - Lexical Analysis (TMA1)"
	@echo "  - Syntax Analysis (TMA2)"  
	@echo "  - Semantic Analysis (TMA3)"
	@echo "  - Code Generation (NEW)"
	@echo "========================================"
	@echo ""

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Generate lexical analyzer
$(LEX_OUTPUT): $(TMA3_SCANNER) | $(BUILD_DIR)
	@echo "Generating lexical analyzer..."
	$(LEX) -o $(LEX_OUTPUT) $(TMA3_SCANNER)

# Compile lexical analyzer
$(LEX_OBJ): $(LEX_OUTPUT)
	@echo "Compiling lexical analyzer..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(LEX_OUTPUT) -o $(LEX_OBJ)

# Compile AST module
$(AST_OBJ): $(TMA3_AST) | $(BUILD_DIR)
	@echo "Compiling AST module..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(TMA3_AST) -o $(AST_OBJ)

# Compile semantic analyzer
$(SEMANTIC_OBJ): $(TMA3_SEMANTIC) | $(BUILD_DIR)
	@echo "Compiling semantic analyzer..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(TMA3_SEMANTIC) -o $(SEMANTIC_OBJ)

# Compile error handler
$(ERROR_OBJ): $(TMA3_ERROR) | $(BUILD_DIR)
	@echo "Compiling error handler..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(TMA3_ERROR) -o $(ERROR_OBJ)

# Compile semantic integration
$(SEMANTIC_INT_OBJ): semantic_integration.c semantic_integration.h | $(BUILD_DIR)
	@echo "Compiling TMA3 semantic integration..."
	$(CC) $(CFLAGS) $(INCLUDES) -c semantic_integration.c -o $(SEMANTIC_INT_OBJ)

# Compile code generator
$(CODEGEN_OBJ): $(CODEGEN_SRC) codegen.h | $(BUILD_DIR)
	@echo "Compiling code generator..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(CODEGEN_SRC) -o $(CODEGEN_OBJ)

# Compile register allocator
$(REGISTER_OBJ): $(REGISTER_SRC) register_allocator.h | $(BUILD_DIR)
	@echo "Compiling register allocator..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(REGISTER_SRC) -o $(REGISTER_OBJ)

# Compile main compiler
$(COMPILER_OBJ): $(COMPILER_SRC) codegen.h register_allocator.h | $(BUILD_DIR)
	@echo "Compiling main compiler..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $(COMPILER_SRC) -o $(COMPILER_OBJ)

# Link final executable
$(TARGET): $(ALL_OBJS)
	@echo "Linking complete compiler..."
	$(CC) $(ALL_OBJS) -o $(TARGET) -lm
	@echo ""
	@echo "Build completed successfully!"
	@echo "Executable: $(TARGET)"
	@echo ""

# Create test directory and files
setup-tests: | $(BUILD_DIR)
	@mkdir -p $(TEST_DIR)
	@echo "Creating test program files..."
	@echo "// Test 1: Simple arithmetic" > $(TEST_DIR)/test1.src
	@echo "function main() -> integer {" >> $(TEST_DIR)/test1.src
	@echo "    local x: integer;" >> $(TEST_DIR)/test1.src
	@echo "    x = 5 + 3 * 2;" >> $(TEST_DIR)/test1.src
	@echo "    return x;" >> $(TEST_DIR)/test1.src
	@echo "}" >> $(TEST_DIR)/test1.src
	@echo ""
	@echo "// Test 2: Control flow" > $(TEST_DIR)/test2.src
	@echo "function factorial(n: integer) -> integer {" >> $(TEST_DIR)/test2.src
	@echo "    if (n <= 1) then {" >> $(TEST_DIR)/test2.src
	@echo "        return 1;" >> $(TEST_DIR)/test2.src
	@echo "    } else {" >> $(TEST_DIR)/test2.src
	@echo "        return n * factorial(n - 1);" >> $(TEST_DIR)/test2.src
	@echo "    }" >> $(TEST_DIR)/test2.src
	@echo "}" >> $(TEST_DIR)/test2.src
	@echo ""
	@echo "// Test 3: Class and inheritance" > $(TEST_DIR)/test3.src
	@echo "class Animal {" >> $(TEST_DIR)/test3.src
	@echo "    public name: integer;" >> $(TEST_DIR)/test3.src
	@echo "    public func speak() -> void;" >> $(TEST_DIR)/test3.src
	@echo "};" >> $(TEST_DIR)/test3.src
	@echo ""
	@echo "Test files created in $(TEST_DIR)/"

# Run tests
test: $(TARGET) setup-tests
	@echo "========================================"
	@echo "Running Compiler Tests"
	@echo "========================================"
	@for test in $(TEST_DIR)/*.src; do \
		echo "Testing: $$test"; \
		$(TARGET) -v "$$test" -o "$${test%.src}.asm" > "$${test%.src}.log" 2>&1; \
		if [ $$? -eq 0 ]; then \
			echo "  ✓ PASSED"; \
		else \
			echo "  ✗ FAILED (see $${test%.src}.log)"; \
		fi; \
	done
	@echo "========================================"
	@echo "Test Results Summary:"
	@echo "  Passed: $$(grep -l '✓ PASSED' $(TEST_DIR)/*.log 2>/dev/null | wc -l)"
	@echo "  Failed: $$(grep -l '✗ FAILED' $(TEST_DIR)/*.log 2>/dev/null | wc -l)"
	@echo "========================================"

# Demonstration targets
demo: $(TARGET) setup-tests
	@echo "========================================"
	@echo "Design Project Compiler Demonstration"
	@echo "========================================"
	@echo ""
	@echo "1. Compiling with all phases (default):"
	$(TARGET) -v $(TEST_DIR)/test1.src -o $(BUILD_DIR)/demo1.asm
	@echo ""
	@echo "2. Showing AST and symbol tables:"
	$(TARGET) -v -a -s $(TEST_DIR)/test2.src -o $(BUILD_DIR)/demo2.asm
	@echo ""
	@echo "3. Showing register allocation and memory layout:"
	$(TARGET) -v -r -m $(TEST_DIR)/test1.src -o $(BUILD_DIR)/demo3.asm
	@echo ""
	@echo "4. Running individual phases:"
	@echo "   Lexical + Syntax only (phases 1+2=3):"
	$(TARGET) -p 3 $(TEST_DIR)/test1.src
	@echo ""
	@echo "   Up to semantic analysis (phases 1+2+4=7):"
	$(TARGET) -p 7 -s $(TEST_DIR)/test2.src
	@echo ""
	@echo "========================================"
	@echo "Generated assembly files:"
	@ls -la $(BUILD_DIR)/*.asm 2>/dev/null || echo "  No assembly files generated"
	@echo "========================================"

# Performance test
perf-test: $(TARGET) setup-tests
	@echo "Running performance tests..."
	@time $(TARGET) -v $(TEST_DIR)/test1.src -o $(BUILD_DIR)/perf1.asm
	@time $(TARGET) -v $(TEST_DIR)/test2.src -o $(BUILD_DIR)/perf2.asm
	@time $(TARGET) -v $(TEST_DIR)/test3.src -o $(BUILD_DIR)/perf3.asm

# Show register allocation strategies
show-strategies: $(TARGET) setup-tests
	@echo "========================================"
	@echo "Register Allocation Strategy Comparison"
	@echo "========================================"
	@echo ""
	@echo "1. Naive allocation:"
	$(TARGET) -v -r -R naive $(TEST_DIR)/test1.src -o $(BUILD_DIR)/naive.asm
	@echo ""
	@echo "2. LRU allocation:"
	$(TARGET) -v -r -R lru $(TEST_DIR)/test1.src -o $(BUILD_DIR)/lru.asm
	@echo ""
	@echo "3. Optimal allocation:"
	$(TARGET) -v -r -R optimal $(TEST_DIR)/test1.src -o $(BUILD_DIR)/optimal.asm
	@echo ""

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TEST_DIR)
	@echo "Clean completed."

# Clean and rebuild
rebuild: clean all

# Install (copy to system location)
install: $(TARGET)
	@echo "Installing compiler..."
	@mkdir -p /usr/local/bin
	@cp $(TARGET) /usr/local/bin/design-compiler
	@chmod +x /usr/local/bin/design-compiler
	@echo "Compiler installed as 'design-compiler'"

# Uninstall
uninstall:
	@echo "Uninstalling compiler..."
	@rm -f /usr/local/bin/design-compiler
	@echo "Compiler uninstalled."

# Help target
help:
	@echo "Design Project Compiler - Available Targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  all           - Build complete compiler (default)"
	@echo "  rebuild       - Clean and rebuild"
	@echo "  clean         - Remove build files"
	@echo ""
	@echo "Test targets:"
	@echo "  setup-tests   - Create test source files"
	@echo "  test          - Run comprehensive test suite"
	@echo "  demo          - Run demonstration of all features"
	@echo "  perf-test     - Run performance tests"
	@echo "  show-strategies - Compare register allocation strategies"
	@echo ""
	@echo "Install targets:"
	@echo "  install       - Install compiler system-wide"
	@echo "  uninstall     - Remove system installation"
	@echo ""
	@echo "Usage examples:"
	@echo "  make all                    # Build compiler"
	@echo "  make test                   # Run all tests"
	@echo "  make demo                   # Show compiler features"
	@echo "  ./build/compiler input.src  # Compile a program"

# Documentation generation
docs: 
	@echo "Generating documentation..."
	@mkdir -p docs
	@echo "# Design Project Documentation" > docs/README.md
	@echo "" >> docs/README.md
	@echo "## Architecture Overview" >> docs/README.md
	@echo "This compiler implements a complete four-phase compilation process:" >> docs/README.md
	@echo "" >> docs/README.md
	@echo "1. **Lexical Analysis**: Tokenization using Flex" >> docs/README.md
	@echo "2. **Syntax Analysis**: Recursive descent parsing with AST generation" >> docs/README.md
	@echo "3. **Semantic Analysis**: Two-phase semantic checking with symbol tables" >> docs/README.md
	@echo "4. **Code Generation**: x86-64 assembly generation with register allocation" >> docs/README.md
	@echo "" >> docs/README.md
	@echo "Generated documentation in docs/"

# Display project statistics
stats:
	@echo "========================================"
	@echo "Design Project Statistics"
	@echo "========================================"
	@echo "Source files:"
	@find . -name "*.c" -o -name "*.h" -o -name "*.l" -o -name "*.y" | wc -l
	@echo "Lines of code:"
	@find . -name "*.c" -o -name "*.h" | xargs wc -l | tail -1
	@echo "Test files:"
	@find $(TEST_DIR) -name "*.src" 2>/dev/null | wc -l || echo "0"
	@echo "Build artifacts:"
	@find $(BUILD_DIR) -type f 2>/dev/null | wc -l || echo "0"
	@echo "========================================"

# Show build configuration
config:
	@echo "========================================"
	@echo "Build Configuration"
	@echo "========================================"
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Lexer: $(LEX)"
	@echo "Include paths: $(INCLUDES)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Target: $(TARGET)"
	@echo "========================================"

# Declare phony targets
.PHONY: all banner clean rebuild test demo perf-test show-strategies help docs stats config setup-tests install uninstall

# Default target when no target is specified
.DEFAULT_GOAL := all