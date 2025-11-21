# Design Project Documentation

## Complete Compiler Implementation

This document describes the complete implementation of the compiler design project, including the code generation phase that finalizes the compiler built through TMA1, TMA2, and TMA3.

---

## 1. Architecture Overview

### 1.1 Four-Phase Compilation Process

The compiler implements a traditional four-phase compilation pipeline:

1. **Lexical Analysis** (TMA1): Tokenization and lexical error detection
2. **Syntax Analysis** (TMA2): Parsing with Abstract Syntax Tree generation
3. **Semantic Analysis** (TMA3): Symbol table construction and type checking
4. **Code Generation** (Design Project): Target code generation with optimization

### 1.2 Target Architecture

- **Target Platform**: x86-64 architecture
- **Assembly Format**: AT&T syntax
- **Calling Convention**: System V ABI
- **Register Model**: 16 general-purpose registers
- **Memory Model**: Stack-based activation records

---

## 2. Register Allocation and Deallocation Scheme

### 2.1 Register Set

The compiler uses the x86-64 register set with the following allocation:

```
General Purpose Registers:
- RAX: Accumulator, function return values
- RBX: Base register, callee-saved
- RCX: Counter, function argument 4
- RDX: Data register, function argument 3
- RSI: Source index, function argument 2
- RDI: Destination index, function argument 1
- R8-R9: Function arguments 5-6
- R10-R11: Temporary registers
- R12-R15: Callee-saved registers

Special Registers:
- RSP: Stack pointer (reserved)
- RBP: Base pointer (reserved)
```

### 2.2 Allocation Strategies

#### 2.2.1 Naive Allocation
- **Strategy**: First-available register allocation
- **Implementation**: Linear scan through register table
- **Use Case**: Simple programs with low register pressure
- **Advantages**: Fast allocation, simple implementation
- **Disadvantages**: Poor register utilization

#### 2.2.2 Least Recently Used (LRU)
- **Strategy**: Replace the register that was used longest ago
- **Implementation**: Timestamp-based tracking
- **Use Case**: Medium complexity programs
- **Advantages**: Good locality of reference
- **Disadvantages**: Overhead of timestamp maintenance

#### 2.2.3 Optimal Allocation (Belady's Algorithm)
- **Strategy**: Replace the register that will be used furthest in the future
- **Implementation**: Look-ahead analysis
- **Use Case**: Critical performance sections
- **Advantages**: Theoretically optimal
- **Disadvantages**: Requires future use information

#### 2.2.4 Graph Coloring
- **Strategy**: Model register allocation as graph coloring problem
- **Implementation**: Interference graph construction
- **Use Case**: Complex programs with high register pressure
- **Advantages**: Global optimization
- **Disadvantages**: Complex implementation, compilation time

### 2.3 Register Deallocation

#### 2.3.1 Explicit Deallocation
```c
void free_register(CodeGenContext* ctx, Register reg) {
    if (ctx->registers[reg].status != REG_RESERVED) {
        ctx->registers[reg].status = REG_FREE;
        ctx->registers[reg].variable = NULL;
        ctx->registers[reg].last_use = -1;
    }
}
```

#### 2.3.2 Automatic Deallocation
- **End of variable scope**: Registers freed when variables go out of scope
- **Function calls**: Caller-saved registers automatically managed
- **Spill operations**: Registers freed during spill-and-reload

---

## 3. Memory Usage Scheme

### 3.1 Stack-Based Memory Management

The compiler uses a stack-based memory model with the following organization:

```
High Address
+------------------+
| Previous Frame   |
+------------------+
| Return Address   |
+------------------+
| Saved RBP        | <- Current RBP
+------------------+
| Local Variables  |
+------------------+
| Temporary Values |
+------------------+
| Spill Area       |
+------------------+
| Function Args    |
+------------------+ <- Current RSP
Low Address
```

### 3.2 Memory Layout Components

