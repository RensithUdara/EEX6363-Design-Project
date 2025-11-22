# Makefile for Design Project - Complete Compiler Implementation
# =============================================================
# Updated for new directory structure with organized source files

# Compiler and tools
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -DDESIGN_PROJECT
LEX = flex

# Directories
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
TESTS_DIR = tests
SCRIPTS_DIR = scripts
DOCS_DIR = docs
OUTPUT_DIR = output

# Include paths
INCLUDES = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core -I$(INCLUDE_DIR)/lexical -I$(INCLUDE_DIR)/syntax -I$(INCLUDE_DIR)/semantic -I$(INCLUDE_DIR)/codegen -I$(INCLUDE_DIR)/utils

# Source files organized by component
CORE_SRCS = $(SRC_DIR)/core/enhanced_compiler.c $(SRC_DIR)/core/ast.c
LEXICAL_SRCS = $(SRC_DIR)/lexical/scanner.l
SEMANTIC_SRCS = $(SRC_DIR)/semantic/semantic.c $(SRC_DIR)/semantic/semantic_integration.c
CODEGEN_SRCS = $(SRC_DIR)/codegen/codegen.c $(SRC_DIR)/codegen/register_allocator.c
UTILS_SRCS = $(SRC_DIR)/utils/error_handler.c $(SRC_DIR)/utils/output_generator.c

# Generated files
LEX_OUTPUT = $(BUILD_DIR)/lex.yy.c

# Object files
CORE_OBJS = $(BUILD_DIR)/enhanced_compiler.o $(BUILD_DIR)/ast.o
LEXICAL_OBJS = $(BUILD_DIR)/lex.yy.o
SEMANTIC_OBJS = $(BUILD_DIR)/semantic.o $(BUILD_DIR)/semantic_integration.o
CODEGEN_OBJS = $(BUILD_DIR)/codegen.o $(BUILD_DIR)/register_allocator.o
UTILS_OBJS = $(BUILD_DIR)/error_handler.o $(BUILD_DIR)/output_generator.o

# All object files
ALL_OBJS = $(CORE_OBJS) $(LEXICAL_OBJS) $(SEMANTIC_OBJS) $(CODEGEN_OBJS) $(UTILS_OBJS)

# Executable
TARGET = $(BUILD_DIR)/enhanced_compiler

# Main targets
.PHONY: all clean test help structure info

all: $(TARGET)

# Build the main executable
$(TARGET): $(ALL_OBJS) | $(BUILD_DIR)
	@echo "Linking enhanced compiler..."
	$(CC) $(CFLAGS) -o $@ $(ALL_OBJS)
	@echo "✓ Build successful! Executable: $(TARGET)"

# Core components
$(BUILD_DIR)/enhanced_compiler.o: $(SRC_DIR)/core/enhanced_compiler.c | $(BUILD_DIR)
	@echo "Compiling main compiler..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/ast.o: $(SRC_DIR)/core/ast.c | $(BUILD_DIR)
	@echo "Compiling AST module..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Lexical analysis
$(BUILD_DIR)/lex.yy.o: $(LEX_OUTPUT) | $(BUILD_DIR)
	@echo "Compiling lexical analyzer..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LEX_OUTPUT): $(SRC_DIR)/lexical/scanner.l | $(BUILD_DIR)
	@echo "Generating lexical analyzer..."
	$(LEX) -o $@ $<

# Semantic analysis
$(BUILD_DIR)/semantic.o: $(SRC_DIR)/semantic/semantic.c | $(BUILD_DIR)
	@echo "Compiling semantic analyzer..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/semantic_integration.o: $(SRC_DIR)/semantic/semantic_integration.c | $(BUILD_DIR)
	@echo "Compiling semantic integration..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Code generation
$(BUILD_DIR)/codegen.o: $(SRC_DIR)/codegen/codegen.c | $(BUILD_DIR)
	@echo "Compiling code generator..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/register_allocator.o: $(SRC_DIR)/codegen/register_allocator.c | $(BUILD_DIR)
	@echo "Compiling register allocator..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Utilities
