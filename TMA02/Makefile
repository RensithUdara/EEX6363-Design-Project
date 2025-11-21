# Makefile for TMA03 Semantic Analyzer
# Compiler and tools
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LEX = flex
YACC = bison
YFLAGS = -d -v

# Directories
SRC_DIR = .
BUILD_DIR = build

# Source files
LEX_FILE = scanner.l
YACC_FILE = parser.y
AST_SRC = ast.c
SEMANTIC_SRC = semantic.c

# Generated files
LEX_OUTPUT = lex.yy.c
YACC_OUTPUT = parser.tab.c
YACC_HEADER = parser.tab.h

# Object files
LEX_OBJ = $(BUILD_DIR)/lex.yy.o
YACC_OBJ = $(BUILD_DIR)/parser.tab.o
AST_OBJ = $(BUILD_DIR)/ast.o
SEMANTIC_OBJ = $(BUILD_DIR)/semantic.o

# Target executable
TARGET = $(BUILD_DIR)/semantic_analyzer

# Test files
VALID_TESTS = valid1.txt valid2.txt valid3.txt valid4.txt valid5.txt
ERROR_TESTS = error1.txt error2.txt error3.txt error4.txt error5.txt
ALL_TESTS = $(VALID_TESTS) $(ERROR_TESTS)

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate parser files
$(YACC_OUTPUT) $(YACC_HEADER): $(YACC_FILE) | $(BUILD_DIR)
	$(YACC) $(YFLAGS) -o $(YACC_OUTPUT) $(YACC_FILE)

# Generate lexer file
$(LEX_OUTPUT): $(LEX_FILE) $(YACC_HEADER) | $(BUILD_DIR)
	$(LEX) -o $(LEX_OUTPUT) $(LEX_FILE)

# Compile object files
$(YACC_OBJ): $(YACC_OUTPUT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(YACC_OUTPUT) -o $(YACC_OBJ)

$(LEX_OBJ): $(LEX_OUTPUT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(LEX_OUTPUT) -o $(LEX_OBJ)

$(AST_OBJ): $(AST_SRC) ast.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(AST_SRC) -o $(AST_OBJ)

$(SEMANTIC_OBJ): $(SEMANTIC_SRC) ast.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SEMANTIC_SRC) -o $(SEMANTIC_OBJ)

# Link executable
$(TARGET): $(YACC_OBJ) $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(YACC_OBJ) $(LEX_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) -lfl

# Test targets
test: $(TARGET) test-valid test-error

test-valid: $(TARGET)
	@echo "=== Testing Valid Programs ==="
	@for file in $(VALID_TESTS); do \
		if [ -f $$file ]; then \
			echo "Testing $$file..."; \
			$(TARGET) $$file; \
			echo ""; \
		fi; \
	done

test-error: $(TARGET)
	@echo "=== Testing Error Programs ==="
	@for file in $(ERROR_TESTS); do \
		if [ -f $$file ]; then \
			echo "Testing $$file..."; \
			$(TARGET) $$file; \
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

# Individual test
test-file: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make test-file FILE=<filename>"; \
	else \
		echo "Testing $(FILE)..."; \
		$(TARGET) $(FILE); \
	fi

# Clean targets
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(LEX_OUTPUT) $(YACC_OUTPUT) $(YACC_HEADER)
	rm -f parser.output
	rm -f *.derivation *.symboltable *.semanticerrors

clean-all: clean
	rm -f *.derivation *.symboltable *.semanticerrors

# Debug target with verbose output
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release target with optimization
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Install target (copies to system location)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/semantic_analyzer

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the semantic analyzer (default)"
	@echo "  test         - Run all tests"
	@echo "  test-valid   - Run tests on valid programs"
	@echo "  test-error   - Run tests on error programs"
	@echo "  test-file    - Test specific file (usage: make test-file FILE=filename)"
	@echo "  clean        - Remove build files"
	@echo "  clean-all    - Remove all generated files"
	@echo "  debug        - Build with debug information"
	@echo "  release      - Build optimized version"
	@echo "  install      - Install to system location"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all test test-valid test-error test-all test-file clean clean-all debug release install help

# Dependencies
$(LEX_OBJ): $(YACC_HEADER)
$(AST_OBJ): ast.h
$(SEMANTIC_OBJ): ast.h
$(YACC_OBJ): ast.h