#### 3.2.1 Activation Records
```c
typedef struct {
    int total_stack_size;    // Total frame size
    int local_vars_size;     // Local variable storage
    int temp_vars_size;      // Temporary value storage
    int params_size;         // Parameter storage
    int spill_area_size;     // Register spill area
    int max_call_args;       // Maximum arguments for calls
} MemoryLayout;
```

#### 3.2.2 Variable Storage
- **Local Variables**: Allocated in activation record
- **Function Parameters**: Passed via registers (first 6) then stack
- **Temporary Variables**: Allocated in temporary area
- **Spilled Registers**: Stored in dedicated spill area

### 3.3 Memory Allocation Algorithm

```c
void calculate_memory_layout(CodeGenContext* ctx, ASTNode* function) {
    // Calculate space for local variables
    analyze_local_variables(function);
    
    // Calculate space for temporaries
    analyze_temporary_needs(function);
    
    // Calculate spill area size
    ctx->memory_layout.spill_area_size = MAX_REGISTERS * WORD_SIZE;
    
    // Align to stack boundary
    int total = ctx->memory_layout.local_vars_size + 
                ctx->memory_layout.temp_vars_size + 
                ctx->memory_layout.spill_area_size;
    ctx->memory_layout.total_stack_size = 
        ALIGN(total, STACK_ALIGNMENT);
}
```

### 3.4 Memory Access Patterns

#### 3.4.1 Variable Access
```assembly
# Local variable access
mov -8(%rbp), %rax     # Load local variable
mov %rax, -16(%rbp)    # Store to local variable
```

#### 3.4.2 Parameter Access
```assembly
# First 6 parameters in registers: RDI, RSI, RDX, RCX, R8, R9
# Additional parameters on stack
mov 16(%rbp), %rax     # Load 7th parameter
```

#### 3.4.3 Spill/Reload Operations
```assembly
# Spill register to memory
mov %rax, -24(%rbp)    # Spill RAX to stack

# Reload from memory
mov -24(%rbp), %rax    # Reload RAX from stack
```

---

## 4. Code Generation Phases

### 4.1 Phase Overview

The code generation phase consists of several sub-phases:

1. **AST Traversal**: Walk the abstract syntax tree
2. **Instruction Selection**: Choose target instructions
3. **Register Allocation**: Assign registers to variables
4. **Code Emission**: Generate assembly code
5. **Optimization**: Peephole and local optimizations

### 4.2 Semantic Actions to AST Node Mapping

#### 4.2.1 Program Structure Nodes

**AST_PROGRAM**
- **Purpose**: Root of the compilation unit
- **Semantic Action**: Initialize global scope, process top-level declarations
- **Code Generation**: Emit file header, process all children
```c
void generate_program_code(CodeGenContext* ctx, ASTNode* program) {
    emit_assembly_prologue(ctx);
    for (int i = 0; i < program->child_count; i++) {
        generate_declaration_code(ctx, program->children[i]);
    }
    emit_assembly_epilogue(ctx);
}
```

**AST_FUNC_DEF**
- **Purpose**: Function definition with body
- **Semantic Action**: Create function scope, analyze parameters and locals
- **Code Generation**: Emit function label, prologue, body, epilogue
```c
void generate_function_code(CodeGenContext* ctx, ASTNode* func_def) {
    calculate_memory_layout(ctx, func_def);
    emit_function_label(ctx, get_function_name(func_def));
    emit_function_prologue(ctx);
    generate_statement_code(ctx, get_function_body(func_def));
    emit_function_epilogue(ctx);
}
```

**AST_CLASS_DECL**
- **Purpose**: Class declaration with members
- **Semantic Action**: Create class scope, process member declarations
- **Code Generation**: Generate data layout information (no runtime code)

#### 4.2.2 Statement Nodes

**AST_ASSIGN_STMT**
- **Purpose**: Variable assignment
- **Semantic Action**: Type checking, lvalue validation
- **Code Generation**: Generate RHS expression, store to LHS location
```c
void generate_assignment(CodeGenContext* ctx, ASTNode* assign) {
    Operand rhs_result;
    generate_expression_code(ctx, assign->children[1], &rhs_result);
    
    Operand lhs = create_variable_operand(assign->children[0]->symbol);
    emit_instruction(ctx, create_instruction(INST_MOV, lhs, rhs_result));
}
```