$(BUILD_DIR)/error_handler.o: $(SRC_DIR)/utils/error_handler.c | $(BUILD_DIR)
	@echo "Compiling error handler..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/output_generator.o: $(SRC_DIR)/utils/output_generator.c | $(BUILD_DIR)
	@echo "Compiling output generator..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete"

# Run tests using the organized test structure
test: $(TARGET)
	@echo "Running comprehensive test suite..."
	@cd $(SCRIPTS_DIR)/test && ./run_tests.sh

# Show project structure
structure:
	@echo "=== Project Structure ==="
	@echo "Source Code:"
	@find $(SRC_DIR) -type f -name "*.c" -o -name "*.l" -o -name "*.y" | sort
	@echo ""
	@echo "Headers:"
	@find $(INCLUDE_DIR) -type f -name "*.h" | sort
	@echo ""
	@echo "Tests:"
	@find $(TESTS_DIR) -type f | sort
	@echo ""
	@echo "Scripts:"
	@find $(SCRIPTS_DIR) -type f | sort

# Show build information
info:
	@echo "=== Build Information ==="
	@echo "Compiler: $(CC)"
	@echo "Flags: $(CFLAGS)"
	@echo "Target: $(TARGET)"
	@echo "Include paths: $(INCLUDES)"
	@echo ""
	@echo "=== Components ==="
	@echo "Core: $(CORE_SRCS)"
	@echo "Lexical: $(LEXICAL_SRCS)"
	@echo "Semantic: $(SEMANTIC_SRCS)"
	@echo "Codegen: $(CODEGEN_SRCS)"
	@echo "Utils: $(UTILS_SRCS)"

# Help target
help:
	@echo "=== Available Targets ==="
	@echo "all        - Build the complete compiler"
	@echo "clean      - Remove build artifacts"
	@echo "test       - Run comprehensive test suite"
	@echo "structure  - Show project directory structure"
	@echo "info       - Show build configuration"
	@echo "help       - Show this help message"
	@echo ""
	@echo "=== Directory Structure ==="
	@echo "src/       - Source code organized by component"
	@echo "include/   - Header files organized by component"
	@echo "build/     - Build artifacts and executables"
	@echo "tests/     - Test files and test outputs"
	@echo "scripts/   - Build and test scripts"
	@echo "docs/      - Documentation and requirements"
	@echo "output/    - Generated assembly and reports"

# Dependencies (auto-generated would be better, but this works for now)
$(BUILD_DIR)/enhanced_compiler.o: $(INCLUDE_DIR)/utils/output_generator.h $(INCLUDE_DIR)/codegen/codegen.h $(INCLUDE_DIR)/semantic/semantic_integration.h $(INCLUDE_DIR)/codegen/register_allocator.h $(INCLUDE_DIR)/core/ast.h $(INCLUDE_DIR)/utils/error_handler.h
$(BUILD_DIR)/ast.o: $(INCLUDE_DIR)/core/ast.h $(INCLUDE_DIR)/utils/error_handler.h
$(BUILD_DIR)/semantic.o: $(INCLUDE_DIR)/core/ast.h $(INCLUDE_DIR)/utils/error_handler.h
$(BUILD_DIR)/semantic_integration.o: $(INCLUDE_DIR)/semantic/semantic_integration.h
$(BUILD_DIR)/codegen.o: $(INCLUDE_DIR)/codegen/codegen.h $(INCLUDE_DIR)/codegen/register_allocator.h
$(BUILD_DIR)/register_allocator.o: $(INCLUDE_DIR)/codegen/register_allocator.h
$(BUILD_DIR)/error_handler.o: $(INCLUDE_DIR)/utils/error_handler.h $(INCLUDE_DIR)/core/ast.h
$(BUILD_DIR)/output_generator.o: $(INCLUDE_DIR)/utils/output_generator.h $(INCLUDE_DIR)/core/ast.h $(INCLUDE_DIR)/utils/error_handler.h $(INCLUDE_DIR)/codegen/codegen.h