#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../core/ast.h"

// Forward declarations
typedef struct CodeGenContext CodeGenContext;
typedef struct Instruction Instruction;
typedef struct BasicBlock BasicBlock;

// Target architecture definitions
#define MAX_REGISTERS 16
#define STACK_ALIGNMENT 16
#define WORD_SIZE 8

// Register definitions for x86-64
typedef enum
{
    REG_RAX = 0,  // Accumulator, return value
    REG_RBX = 1,  // Base register, callee-saved
    REG_RCX = 2,  // Counter, function argument 4
    REG_RDX = 3,  // Data, function argument 3
    REG_RSI = 4,  // Source index, function argument 2
    REG_RDI = 5,  // Destination index, function argument 1
    REG_RBP = 6,  // Base pointer, callee-saved
    REG_RSP = 7,  // Stack pointer
    REG_R8 = 8,   // Function argument 5
    REG_R9 = 9,   // Function argument 6
    REG_R10 = 10, // Temporary
    REG_R11 = 11, // Temporary
    REG_R12 = 12, // Callee-saved
    REG_R13 = 13, // Callee-saved
    REG_R14 = 14, // Callee-saved
    REG_R15 = 15, // Callee-saved
    REG_NONE = -1 // No register assigned
} Register;

// Register allocation status
typedef enum
{
    REG_FREE = 0,
    REG_ALLOCATED = 1,
    REG_RESERVED = 2, // For special purposes (RSP, RBP)
    REG_CALLER_SAVED = 3,
    REG_CALLEE_SAVED = 4
} RegisterStatus;

// Instruction types
typedef enum
{
    INST_MOV,   // Move
    INST_ADD,   // Addition
    INST_SUB,   // Subtraction
    INST_MUL,   // Multiplication
    INST_DIV,   // Division
    INST_CMP,   // Compare
    INST_JMP,   // Unconditional jump
    INST_JE,    // Jump if equal
    INST_JNE,   // Jump if not equal
    INST_JL,    // Jump if less
    INST_JLE,   // Jump if less or equal
    INST_JG,    // Jump if greater
    INST_JGE,   // Jump if greater or equal
    INST_CALL,  // Function call
    INST_RET,   // Return
    INST_PUSH,  // Push to stack
    INST_POP,   // Pop from stack
    INST_LABEL, // Label
    INST_LOAD,  // Load from memory
    INST_STORE, // Store to memory
    INST_LEA    // Load effective address
} InstructionType;

// Operand types
typedef enum
{
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL,
    OPERAND_VARIABLE
} OperandType;

// Operand structure
typedef struct
{
    OperandType type;
    union
    {
        Register reg;
        int immediate;
        float float_immediate;
        char *label;
        struct
        {
            Register base;
            int offset;
        } memory;
        Symbol *variable;
    } value;
    DataType data_type;
} Operand;

// Instruction structure
struct Instruction
{
    InstructionType type;
    Operand dest;
    Operand src1;
    Operand src2;
    char *comment;
    int line_number;
    struct Instruction *next;
};

// Basic block for control flow
struct BasicBlock
{
    char *label;
    Instruction *instructions;
    struct BasicBlock **successors;
    int successor_count;
    struct BasicBlock **predecessors;
    int predecessor_count;
    struct BasicBlock *next;
};

// Register descriptor
typedef struct
{
    RegisterStatus status;
    Symbol *variable; // Variable currently in this register
    int last_use;     // Last instruction that used this register
    int spill_cost;   // Cost of spilling this register
} RegisterDescriptor;

// Variable descriptor
typedef struct
{
    Symbol *symbol;
    Register allocated_register;
    int stack_offset; // Offset from base pointer if spilled
    int is_spilled;
    int next_use; // Next instruction that will use this variable
} VariableDescriptor;

// Memory layout information
typedef struct
{
    int total_stack_size;
    int local_vars_size;
    int temp_vars_size;
    int params_size;
    int spill_area_size;
    int max_call_args;
} MemoryLayout;

// Code generation context
struct CodeGenContext
{
    FILE *output_file;
    BasicBlock *current_block;
    BasicBlock *first_block;

    // Register allocation
    RegisterDescriptor registers[MAX_REGISTERS];
    VariableDescriptor *variables;
    int variable_count;

    // Memory management
    MemoryLayout memory_layout;
    int current_stack_offset;

    // Label generation
    int label_counter;
    int temp_counter;

    // Function context
    Symbol *current_function;
    SymbolTable *current_scope;

    // Code generation options
    int optimize_level;
    int debug_info;
    int verbose;
};

// Code generation functions
CodeGenContext *create_codegen_context(FILE *output);
void destroy_codegen_context(CodeGenContext *ctx);

// Main code generation entry point
void generate_code(ASTNode *root, FILE *output);

// AST traversal for code generation
void generate_program_code(CodeGenContext *ctx, ASTNode *program);
void generate_class_code(CodeGenContext *ctx, ASTNode *class_decl);
void generate_function_code(CodeGenContext *ctx, ASTNode *func_def);
void generate_statement_code(CodeGenContext *ctx, ASTNode *stmt);
void generate_expression_code(CodeGenContext *ctx, ASTNode *expr, Operand *result);

// Instruction generation
Instruction *create_instruction(InstructionType type, Operand dest, Operand src1, Operand src2);
void emit_instruction(CodeGenContext *ctx, Instruction *inst);
void emit_label(CodeGenContext *ctx, const char *label);
void emit_comment(CodeGenContext *ctx, const char *comment);

// Operand creation helpers
Operand create_register_operand(Register reg, DataType type);
Operand create_immediate_operand(int value);
Operand create_float_operand(float value);
Operand create_memory_operand(Register base, int offset, DataType type);
Operand create_label_operand(const char *label);
Operand create_variable_operand(Symbol *var);

// Memory management
void calculate_memory_layout(CodeGenContext *ctx, ASTNode *function);
int allocate_stack_space(CodeGenContext *ctx, int size);
int get_variable_offset(CodeGenContext *ctx, Symbol *var);

// Label generation
char *generate_label(CodeGenContext *ctx, const char *prefix);
char *generate_temp_name(CodeGenContext *ctx);

// Register allocation interface
Register allocate_register(CodeGenContext *ctx, Symbol *var);
void free_register(CodeGenContext *ctx, Register reg);
void spill_register(CodeGenContext *ctx, Register reg);
Register find_free_register(CodeGenContext *ctx);
void save_caller_saved_registers(CodeGenContext *ctx);
void restore_caller_saved_registers(CodeGenContext *ctx);

// Assembly output
void emit_assembly_prologue(CodeGenContext *ctx);
void emit_assembly_epilogue(CodeGenContext *ctx);
void emit_function_prologue(CodeGenContext *ctx, Symbol *func);
void emit_function_epilogue(CodeGenContext *ctx, Symbol *func);

// Debug and optimization
void print_register_allocation_info(CodeGenContext *ctx);
void print_memory_layout_info(CodeGenContext *ctx);
void optimize_basic_blocks(CodeGenContext *ctx);

// Utility functions
const char *register_name(Register reg);
const char *instruction_name(InstructionType type);
int is_caller_saved(Register reg);
int is_callee_saved(Register reg);

#endif // CODEGEN_H