**AST_IF_STMT**
- **Purpose**: Conditional execution
- **Semantic Action**: Condition type checking
- **Code Generation**: Generate condition, conditional jumps, branches
```c
void generate_if_statement(CodeGenContext* ctx, ASTNode* if_stmt) {
    char* else_label = generate_label(ctx, "else");
    char* end_label = generate_label(ctx, "endif");
    
    // Generate condition and jump
    Operand condition;
    generate_expression_code(ctx, if_stmt->children[0], &condition);
    emit_instruction(ctx, create_instruction(INST_CMP, condition, zero));
    emit_instruction(ctx, create_instruction(INST_JE, else_label));
    
    // Generate then branch
    generate_statement_code(ctx, if_stmt->children[1]);
    
    if (if_stmt->child_count > 2) {  // Has else branch
        emit_instruction(ctx, create_instruction(INST_JMP, end_label));
        emit_label(ctx, else_label);
        generate_statement_code(ctx, if_stmt->children[2]);
    }
    
    emit_label(ctx, end_label);
}
```

**AST_WHILE_STMT**
- **Purpose**: Loop execution
- **Semantic Action**: Condition type checking
- **Code Generation**: Generate loop with condition testing
```c
void generate_while_loop(CodeGenContext* ctx, ASTNode* while_stmt) {
    char* loop_label = generate_label(ctx, "while_loop");
    char* end_label = generate_label(ctx, "while_end");
    
    emit_label(ctx, loop_label);
    
    // Generate condition
    Operand condition;
    generate_expression_code(ctx, while_stmt->children[0], &condition);
    emit_instruction(ctx, create_instruction(INST_CMP, condition, zero));
    emit_instruction(ctx, create_instruction(INST_JE, end_label));
    
    // Generate body
    generate_statement_code(ctx, while_stmt->children[1]);
    emit_instruction(ctx, create_instruction(INST_JMP, loop_label));
    
    emit_label(ctx, end_label);
}
```

**AST_RETURN_STMT**
- **Purpose**: Function return
- **Semantic Action**: Return type checking
- **Code Generation**: Move result to RAX, jump to epilogue
```c
void generate_return_statement(CodeGenContext* ctx, ASTNode* return_stmt) {
    if (return_stmt->child_count > 0) {
        Operand return_value;
        generate_expression_code(ctx, return_stmt->children[0], &return_value);
        
        Operand rax = create_register_operand(REG_RAX, return_value.data_type);
        emit_instruction(ctx, create_instruction(INST_MOV, rax, return_value));
    }
    
    emit_instruction(ctx, create_instruction(INST_RET));
}
```

#### 4.2.3 Expression Nodes

**AST_BINARY_EXPR**
- **Purpose**: Binary operations (+, -, *, /, comparisons)
- **Semantic Action**: Type checking, operator compatibility
- **Code Generation**: Generate operands, perform operation, store result
```c
void generate_binary_expression(CodeGenContext* ctx, ASTNode* binary_expr, Operand* result) {
    Operand left, right;
    generate_expression_code(ctx, binary_expr->children[0], &left);
    generate_expression_code(ctx, binary_expr->children[2], &right);
    
    Register result_reg = allocate_register(ctx, NULL);
    *result = create_register_operand(result_reg, TYPE_INTEGER);
    
    // Move left operand to result register
    emit_instruction(ctx, create_instruction(INST_MOV, *result, left));
    
    // Perform operation
    char* op = binary_expr->children[1]->data.string_value;
    InstructionType inst_type = get_instruction_for_operator(op);
    emit_instruction(ctx, create_instruction(inst_type, *result, *result, right));
}
```

