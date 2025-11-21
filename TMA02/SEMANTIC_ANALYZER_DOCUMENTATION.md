# TMA 03: Semantic Analyzer Implementation

## Overview

This document describes the implementation of a semantic analyzer for the programming language specified in TMA #2. The semantic analyzer performs syntax-directed translation to generate an Abstract Syntax Tree (AST) and then conducts semantic analysis through symbol table generation and type checking.

## 1. Abstract Syntax Tree (AST) Structure

### 1.1 AST Node Types

The AST uses a hierarchical node structure with the following main categories:

#### Program Structure Nodes
- `AST_PROGRAM`: Root node of the program
- `AST_CLASS_DECL`: Class declaration
- `AST_IMPL_DEF`: Implementation definition
- `AST_FUNC_DEF`: Function definition
- `AST_FUNC_DECL`: Function declaration (in class)
- `AST_FUNC_HEAD`: Function header
- `AST_FUNC_BODY`: Function body

#### Declaration Nodes
- `AST_VAR_DECL`: Variable declaration
- `AST_ATTR_DECL`: Attribute declaration
- `AST_LOCAL_VAR_DECL`: Local variable declaration
- `AST_PARAM_DECL`: Parameter declaration

#### Statement Nodes
- `AST_ASSIGN_STMT`: Assignment statement
- `AST_IF_STMT`: If statement
- `AST_WHILE_STMT`: While statement
- `AST_READ_STMT`: Read statement
- `AST_WRITE_STMT`: Write statement
- `AST_RETURN_STMT`: Return statement
- `AST_FUNC_CALL_STMT`: Function call statement
- `AST_STAT_BLOCK`: Statement block

#### Expression Nodes
- `AST_BINARY_EXPR`: Binary expression (+, -, *, /, ==, !=, <, >, <=, >=, and, or)
- `AST_UNARY_EXPR`: Unary expression (+, -, not)
- `AST_FUNC_CALL_EXPR`: Function call expression
- `AST_VARIABLE`: Variable reference
- `AST_ARRAY_ACCESS`: Array element access
- `AST_MEMBER_ACCESS`: Object member access
- `AST_INT_LITERAL`: Integer literal
- `AST_FLOAT_LITERAL`: Float literal

#### Type and Utility Nodes
- `AST_TYPE`: Type specification
- `AST_ARRAY_SIZE`: Array dimension size
- `AST_ID`: Identifier
- `AST_VISIBILITY`: Visibility modifier (public/private)

### 1.2 AST Node Structure

```c
struct ASTNode {
    ASTNodeType type;           // Node type
    int line, column;           // Source location
    
    // Node-specific data
    union {
        char* string_value;     // For identifiers, operators
        int int_value;          // For integer literals
        float float_value;      // For float literals
        DataType data_type;     // For type nodes
        Visibility visibility;   // For visibility nodes
    } data;
    
    // Semantic attributes (computed during analysis)
    DataType semantic_type;     // Computed type
    Symbol* symbol;             // Associated symbol table entry
    int array_dimensions;       // Array dimension count
    int* array_sizes;          // Array sizes
    
    // Tree structure
    ASTNode** children;        // Child nodes
    int child_count;           // Number of children
    ASTNode* next;             // Sibling pointer
};
```

## 2. Symbol Table Implementation

### 2.1 Symbol Types

The symbol table supports the following symbol types:

- `SYM_CLASS`: Class definitions
- `SYM_FUNC`: Function definitions
- `SYM_VAR`: Variable declarations
- `SYM_PARAM`: Function parameters
- `SYM_ATTR`: Class attributes

### 2.2 Data Types

Supported data types:
- `TYPE_INTEGER`: Integer type
- `TYPE_FLOAT`: Floating-point type
- `TYPE_VOID`: Void type (functions only)
- `TYPE_CLASS`: User-defined class type
- `TYPE_ARRAY`: Array type
- `TYPE_ERROR`: Error type for type checking
- `TYPE_UNKNOWN`: Unknown/unresolved type

### 2.3 Symbol Structure

```c
struct Symbol {
    char* name;                 // Symbol name
    SymbolType type;           // Symbol type
    DataType data_type;        // Data type
    Visibility visibility;      // Access modifier
    int line, column;          // Declaration location
    
    // Type-specific attributes
    union {
        struct {                // For functions
            ASTNode* param_list;
            DataType return_type;
            SymbolTable* local_scope;
        } func;
        
        struct {                // For classes
            SymbolTable* members;
            char** parent_classes;
            int parent_count;
        } class_info;
        
        struct {                // For arrays
            int dimensions;
            int* sizes;
            DataType element_type;
        } array;
    } attributes;
    
    SymbolTable* scope;        // Containing scope
    Symbol* next;              // Next symbol in scope
};
```

