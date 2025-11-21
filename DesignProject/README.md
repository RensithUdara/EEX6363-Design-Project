# Design Project - Complete Compiler Implementation

This directory contains the complete implementation of the compiler design project, including the code generation phase that finalizes the compiler built through TMA1, TMA2, and TMA3.

## Project Structure

### Core Components
- **Lexical Analyzer** (from TMA1): Tokenization and lexical analysis
- **Parser** (from TMA2): Syntax analysis with AST generation  
- **Semantic Analyzer** (from TMA3): Symbol table management and semantic validation
- **Code Generator** (NEW): Target code generation with register allocation

### Architecture
- **Target Architecture**: x86-64 Assembly
- **Register Allocation**: Register-based allocation with spilling
- **Memory Management**: Stack-based with activation records
- **Code Organization**: Three-address code intermediate representation

## Files

### Code Generation
- `codegen.h` - Code generation interface and data structures
- `codegen.c` - Code generation implementation
- `register_allocator.h` - Register allocation interface
- `register_allocator.c` - Register allocation implementation
- `intermediate.h` - Intermediate representation definitions
- `intermediate.c` - IR generation and optimization

### Testing and Integration
- `compiler.c` - Main compiler driver
- `test_suite.c` - Comprehensive test suite
- `Makefile` - Build configuration
- `README.md` - This file

### Documentation
- `DESIGN_PROJECT_DOCUMENTATION.md` - Complete design documentation
- `CODE_GENERATION_GUIDE.md` - Code generation implementation guide
- `TEST_RESULTS.md` - Test results and validation

## Building the Complete Compiler

```bash
make all          # Build complete compiler
make test         # Run test suite  
make clean        # Clean build files
```

## Usage

```bash
./compiler input.src -o output.asm    # Compile to assembly
./compiler input.src -r              # Show register allocation
./compiler input.src -i              # Show intermediate code
```