**AST_FUNC_CALL_EXPR**
- **Purpose**: Function call with return value
- **Semantic Action**: Parameter type checking, function lookup
- **Code Generation**: Setup arguments, call function, retrieve result
```c
void generate_function_call(CodeGenContext* ctx, ASTNode* call_expr, Operand* result) {
    // Save caller-saved registers
    save_caller_saved_registers(ctx);
    
    // Setup arguments (simplified - first 6 in registers)
    setup_function_arguments(ctx, call_expr);
    
    // Generate call instruction
    char* func_name = call_expr->children[0]->data.string_value;
    emit_instruction(ctx, create_instruction(INST_CALL, func_name));
    
    // Get result from RAX
    Register result_reg = allocate_register(ctx, NULL);
    *result = create_register_operand(result_reg, TYPE_INTEGER);
    
    Operand rax = create_register_operand(REG_RAX, TYPE_INTEGER);
    emit_instruction(ctx, create_instruction(INST_MOV, *result, rax));
    
    // Restore caller-saved registers
    restore_caller_saved_registers(ctx);
}
```

**AST_VARIABLE**
- **Purpose**: Variable reference
- **Semantic Action**: Symbol lookup, scope validation
- **Code Generation**: Generate load from variable's memory location
```c
void generate_variable_access(CodeGenContext* ctx, ASTNode* var_node, Operand* result) {
    Symbol* var_symbol = var_node->symbol;
    
    // Check if variable is already in a register
    Register var_reg = find_variable_register(ctx, var_symbol);
    if (var_reg != REG_NONE) {
        *result = create_register_operand(var_reg, var_symbol->data_type);
        return;
    }
    
    // Variable is in memory - load it
    Register load_reg = allocate_register(ctx, var_symbol);
    *result = create_register_operand(load_reg, var_symbol->data_type);
    
    int var_offset = get_variable_offset(ctx, var_symbol);
    Operand mem_op = create_memory_operand(REG_RBP, var_offset, var_symbol->data_type);
    emit_instruction(ctx, create_instruction(INST_MOV, *result, mem_op));
}
```

#### 4.2.4 Literal Nodes

**AST_INT_LITERAL**
- **Purpose**: Integer constant
- **Semantic Action**: Value validation
- **Code Generation**: Create immediate operand
```c
void generate_integer_literal(ASTNode* literal, Operand* result) {
    *result = create_immediate_operand(literal->data.int_value);
    result->data_type = TYPE_INTEGER;
}
```

**AST_FLOAT_LITERAL**
- **Purpose**: Floating-point constant
- **Semantic Action**: Value validation
- **Code Generation**: Create floating-point immediate operand
```c
void generate_float_literal(ASTNode* literal, Operand* result) {
    *result = create_float_operand(literal->data.float_value);
    result->data_type = TYPE_FLOAT;
}
```

### 4.3 Effect and Role of Each Semantic Action

#### 4.3.1 Declaration Processing
- **Effect**: Creates symbol table entries, allocates storage
- **Role**: Establishes program structure and variable binding
- **Implementation**: Traverses AST during symbol table construction phase

#### 4.3.2 Type Checking
- **Effect**: Validates type compatibility, prevents runtime errors
- **Role**: Ensures program correctness before code generation
- **Implementation**: Recursive type inference with error reporting

#### 4.3.3 Scope Management
- **Effect**: Enforces variable visibility rules
- **Role**: Prevents name conflicts, enables nested scopes
- **Implementation**: Hierarchical symbol table stack

#### 4.3.4 Code Generation Actions
- **Effect**: Translates high-level constructs to assembly
- **Role**: Produces executable target code
- **Implementation**: AST traversal with instruction emission

---

## 5. Implementation Details

### 5.1 Data Structures

#### 5.1.1 Code Generation Context
```c
struct CodeGenContext {
    FILE* output_file;                    // Assembly output
    RegisterDescriptor registers[16];     // Register state
    VariableDescriptor* variables;        // Variable mapping
    MemoryLayout memory_layout;           // Stack frame info
    int label_counter;                    // Unique label generation
    Symbol* current_function;             // Current function context
    int optimize_level;                   // Optimization settings
};
```