### 2.4 Symbol Table Structure

```c
struct SymbolTable {
    char* scope_name;          // Scope identifier
    Symbol* symbols;           // Symbol list
    SymbolTable* parent;       // Parent scope
    SymbolTable* children;     // Child scopes
    SymbolTable* next_sibling; // Sibling scopes
    int scope_level;           // Nesting level
};
```

## 3. Semantic Analysis Phases

The semantic analyzer operates in two distinct phases:

### 3.1 Phase 1: Symbol Table Generation

**Purpose**: Build symbol tables for all scopes and analyze declarations.

**Process**:
1. **Global Scope Creation**: Initialize global symbol table
2. **Declaration Processing**: 
   - Process class declarations and create class scopes
   - Process function declarations and create function scopes
   - Process variable and attribute declarations
   - Handle inheritance relationships
3. **Scope Management**: 
   - Enter and exit scopes appropriately
   - Maintain parent-child relationships between scopes
4. **Symbol Registration**: 
   - Insert symbols into appropriate scopes
   - Check for duplicate declarations
   - Store type information and attributes

**Key Functions**:
- `analyze_class_declaration()`: Process class definitions
- `analyze_function_definition()`: Process function definitions  
- `analyze_variable_declaration()`: Process variable declarations
- `analyze_inheritance()`: Handle class inheritance
- `insert_symbol()`: Add symbols to tables with duplicate checking

### 3.2 Phase 2: Type Checking and Semantic Validation

**Purpose**: Perform type checking and semantic validation on statements and expressions.

**Process**:
1. **Expression Type Checking**: 
   - Compute types for all expressions
   - Check type compatibility for operations
   - Validate function calls and argument types
2. **Statement Validation**:
   - Check assignment compatibility
   - Validate control flow statements
   - Ensure return statements match function types
3. **Scope and Access Checking**:
   - Verify variable and function references
   - Check visibility and access permissions
   - Validate member access operations
4. **Semantic Constraint Validation**:
   - Ensure proper use of language constructs
   - Check for unreachable code or missing returns

**Key Functions**:
- `analyze_expression()`: Type checking for expressions
- `analyze_assignment()`: Assignment compatibility checking
- `analyze_function_call()`: Function call validation
- `analyze_variable_access()`: Variable reference checking

## 4. Semantic Rules and Constraints

### 4.1 Declaration Rules

1. **Unique Names**: No duplicate names in the same scope
2. **Forward References**: Classes must be declared before use in inheritance
3. **Implementation Completeness**: All declared functions must be implemented
4. **Parameter Matching**: Function implementations must match declarations

### 4.2 Type Compatibility Rules

1. **Exact Match**: Same types are always compatible
2. **Numeric Conversion**: Integer ↔ Float for arithmetic operations
3. **Assignment**: Right-hand side must be compatible with left-hand side
4. **Return Types**: Return expressions must match function return type

### 4.3 Expression Type Rules

#### Binary Expressions
- **Arithmetic (`+`, `-`, `*`, `/`)**: 
  - Operands: integer or float
  - Result: float if either operand is float, otherwise integer
- **Relational (`==`, `!=`, `<`, `>`, `<=`, `>=`)**:
  - Operands: compatible types
  - Result: integer (boolean)
- **Logical (`and`, `or`)**:
  - Operands: integer (boolean)
  - Result: integer (boolean)

#### Unary Expressions
- **Arithmetic (`+`, `-`)**:
  - Operand: integer or float
  - Result: same as operand
- **Logical (`not`)**:
  - Operand: integer (boolean)
  - Result: integer (boolean)

### 4.4 Access Control Rules

1. **Private Members**: Accessible only within the same class
2. **Public Members**: Accessible from any context
3. **Inheritance**: Derived classes can access public members of parent classes
4. **Self Reference**: `self` keyword provides access to current object members

### 4.5 Control Flow Rules

1. **Conditional Statements**: Conditions must be integer (boolean) type
2. **Return Statements**: 
   - Must be inside function scope
   - Type must match function return type
   - Non-void functions must have return statements
3. **Assignment Targets**: Left-hand side must be assignable (variables, array elements, members)

## 5. Error Detection and Reporting

### 5.1 Error Categories

#### Syntax Errors (from parser)
- Missing tokens, malformed constructs

