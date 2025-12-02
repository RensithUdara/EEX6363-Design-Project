# Design Project - Complete Compiler Implementation

## Project Structure

This project has been reorganized into a comprehensive, well-structured directory layout for better maintainability and development workflow.

### Directory Structure

```
DesignProject/
├── src/                          # Source code organized by component
│   ├── core/                     # Core compiler components
│   │   ├── compiler.c   # Main compiler implementation
│   │   └── ast.c                 # Abstract Syntax Tree implementation
│   ├── lexical/                  # Lexical analysis components
│   │   └── scanner.l             # Flex lexical analyzer specification
│   ├── syntax/                   # Syntax analysis components (future)
│   ├── semantic/                 # Semantic analysis components
│   │   ├── semantic.c            # Main semantic analyzer
│   │   └── semantic_integration.c # Integration wrapper
│   ├── codegen/                  # Code generation components
│   │   ├── codegen.c             # Code generator implementation
│   │   └── register_allocator.c  # Register allocation algorithms
│   └── utils/                    # Utility components
│       ├── error_handler.c       # Comprehensive error handling
│       └── output_generator.c    # Output file generation
├── include/                      # Header files organized by component
│   ├── core/                     # Core component headers
│   │   └── ast.h                 # AST definitions and declarations
│   ├── lexical/                  # Lexical analysis headers (future)
│   ├── syntax/                   # Syntax analysis headers (future)
│   ├── semantic/                 # Semantic analysis headers
│   │   └── semantic_integration.h # Semantic integration interface
│   ├── codegen/                  # Code generation headers
│   │   ├── codegen.h             # Code generation interface
│   │   └── register_allocator.h  # Register allocation interface
│   └── utils/                    # Utility headers
│       ├── error_handler.h       # Error handling interface
│       └── output_generator.h    # Output generation interface
├── build/                        # Build artifacts and executables
│   └── compiler         # Main executable (after build)
├── tests/                        # Test files and outputs
│   ├── unit/                     # Unit tests
│   │   └── test_suite.c          # Test suite implementation
│   ├── integration/              # Integration tests
│   ├── examples/                 # Test input files
│   │   ├── simple_test.txt       # Simple test case
│   │   ├── comprehensive_test.txt # Comprehensive test case
│   │   ├── minimal_test.txt      # Minimal test case
│   │   └── sample_input.txt      # Sample input for testing
│   └── outputs/                  # Test output directories
│       ├── assembly/             # Generated assembly files
│       └── reports/              # Test reports and analysis
├── scripts/                      # Build and test scripts
│   ├── build/                    # Build scripts
│   │   ├── build.bat             # Windows build script
│   │   └── comprehensive_build.sh # Unix build script
│   └── test/                     # Test scripts
│       ├── run_tests.sh          # Main test runner
│       ├── test_compiler.bat     # Windows test script
│       └── test_enhanced.sh      # Enhanced test script
├── docs/                         # Documentation
│   ├── design/                   # Design documents
│   │   ├── DESIGN_PROJECT_DOCUMENTATION.md
│   │   ├── DESIGN_PROJECT_REQUIREMENTS_ANALYSIS.md
│   │   ├── PROJECT_COMPLETION_SUMMARY.md
│   │   ├── TMA03_INTEGRATION_DOCUMENTATION.md
│   │   └── DELIVERABLES_SUMMARY.md
│   ├── requirements/             # Requirements documents (future)
│   └── testing/                  # Testing documentation
│       ├── TEST_RESULTS.md       # Test results summary
│       └── TEST_RESULTS_SUMMARY.md # Detailed test results
├── output/                       # Generated output files
│   ├── assembly/                 # Generated assembly code
│   │   └── output.s              # Assembly output
│   └── reports/                  # Generated reports
│       └── semantic_output.derivation # Semantic analysis output
├── Makefile                      # Main build configuration
└── README.md                     # This file
```

## Building the Project

### Prerequisites
- GCC compiler
- Flex (lexical analyzer generator)
- Make utility (Unix/Linux) or use provided batch scripts (Windows)

### Build Commands

#### Using Make (Recommended)
```bash
# Build the complete compiler
make all

# Clean build artifacts
make clean

# Run comprehensive test suite
make test

# Show project structure
make structure

# Show build information
make info

# Show help
make help
```

#### Using Scripts
```bash
# Windows
scripts\build\build.bat

# Unix/Linux
scripts/build/comprehensive_build.sh
```

## Testing

### Running Tests
```bash
# Using Make
make test

# Using scripts directly
scripts/test/run_tests.sh          # Unix/Linux
scripts\test\test_compiler.bat     # Windows
```

### Test Files
- `tests/examples/simple_test.txt` - Basic functionality test
- `tests/examples/comprehensive_test.txt` - Full feature test
- `tests/examples/minimal_test.txt` - Minimal viable test
- `tests/examples/sample_input.txt` - General sample input

## Architecture

### Compiler Phases
1. **Lexical Analysis** (`src/lexical/`) - Tokenization using Flex
2. **Syntax Analysis** (`src/syntax/`) - Parsing (future expansion)
3. **Semantic Analysis** (`src/semantic/`) - Type checking and symbol table management
4. **Code Generation** (`src/codegen/`) - Assembly code generation with register allocation

### Key Components
- **Enhanced Compiler** (`src/core/compiler.c`) - Main driver program
- **AST** (`src/core/ast.c`) - Abstract Syntax Tree implementation
- **Error Handler** (`src/utils/error_handler.c`) - Comprehensive error reporting
- **Output Generator** (`src/utils/output_generator.c`) - Multi-format output generation
- **Register Allocator** (`src/codegen/register_allocator.c`) - Efficient register management

## Development Guidelines

### Adding New Components
1. Place source files in appropriate `src/` subdirectory
2. Place headers in corresponding `include/` subdirectory
3. Update Makefile to include new files
4. Update include paths if necessary
5. Add appropriate tests

### Include Path Convention
All include statements use relative paths from the source file to the include directory:
```c
#include "../../include/component/header.h"
```

### Building Individual Components
The Makefile supports building individual components for development and debugging.

## Integration Notes

This project integrates components from previous assignments:
- **TMA01**: Lexical analysis foundation
- **TMA02**: Syntax analysis components  
- **TMA03**: Comprehensive semantic analysis (6000+ lines)

All integration is handled through the organized directory structure with proper header dependencies.

## Output Files

The compiler generates various output files in the `output/` directory:
- Assembly code (`output/assembly/`)
- Analysis reports (`output/reports/`)
- Test results (`tests/outputs/`)

## Support

For build issues or questions about the project structure:
1. Check `make help` for available commands
2. Verify all prerequisites are installed
3. Ensure proper directory structure is maintained
4. Check include paths in source files

---
*Design Project - Complete Compiler Implementation*
*Organized directory structure for maintainable development*