#### 5.1.2 Instruction Representation
```c
struct Instruction {
    InstructionType type;                 // Operation type
    Operand dest, src1, src2;            // Operands
    char* comment;                        // Documentation
    int line_number;                      // Source correlation
    struct Instruction* next;             // Linked list
};
```

#### 5.1.3 Register Descriptor
```c
typedef struct {
    RegisterStatus status;                // Free/allocated/reserved
    Symbol* variable;                     // Currently held variable
    int last_use;                        // LRU timestamp
    int spill_cost;                      // Cost of spilling
} RegisterDescriptor;
```

### 5.2 Code Generation Pipeline

1. **Initialization**: Create code generation context, initialize registers
2. **Memory Layout**: Calculate stack frame size and variable offsets
3. **AST Traversal**: Walk syntax tree, generate instructions
4. **Register Management**: Allocate/deallocate registers as needed
5. **Code Emission**: Write assembly instructions to output file
6. **Cleanup**: Free resources, close files

### 5.3 Optimization Opportunities

#### 5.3.1 Peephole Optimizations
- **Redundant Move Elimination**: Remove `mov %rax, %rax`
- **Strength Reduction**: Replace multiplication by powers of 2 with shifts
- **Dead Code Elimination**: Remove unreachable instructions

#### 5.3.2 Register Allocation Optimizations
- **Coalescing**: Merge copy-related variables into same register
- **Live Range Splitting**: Split variables with non-contiguous live ranges
- **Rematerialization**: Recompute instead of spill for cheap operations

---

## 6. Testing and Validation

### 6.1 Test Categories

#### 6.1.1 Unit Tests
- **Register Allocation**: Test allocation strategies with various patterns
- **Memory Management**: Verify stack frame calculation
- **Instruction Generation**: Check correct instruction selection

#### 6.1.2 Integration Tests
- **End-to-End**: Complete compilation from source to assembly
- **Phase Integration**: Verify data flow between compilation phases
- **Error Handling**: Test error recovery and reporting

#### 6.1.3 Correctness Tests
- **Semantic Preservation**: Verify generated code maintains program semantics
- **Type Safety**: Ensure type checking prevents invalid operations
- **Control Flow**: Test correct implementation of conditionals and loops

### 6.2 Performance Metrics

#### 6.2.1 Compilation Metrics
- **Compilation Time**: Time taken for each phase
- **Memory Usage**: Peak memory consumption during compilation
- **Error Recovery**: Quality of error messages and recovery

#### 6.2.2 Generated Code Quality
- **Register Utilization**: Percentage of successful register allocations  
- **Code Size**: Number of generated instructions
- **Spill Frequency**: Number of register spills per function

---

## 7. Usage Examples

### 7.1 Basic Compilation
```bash
# Compile source file to assembly
./compiler program.src -o program.asm

# Verbose compilation with all information
./compiler -v -a -s -r -m program.src
```

### 7.2 Phase-Specific Compilation
```bash
# Run only lexical and syntax analysis
./compiler -p 3 program.src

# Run up to semantic analysis
./compiler -p 7 program.src

# Full compilation with register strategy
./compiler -R lru program.src
```

### 7.3 Debugging and Analysis
```bash
# Show AST and symbol tables
./compiler -a -s program.src

# Show register allocation details
./compiler -r -m program.src

# Compare allocation strategies
./compiler -R naive program.src -o naive.asm
./compiler -R lru program.src -o lru.asm
```

---

## 8. Conclusion

This design project completes the compiler implementation by adding a sophisticated code generation phase with:

- **Multi-strategy register allocation** for optimal resource utilization
- **Stack-based memory management** with efficient activation records  
- **Comprehensive instruction selection** for x86-64 target architecture
- **Integrated optimization pipeline** for improved code quality

The resulting compiler successfully transforms high-level source programs through all compilation phases to produce executable assembly code, demonstrating mastery of compiler construction principles and implementation techniques.

---

*Design Project - Complete Compiler Implementation*  
*EEX6363 - Compiler Design*