#### Semantic Errors (from analyzer)
- **REDECLARATION**: Duplicate symbol names in same scope
- **UNDEFINED_VARIABLE**: Use of undeclared variables
- **UNDEFINED_FUNCTION**: Calls to undeclared functions
- **UNDEFINED_CLASS**: Reference to undeclared classes
- **UNDEFINED_TYPE**: Use of unknown types
- **UNDEFINED_MEMBER**: Access to non-existent class members
- **TYPE_MISMATCH**: Incompatible types in operations
- **TYPE_ERROR**: Invalid operand types for operations
- **ASSIGNMENT_ERROR**: Invalid assignment targets
- **ACCESS_VIOLATION**: Unauthorized access to private members
- **ARGUMENT_COUNT_MISMATCH**: Wrong number of function arguments
- **RETURN_ERROR**: Missing or incompatible return statements
- **SCOPE_ERROR**: Statements in wrong scope context

### 5.2 Error Reporting Features

1. **Location Information**: Line and column numbers for all errors
2. **Ordered Reporting**: Errors reported in source order across phases
3. **Detailed Messages**: Descriptive error messages with context
4. **Error Recovery**: Continue analysis after errors to find multiple issues
5. **Separate Error File**: Semantic errors written to dedicated output file

### 5.3 Warning System

- **Access Pattern Warnings**: Potential access issues
- **Type Conversion Warnings**: Implicit type conversions
- **Unreachable Code Warnings**: Statements after returns

## 6. Output Files

### 6.1 Symbol Table Output (`.symboltable`)

```
=== Scope: global (Level 0) ===
main            | FUNCTION   | integer    | public     | Line 45
Calculator      | CLASS      | class      | public     | Line 3

  === Scope: class_Calculator (Level 1) ===
  add           | FUNCTION   | integer    | public     | Line 5
    Return Type: integer
  result        | ATTRIBUTE  | integer    | private    | Line 9
```

### 6.2 Semantic Error Report (`.semanticerrors`)

```
=== Semantic Analysis Report ===
Total Errors: 5
Total Warnings: 2

REDECLARATION at line 23, column 5: Redeclaration of 'testFunction' (previously declared at line 15)
TYPE_MISMATCH at line 18, column 16: Type mismatch in assignment: cannot assign float to integer
UNDEFINED_VARIABLE at line 35, column 5: Undefined variable 'undefinedVar'
ACCESS_VIOLATION at line 38, column 9: Cannot access private member 'result'
RETURN_ERROR at line 52, column 1: Non-void function must return a value
```

### 6.3 Derivation Output (`.derivation`)

```
prog → classOrImplOrFuncList
classOrImplOrFuncList → classOrImplOrFunc classOrImplOrFuncList
classOrImplOrFunc → classDecl
classDecl → class id inheritanceOpt { visibilityMemberList } ;
...
```

## 7. Usage Instructions

### 7.1 Building the Project

```bash
# Build the semantic analyzer
make all

# Build with debug information
make debug

# Build optimized version
make release
```

### 7.2 Running the Analyzer

```bash
# Analyze a single file
./build/semantic_analyzer program.txt

# Files generated:
# - program.txt.derivation      (parsing derivation)
# - program.txt.symboltable     (symbol table)
# - program.txt.semanticerrors  (semantic error report)
```

### 7.3 Testing

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

## 8. Implementation Architecture

### 8.1 Component Interaction

```
Source Code
     ↓
[Lexical Analyzer] → Tokens
     ↓
[Parser] → AST + Symbol Tables (Phase 1)
     ↓
[Semantic Analyzer] → Type Checking (Phase 2)
     ↓
Symbol Table + Error Report
```

### 8.2 Key Design Decisions

1. **Two-Phase Analysis**: Separates declaration processing from type checking
2. **Unified AST**: Single tree structure for all language constructs  
3. **Hierarchical Symbol Tables**: Mirrors program scope structure
4. **Comprehensive Error Tracking**: Maintains error order across phases
5. **Extensible Type System**: Supports user-defined types and arrays
6. **Memory Management**: Proper cleanup of all allocated structures

### 8.3 Performance Considerations

- **Symbol Lookup**: O(n) per scope level, optimized for typical nesting depth
- **AST Traversal**: Single-pass for each phase
- **Memory Usage**: Efficient storage with proper cleanup
- **Error Reporting**: Maintains order without significant performance impact

## 9. Future Enhancements

1. **Advanced Type Checking**: Generic types, function overloading
2. **Optimization**: Better symbol table data structures (hash tables)
3. **Enhanced Error Recovery**: More sophisticated error recovery strategies
4. **Code Generation Interface**: Prepare for next compilation phase
5. **Interactive Mode**: Real-time semantic analysis for IDEs

This semantic analyzer provides a solid foundation for the next phases of compilation while ensuring comprehensive error detection and clear reporting for developers.