# TMA 03 - Semantic Analyzer

This is the implementation of TMA 03: a semantic analyzer for the programming language specified in TMA #2. The implementation includes Abstract Syntax Tree (AST) generation, symbol table management, and comprehensive semantic analysis.

## Features

### Core Components
- **Lexical Analyzer**: Extended from TMA #2 with enhanced error handling
- **Parser**: Extended from TMA #2 with AST generation using syntax-directed translation  
- **AST Generator**: Creates hierarchical abstract syntax tree representation
- **Symbol Table Manager**: Multi-level symbol tables with scope management
- **Semantic Analyzer**: Two-phase semantic analysis with type checking

### Semantic Analysis Capabilities
- **Declaration Analysis**: Class, function, and variable declarations
- **Inheritance Support**: Class inheritance with parent-child relationships
- **Type Checking**: Comprehensive type compatibility checking
- **Scope Management**: Nested scopes with proper visibility rules
- **Error Detection**: Detailed semantic error reporting with source locations

## File Structure

```
TMA02/
├── scanner.l              # Lexical analyzer (Flex)
├── parser.y              # Parser with AST generation (Bison)
├── ast.h                 # AST and symbol table definitions
├── ast.c                 # AST manipulation functions
├── semantic.c            # Semantic analysis implementation
├── Makefile              # Build configuration
├── SEMANTIC_ANALYZER_DOCUMENTATION.md  # Detailed documentation
├── test_valid_semantic.txt    # Sample valid program
├── test_error_semantic.txt    # Sample program with errors
└── build/                # Build output directory
```

## Building the Project

### Prerequisites
- GCC compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make utility

### Build Commands

```bash
# Build the semantic analyzer
make all

# Build with debug information
make debug

# Build optimized version
make release

# Clean build files
make clean
```

## Usage

### Basic Usage
```bash
# Analyze a program file
./build/semantic_analyzer program.txt
```

### Output Files
The analyzer generates several output files:

1. **program.txt.derivation** - Parse derivation steps
2. **program.txt.symboltable** - Complete symbol table dump
3. **program.txt.semanticerrors** - Semantic error report

### Example Output

**Symbol Table (program.txt.symboltable):**
```
=== Scope: global (Level 0) ===
Calculator      | CLASS      | class      | public     | Line 3
main           | FUNCTION   | integer    | public     | Line 45

  === Scope: class_Calculator (Level 1) ===
  add          | FUNCTION   | integer    | public     | Line 5
    Return Type: integer
  result       | ATTRIBUTE  | integer    | private    | Line 9
```

**Semantic Errors (program.txt.semanticerrors):**
```
=== Semantic Analysis Report ===
Total Errors: 3
Total Warnings: 1

TYPE_MISMATCH at line 18, column 16: Type mismatch in assignment: cannot assign float to integer
UNDEFINED_VARIABLE at line 25, column 5: Undefined variable 'undefinedVar'
ACCESS_VIOLATION at line 30, column 9: Cannot access private member 'result'
```

## Testing

### Run Test Suite
```bash
# Run all tests
make test

# Test valid programs only
make test-valid

# Test error programs only
make test-error

# Test specific file
make test-file FILE=myprogram.txt
```

### Sample Programs

**Valid Program** (test_valid_semantic.txt):
```cpp
class Shape {
    public:
        attribute area : float;
        func calculateArea() => float;
    private:
        attribute width : float;
}

func main() => integer {
    local shape : Shape;
    local result : float;
    
    result := shape.calculateArea();
    write(result);
    return(0);
}
```

**Error Program** (test_error_semantic.txt):
```cpp
class Calculator {
    public:
        func add(a : integer, b : integer) => integer;
    private:
        attribute result : integer;
}

func testErrors() => integer {
    local calc : Calculator;
    local num : float;
    
    // Type mismatch error
    calc.result := 3.14;
    
    // Undefined variable error  
    undefinedVar := 100;
    
    // Access violation error
    write(calc.result);
}
```

## Semantic Rules Implemented

### Type System
- **Basic Types**: integer, float, void, class types
- **Array Types**: Multi-dimensional arrays with size checking
- **Type Compatibility**: Automatic integer ↔ float conversion for arithmetic
- **Function Types**: Parameter and return type validation

### Scoping Rules  
- **Global Scope**: Classes and top-level functions
- **Class Scope**: Class members (attributes and methods)
- **Function Scope**: Local variables and parameters
- **Block Scope**: Statement blocks with local declarations

### Access Control
- **Public Members**: Accessible from any scope
- **Private Members**: Accessible only within the same class
- **Inheritance**: Proper visibility in class hierarchies

### Semantic Constraints
- **Unique Declarations**: No duplicate names in same scope
- **Forward References**: Proper declaration order
- **Type Consistency**: Assignment and expression type checking
- **Control Flow**: Proper return statement validation

## Error Categories

### Declaration Errors
- Redeclaration of identifiers
- Undefined types, variables, functions, classes
- Missing implementations

### Type Errors  
- Type mismatches in assignments and expressions
- Invalid operand types for operators
- Function argument type/count mismatches

### Access Errors
- Access to undefined members
- Violation of visibility rules
- Invalid assignment targets

### Control Flow Errors
- Missing return statements
- Invalid condition types
- Statements in wrong scope context

## Architecture

### Two-Phase Analysis
1. **Phase 1**: Symbol table generation and declaration analysis
2. **Phase 2**: Type checking and semantic validation

### AST Structure
- Hierarchical tree representing program structure
- Node types for all language constructs
- Semantic attributes attached to nodes

### Symbol Tables
- Hierarchical scope structure
- Symbol attributes (type, visibility, location)
- Efficient lookup with scope chain traversal

## Development Notes

### Extending the Analyzer
- Add new AST node types in `ast.h`
- Implement semantic rules in `semantic.c`  
- Update parser grammar in `parser.y`
- Add corresponding test cases

### Debugging
- Use `make debug` for debug build
- Check AST structure in console output
- Examine symbol table dumps
- Review error messages with line numbers

## Documentation

For detailed implementation information, see:
- **SEMANTIC_ANALYZER_DOCUMENTATION.md** - Complete technical documentation
- **ast.h** - Data structure definitions
- **semantic.c** - Implementation details

## Future Enhancements

Potential extensions for the semantic analyzer:
- Generic types and templates
- Function overloading support  
- Advanced optimization analysis
- Integration with code generation phase
- IDE integration capabilities