# Top-level Makefile for Design Project
# This makefile provides convenience targets to build the complete compiler
# from the parent directory without needing to cd into DesignProject/

# Default target
.PHONY: all clean test demo help install

# Forward all targets to the DesignProject subdirectory
all:
	@echo "========================================="
	@echo "Building Design Project Complete Compiler"
	@echo "========================================="
	@$(MAKE) -C DesignProject all

clean:
	@echo "Cleaning Design Project..."
	@$(MAKE) -C DesignProject clean

test:
	@echo "Running Design Project tests..."
	@$(MAKE) -C DesignProject test

demo:
	@echo "Running Design Project demonstration..."
	@$(MAKE) -C DesignProject demo

install:
	@echo "Installing Design Project compiler..."
	@$(MAKE) -C DesignProject install

help:
	@echo "Available targets:"
	@echo "  all     - Build the complete compiler"
	@echo "  clean   - Clean build files"
	@echo "  test    - Run test suite"
	@echo "  demo    - Run demonstration"
	@echo "  install - Install compiler system-wide"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "For more detailed options, run:"
	@echo "  make -C DesignProject help"

# Also forward any other targets
.DEFAULT:
	@$(MAKE) -C DesignProject $@