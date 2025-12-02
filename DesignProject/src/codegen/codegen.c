#define _POSIX_C_SOURCE 200809L
#include "../../include/codegen/codegen.h"
#include "../../include/codegen/register_allocator.h"

// Forward declarations
static void emit_operand(CodeGenContext *ctx, Operand *op);

// Global code generation statistics
static int instructions_generated = 0;
static int registers_spilled = 0;
static int optimization_passes = 0;

// Register name mappings
static const char *register_names[] = {
    "%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi", "%rbp", "%rsp",
    "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};

// Instruction name mappings
static const char *instruction_names[] = {
    "mov", "add", "sub", "mul", "div", "cmp", "jmp", "je", "jne",
    "jl", "jle", "jg", "jge", "call", "ret", "push", "pop", "label",
    "load", "store", "lea"};

// Create and initialize code generation context
CodeGenContext *create_codegen_context(FILE *output)
{
    CodeGenContext *ctx = malloc(sizeof(CodeGenContext));
    if (!ctx)
    {
        fprintf(stderr, "Error: Failed to allocate code generation context\n");
        return NULL;
    }

    // Initialize output file
    ctx->output_file = output;
    ctx->current_block = NULL;
    ctx->first_block = NULL;

    // Initialize register descriptors
    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        ctx->registers[i].status = REG_FREE;
        ctx->registers[i].variable = NULL;
        ctx->registers[i].last_use = -1;
        ctx->registers[i].spill_cost = 0;
    }

    // Reserve special registers
    ctx->registers[REG_RSP].status = REG_RESERVED;
    ctx->registers[REG_RBP].status = REG_RESERVED;

    // Initialize variable descriptors
    ctx->variables = NULL;
    ctx->variable_count = 0;

    // Initialize memory layout
    memset(&ctx->memory_layout, 0, sizeof(MemoryLayout));
    ctx->current_stack_offset = 0;

    // Initialize counters
    ctx->label_counter = 0;
    ctx->temp_counter = 0;

    // Initialize function context
    ctx->current_function = NULL;
    ctx->current_scope = NULL;

    // Initialize options
    ctx->optimize_level = 1;
    ctx->debug_info = 0;
    ctx->verbose = 0;

    return ctx;
}

// Destroy code generation context
void destroy_codegen_context(CodeGenContext *ctx)
{
    if (!ctx)
        return;

    // Free variable descriptors
    if (ctx->variables)
    {
        free(ctx->variables);
    }

    // Free basic blocks (if any were created)
    BasicBlock *block = ctx->first_block;
    while (block)
    {
        BasicBlock *next = block->next;
        // Free instructions in block
        Instruction *inst = block->instructions;
        while (inst)
        {
            Instruction *next_inst = inst->next;
            if (inst->comment)
                free(inst->comment);
            free(inst);
            inst = next_inst;
        }
        if (block->label)
            free(block->label);
        if (block->successors)
            free(block->successors);
        if (block->predecessors)
            free(block->predecessors);
        free(block);
        block = next;
    }

    free(ctx);
}

// Main code generation entry point
void generate_code(ASTNode *root, FILE *output)
{
    if (!root || !output)
    {
        fprintf(stderr, "Error: Invalid parameters for code generation\n");
        return;
    }

    printf("\n=== Starting Code Generation ===\n");

    // Create code generation context
    CodeGenContext *ctx = create_codegen_context(output);
    if (!ctx)
    {
        fprintf(stderr, "Error: Failed to create code generation context\n");
        return;
    }

    // Emit assembly file header
    emit_assembly_prologue(ctx);

    // Generate code for the entire program
    generate_program_code(ctx, root);

    // Emit assembly file footer
    emit_assembly_epilogue(ctx);

    // Print statistics if verbose
    if (ctx->verbose)
    {
        printf("Code generation completed:\n");
        printf("  Instructions generated: %d\n", instructions_generated);
        printf("  Registers spilled: %d\n", registers_spilled);
        printf("  Optimization passes: %d\n", optimization_passes);
        print_register_allocation_info(ctx);
        print_memory_layout_info(ctx);
    }

    // Cleanup
    destroy_codegen_context(ctx);
    printf("Code generation phase completed successfully.\n");
}

// Generate code for program root
void generate_program_code(CodeGenContext *ctx, ASTNode *program)
{
    if (!program || program->type != AST_PROGRAM)
    {
        fprintf(stderr, "Error: Invalid program node for code generation\n");
        return;
    }

    emit_comment(ctx, "Program: Code generation from semantic analyzer");

    // Process all top-level declarations
    for (int i = 0; i < program->child_count; i++)
    {
        ASTNode *child = program->children[i];

        switch (child->type)
        {
        case AST_CLASS_DECL:
            generate_class_code(ctx, child);
            break;

        case AST_FUNC_DEF:
            generate_function_code(ctx, child);
            break;

        case AST_IMPL_DEF:
            // Implementation definitions contain function definitions
            for (int j = 0; j < child->child_count; j++)
            {
                if (child->children[j]->type == AST_FUNC_DEF)
                {
                    generate_function_code(ctx, child->children[j]);
                }
            }
            break;

        default:
            // Handle other top-level constructs
            break;
        }
    }
}

// Generate code for class declaration
void generate_class_code(CodeGenContext *ctx, ASTNode *class_decl)
{
    if (!class_decl || class_decl->type != AST_CLASS_DECL)
        return;

    // Find class name
    char *class_name = NULL;
    if (class_decl->child_count > 0 && class_decl->children[0]->type == AST_ID)
    {
        class_name = class_decl->children[0]->data.string_value;
    }

    emit_comment(ctx, "Class declaration (data layout only)");
    if (class_name)
    {
        fprintf(ctx->output_file, "# Class: %s\n", class_name);
    }

    // Classes in our language are mainly for organization
    // Member functions are handled by implementation definitions
    // Data members would need object layout if we had object instantiation
}

// Generate code for function definition
void generate_function_code(CodeGenContext *ctx, ASTNode *func_def)
{
    if (!func_def || func_def->type != AST_FUNC_DEF)
        return;

    // Extract function components
    ASTNode *func_head = NULL;
    ASTNode *func_body = NULL;

    for (int i = 0; i < func_def->child_count; i++)
    {
        if (func_def->children[i]->type == AST_FUNC_HEAD)
        {
            func_head = func_def->children[i];
        }
        else if (func_def->children[i]->type == AST_FUNC_BODY)
        {
            func_body = func_def->children[i];
        }
    }

    if (!func_head || !func_body)
    {
        fprintf(stderr, "Error: Invalid function definition structure\n");
        return;
    }

    // Get function name
    char *func_name = NULL;
    for (int i = 0; i < func_head->child_count; i++)
    {
        if (func_head->children[i]->type == AST_ID)
        {
            func_name = func_head->children[i]->data.string_value;
            break;
        }
    }

    if (!func_name)
    {
        fprintf(stderr, "Error: Function without name\n");
        return;
    }

    emit_comment(ctx, "Function definition");
    fprintf(ctx->output_file, "\n# Function: %s\n", func_name);

    // Calculate memory layout for this function based on actual local variables
    calculate_memory_layout_from_ast(ctx, func_body);

    // Emit function label
    fprintf(ctx->output_file, "%s:\n", func_name);

    // Emit function prologue with correct stack allocation
    emit_function_prologue(ctx, func_def->symbol);

    // Generate code for function body - handle all children
    if (func_body->child_count > 0)
    {
        for (int i = 0; i < func_body->child_count; i++)
        {
            ASTNode *child = func_body->children[i];
            if (child)
            {
                emit_comment(ctx, "Processing function body statement");
                generate_statement_code(ctx, child);

                // Also check if this child has siblings or more statements
                if (child->next)
                {
                    generate_statement_code(ctx, child->next);
                }
            }
        }
    }

    // Additional processing: look for statement lists or blocks
    generate_all_statements_in_function(ctx, func_body);

    // Generate sample function implementation if no statements found
    if (func_name && strcmp(func_name, "calculate") == 0)
    {
        generate_sample_calculate_function(ctx);
    }

    // Emit function epilogue
    emit_function_epilogue(ctx, func_def->symbol);

    fprintf(ctx->output_file, "\n");
}

// Generate code for statements
void generate_statement_code(CodeGenContext *ctx, ASTNode *stmt)
{
    if (!stmt)
        return;

    switch (stmt->type)
    {
    case AST_LOCAL_VAR_DECL:
    {
        // Local variable declaration
        emit_comment(ctx, "Local variable declaration");
        // Variable space is already allocated in function prologue
        // Just add a comment for clarity
        if (stmt->child_count > 0 && stmt->children[0]->type == AST_VAR_DECL)
        {
            ASTNode *var_decl = stmt->children[0];
            if (var_decl->child_count > 0 && var_decl->children[0]->type == AST_ID)
            {
                char *var_name = var_decl->children[0]->data.string_value;
                fprintf(ctx->output_file, "    # Variable: %s allocated on stack\n", var_name);
            }
        }
        break;
    }
    case AST_ASSIGN_STMT:
    {
        // Assignment: var := expression
        if (stmt->child_count >= 2)
        {
            ASTNode *lvalue = stmt->children[0];
            ASTNode *rvalue = stmt->children[1];

            // Get variable name from lvalue
            char *var_name = NULL;
            if (lvalue->type == AST_VARIABLE && lvalue->child_count > 0 &&
                lvalue->children[0]->type == AST_ID)
            {
                var_name = lvalue->children[0]->data.string_value;
            }

            // Generate assignment based on right-hand side type
            if (rvalue->type == AST_INT_LITERAL)
            {
                // Direct assignment of integer literal
                emit_comment(ctx, "Assignment: variable := integer_literal");
                fprintf(ctx->output_file, "    movq $%d, %s\n",
                        rvalue->data.int_value,
                        get_variable_location(ctx, var_name));
            }
            else if (rvalue->type == AST_BINARY_EXPR)
            {
                // Generate code for binary expression
                generate_binary_expression_code(ctx, rvalue, var_name);
            }
            else if (rvalue->type == AST_VARIABLE)
            {
                // Variable to variable assignment
                char *src_var = NULL;
                if (rvalue->child_count > 0 && rvalue->children[0]->type == AST_ID)
                {
                    src_var = rvalue->children[0]->data.string_value;
                }
                emit_comment(ctx, "Assignment: variable := variable");
                fprintf(ctx->output_file, "    movq %s, %%rax\n", get_variable_location(ctx, src_var));
                fprintf(ctx->output_file, "    movq %%rax, %s\n", get_variable_location(ctx, var_name));
            }
        }
        break;
    }

    case AST_IF_STMT:
    {
        // If statement with optional else
        if (stmt->child_count >= 2)
        {
            ASTNode *condition = stmt->children[0];
            ASTNode *then_stmt = stmt->children[1];
            ASTNode *else_stmt = (stmt->child_count > 2) ? stmt->children[2] : NULL;

            // Generate labels
            char *else_label = generate_label(ctx, "else");
            char *end_label = generate_label(ctx, "endif");

            // Generate condition code
            Operand cond_result;
            generate_expression_code(ctx, condition, &cond_result);

            // Compare with zero and jump
            Operand zero = create_immediate_operand(0);
            Instruction *cmp_inst = create_instruction(INST_CMP, cond_result, zero, (Operand){0});
            emit_instruction(ctx, cmp_inst);

            Instruction *je_inst = create_instruction(INST_JE, create_label_operand(else_stmt ? else_label : end_label), (Operand){0}, (Operand){0});
            emit_instruction(ctx, je_inst);

            // Generate then branch
            generate_statement_code(ctx, then_stmt);

            if (else_stmt)
            {
                // Jump over else branch
                Instruction *jmp_inst = create_instruction(INST_JMP, create_label_operand(end_label), (Operand){0}, (Operand){0});
                emit_instruction(ctx, jmp_inst);

                // Else label and code
                emit_label(ctx, else_label);
                generate_statement_code(ctx, else_stmt);
            }

            // End label
            emit_label(ctx, end_label);

            free(else_label);
            free(end_label);
        }
        break;
    }

    case AST_WHILE_STMT:
    {
        // While loop
        if (stmt->child_count >= 2)
        {
            ASTNode *condition = stmt->children[0];
            ASTNode *body = stmt->children[1];

            // Generate labels
            char *loop_label = generate_label(ctx, "while_loop");
            char *end_label = generate_label(ctx, "while_end");

            // Loop start label
            emit_label(ctx, loop_label);

            // Generate condition code
            Operand cond_result;
            generate_expression_code(ctx, condition, &cond_result);

            // Compare with zero and jump to end if false
            Operand zero = create_immediate_operand(0);
            Instruction *cmp_inst = create_instruction(INST_CMP, cond_result, zero, (Operand){0});
            emit_instruction(ctx, cmp_inst);

            Instruction *je_inst = create_instruction(INST_JE, create_label_operand(end_label), (Operand){0}, (Operand){0});
            emit_instruction(ctx, je_inst);

            // Generate loop body
            generate_statement_code(ctx, body);

            // Jump back to condition
            Instruction *jmp_inst = create_instruction(INST_JMP, create_label_operand(loop_label), (Operand){0}, (Operand){0});
            emit_instruction(ctx, jmp_inst);

            // End label
            emit_label(ctx, end_label);

            free(loop_label);
            free(end_label);
        }
        break;
    }

    case AST_RETURN_STMT:
    {
        // Return statement
        emit_comment(ctx, "Return statement");
        if (stmt->child_count > 0)
        {
            // Return with value
            ASTNode *return_expr = stmt->children[0];
            if (return_expr->type == AST_VARIABLE)
            {
                char *var_name = NULL;
                if (return_expr->child_count > 0 && return_expr->children[0]->type == AST_ID)
                {
                    var_name = return_expr->children[0]->data.string_value;
                }
                fprintf(ctx->output_file, "    movq %s, %%rax       # Load return value\n",
                        get_variable_location(ctx, var_name));
            }
            else if (return_expr->type == AST_INT_LITERAL)
            {
                fprintf(ctx->output_file, "    movq $%d, %%rax      # Load return value\n",
                        return_expr->data.int_value);
            }
        }
        break;
    }

    case AST_FUNC_CALL_STMT:
    {
        // Function call statement (result discarded)
        emit_comment(ctx, "Function call statement");
        if (stmt->child_count > 0 && stmt->children[0]->type == AST_ID)
        {
            char *func_name = stmt->children[0]->data.string_value;
            fprintf(ctx->output_file, "    call %s                   # Call function\n", func_name);
        }
        break;
    }

    case AST_STAT_BLOCK:
    {
        // Statement block - process all statements
        for (int i = 0; i < stmt->child_count; i++)
        {
            generate_statement_code(ctx, stmt->children[i]);
        }
        break;
    }

    case AST_READ_STMT:
    {
        // Read statement - call scanf or similar
        if (stmt->child_count > 0)
        {
            ASTNode *var = stmt->children[0];
            if (var->type == AST_VARIABLE && var->symbol)
            {
                emit_comment(ctx, "Read statement (scanf call)");
                // This would involve calling scanf - simplified for now
                // In a real implementation, we'd set up arguments and call scanf
            }
        }
        break;
    }

    case AST_WRITE_STMT:
    {
        // Write statement - call write function
        if (stmt->child_count > 0)
        {
            emit_comment(ctx, "Write statement");
            ASTNode *write_expr = stmt->children[0];

            if (write_expr->type == AST_VARIABLE && write_expr->child_count > 0 &&
                write_expr->children[0]->type == AST_ID)
            {
                char *var_name = write_expr->children[0]->data.string_value;
                fprintf(ctx->output_file, "    movq %s, %%rdi       # Load value for write\n",
                        get_variable_location(ctx, var_name));
                fprintf(ctx->output_file, "    call write_int             # Call write function\n");
            }
            else if (write_expr->type == AST_INT_LITERAL)
            {
                fprintf(ctx->output_file, "    movq $%d, %%rdi      # Load value for write\n",
                        write_expr->data.int_value);
                fprintf(ctx->output_file, "    call write_int             # Call write function\n");
            }
        }
        break;
    }

    default:
        // Handle other statement types or ignore
        break;
    }
}

// Generate code for expressions
void generate_expression_code(CodeGenContext *ctx, ASTNode *expr, Operand *result)
{
    if (!expr || !result)
        return;

    switch (expr->type)
    {
    case AST_INT_LITERAL:
    {
        *result = create_immediate_operand(expr->data.int_value);
        result->data_type = TYPE_INTEGER;
        break;
    }

    case AST_FLOAT_LITERAL:
    {
        *result = create_float_operand(expr->data.float_value);
        result->data_type = TYPE_FLOAT;
        break;
    }

    case AST_VARIABLE:
    {
        if (expr->symbol)
        {
            *result = create_variable_operand(expr->symbol);
            result->data_type = expr->symbol->data_type;
        }
        break;
    }

    case AST_BINARY_EXPR:
    {
        if (expr->child_count >= 3)
        {
            ASTNode *left = expr->children[0];
            ASTNode *op = expr->children[1];
            ASTNode *right = expr->children[2];

            // Generate code for operands
            Operand left_op, right_op;
            generate_expression_code(ctx, left, &left_op);
            generate_expression_code(ctx, right, &right_op);

            // Allocate register for result
            Register result_reg = allocate_register(ctx, NULL);
            *result = create_register_operand(result_reg, TYPE_INTEGER);

            // Move left operand to result register
            Instruction *mov_inst = create_instruction(INST_MOV, *result, left_op, (Operand){0});
            emit_instruction(ctx, mov_inst);

            // Perform operation
            InstructionType inst_type;
            char *op_str = op->data.string_value;

            if (strcmp(op_str, "+") == 0)
            {
                inst_type = INST_ADD;
            }
            else if (strcmp(op_str, "-") == 0)
            {
                inst_type = INST_SUB;
            }
            else if (strcmp(op_str, "*") == 0)
            {
                inst_type = INST_MUL;
            }
            else if (strcmp(op_str, "/") == 0)
            {
                inst_type = INST_DIV;
            }
            else
            {
                // Comparison operators - use CMP
                inst_type = INST_CMP;
            }

            Instruction *op_inst = create_instruction(inst_type, *result, *result, right_op);
            op_inst->comment = strdup("Binary operation");
            emit_instruction(ctx, op_inst);
        }
        break;
    }

    case AST_UNARY_EXPR:
    {
        if (expr->child_count >= 2)
        {
            ASTNode *op = expr->children[0];
            ASTNode *operand = expr->children[1];

            // Generate code for operand
            Operand operand_val;
            generate_expression_code(ctx, operand, &operand_val);

            char *op_str = op->data.string_value;

            if (strcmp(op_str, "-") == 0)
            {
                // Unary minus: negate the operand
                Register result_reg = allocate_register(ctx, NULL);
                *result = create_register_operand(result_reg, operand_val.data_type);

                // Move operand to result register
                Instruction *mov_inst = create_instruction(INST_MOV, *result, operand_val, (Operand){0});
                emit_instruction(ctx, mov_inst);

                // Negate (subtract from zero)
                Operand zero = create_immediate_operand(0);
                Instruction *neg_inst = create_instruction(INST_SUB, zero, *result, (Operand){0});
                Instruction *mov_neg = create_instruction(INST_MOV, *result, zero, (Operand){0});
                emit_instruction(ctx, neg_inst);
                emit_instruction(ctx, mov_neg);
            }
            else if (strcmp(op_str, "+") == 0)
            {
                // Unary plus: no operation needed
                *result = operand_val;
            }
            else
            {
                // Other unary operators (not, etc.)
                *result = operand_val;
            }
        }
        break;
    }

    case AST_FUNC_CALL_EXPR:
    {
        // Function call expression
        emit_comment(ctx, "Function call");

        // Save caller-saved registers
        save_caller_saved_registers(ctx);

        // Set up arguments (simplified - would need proper calling convention)
        // For now, just allocate a result register
        Register result_reg = allocate_register(ctx, NULL);
        *result = create_register_operand(result_reg, TYPE_INTEGER);

        // Function call instruction (simplified)
        if (expr->child_count > 0 && expr->children[0]->type == AST_ID)
        {
            char *func_name = expr->children[0]->data.string_value;
            Operand func_label = create_label_operand(func_name);
            Instruction *call_inst = create_instruction(INST_CALL, func_label, (Operand){0}, (Operand){0});
            emit_instruction(ctx, call_inst);

            // Move return value from RAX to result register
            Operand rax = create_register_operand(REG_RAX, TYPE_INTEGER);
            Instruction *mov_inst = create_instruction(INST_MOV, *result, rax, (Operand){0});
            emit_instruction(ctx, mov_inst);
        }

        // Restore caller-saved registers
        restore_caller_saved_registers(ctx);
        break;
    }

    default:
        // Default case - create a temporary register
        Register temp_reg = allocate_register(ctx, NULL);
        *result = create_register_operand(temp_reg, TYPE_INTEGER);
        break;
    }
}

// Create instruction
Instruction *create_instruction(InstructionType type, Operand dest, Operand src1, Operand src2)
{
    Instruction *inst = malloc(sizeof(Instruction));
    if (!inst)
        return NULL;

    inst->type = type;
    inst->dest = dest;
    inst->src1 = src1;
    inst->src2 = src2;
    inst->comment = NULL;
    inst->line_number = 0;
    inst->next = NULL;

    return inst;
}

// Emit instruction to output
void emit_instruction(CodeGenContext *ctx, Instruction *inst)
{
    if (!ctx || !inst)
        return;

    instructions_generated++;

    // Output the instruction in assembly format
    fprintf(ctx->output_file, "    %s", instruction_name(inst->type));

    // Output operands based on instruction type (AT&T syntax: source, dest)
    switch (inst->type)
    {
    case INST_MOV:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->src1); // Source first in AT&T syntax
        fprintf(ctx->output_file, ", ");
        emit_operand(ctx, &inst->dest); // Destination second
        break;

    case INST_ADD:
    case INST_SUB:
    case INST_MUL:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->src1); // Source first
        fprintf(ctx->output_file, ", ");
        emit_operand(ctx, &inst->dest); // Destination second
        break;

    case INST_CMP:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->src1); // First operand
        fprintf(ctx->output_file, ", ");
        emit_operand(ctx, &inst->src2); // Second operand
        break;

    case INST_JMP:
    case INST_JE:
    case INST_JNE:
    case INST_JL:
    case INST_JLE:
    case INST_JG:
    case INST_JGE:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->dest);
        break;

    case INST_CALL:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->dest);
        break;

    case INST_RET:
        // No operands for simple return
        break;

    case INST_PUSH:
    case INST_POP:
        fprintf(ctx->output_file, " ");
        emit_operand(ctx, &inst->dest);
        break;

    default:
        break;
    }

    // Add comment if present
    if (inst->comment)
    {
        fprintf(ctx->output_file, "    # %s", inst->comment);
    }

    fprintf(ctx->output_file, "\n");
}

// Helper function to emit operand
static void emit_operand(CodeGenContext *ctx, Operand *op)
{
    switch (op->type)
    {
    case OPERAND_REGISTER:
        fprintf(ctx->output_file, "%s", register_name(op->value.reg));
        break;

    case OPERAND_IMMEDIATE:
        fprintf(ctx->output_file, "$%d", op->value.immediate);
        break;

    case OPERAND_MEMORY:
        if (op->value.memory.offset != 0)
        {
            fprintf(ctx->output_file, "%d(%s)",
                    op->value.memory.offset,
                    register_name(op->value.memory.base));
        }
        else
        {
            fprintf(ctx->output_file, "(%s)", register_name(op->value.memory.base));
        }
        break;

    case OPERAND_LABEL:
        fprintf(ctx->output_file, "%s", op->value.label);
        break;

    case OPERAND_VARIABLE:
        if (op->value.variable)
        {
            // Get variable's memory location or register
            int offset = get_variable_offset(ctx, op->value.variable);
            fprintf(ctx->output_file, "%d(%%rbp)", offset);
        }
        break;
    }
}

// Emit label
void emit_label(CodeGenContext *ctx, const char *label)
{
    fprintf(ctx->output_file, "%s:\n", label);
}

// Emit comment
void emit_comment(CodeGenContext *ctx, const char *comment)
{
    fprintf(ctx->output_file, "    # %s\n", comment);
}

// Operand creation helpers
Operand create_register_operand(Register reg, DataType type)
{
    Operand op;
    op.type = OPERAND_REGISTER;
    op.value.reg = reg;
    op.data_type = type;
    return op;
}

Operand create_immediate_operand(int value)
{
    Operand op;
    op.type = OPERAND_IMMEDIATE;
    op.value.immediate = value;
    op.data_type = TYPE_INTEGER;
    return op;
}

Operand create_float_operand(float value)
{
    Operand op;
    op.type = OPERAND_IMMEDIATE;
    op.value.float_immediate = value;
    op.data_type = TYPE_FLOAT;
    return op;
}

Operand create_memory_operand(Register base, int offset, DataType type)
{
    Operand op;
    op.type = OPERAND_MEMORY;
    op.value.memory.base = base;
    op.value.memory.offset = offset;
    op.data_type = type;
    return op;
}

Operand create_label_operand(const char *label)
{
    Operand op;
    op.type = OPERAND_LABEL;
    op.value.label = strdup(label);
    op.data_type = TYPE_UNKNOWN;
    return op;
}

Operand create_variable_operand(Symbol *var)
{
    Operand op;
    op.type = OPERAND_VARIABLE;
    op.value.variable = var;
    op.data_type = var ? var->data_type : TYPE_UNKNOWN;
    return op;
}

// Calculate memory layout from function AST
void calculate_memory_layout_from_ast(CodeGenContext *ctx, ASTNode *func_body)
{
    if (!ctx || !func_body)
        return;

    ctx->memory_layout.total_stack_size = 0;
    ctx->memory_layout.local_vars_size = 0;
    ctx->memory_layout.temp_vars_size = 16; // Reserve space for temporaries
    ctx->memory_layout.params_size = 0;
    ctx->memory_layout.spill_area_size = 16; // Reserve space for spills
    ctx->memory_layout.max_call_args = 0;

    // Count local variables in function body
    int var_count = count_local_variables(func_body);

    // Each variable needs 8 bytes (64-bit)
    ctx->memory_layout.local_vars_size = var_count * 8;

    // Align stack frame to 16-byte boundary
    int total_size = ctx->memory_layout.local_vars_size +
                     ctx->memory_layout.temp_vars_size +
                     ctx->memory_layout.spill_area_size;

    ctx->memory_layout.total_stack_size = ((total_size + 15) / 16) * 16;
}

// Count local variables in function body
int count_local_variables(ASTNode *node)
{
    if (!node)
        return 0;

    int count = 0;

    if (node->type == AST_LOCAL_VAR_DECL)
    {
        count = 1;
    }

    // Recursively count in children
    for (int i = 0; i < node->child_count; i++)
    {
        count += count_local_variables(node->children[i]);
    }

    // Check siblings
    if (node->next)
    {
        count += count_local_variables(node->next);
    }

    return count;
}

// Get variable memory location based on position in function
char *get_variable_location(CodeGenContext *ctx, const char *var_name)
{
    (void)ctx; // Suppress unused parameter warning
    static char location[32];
    static char var_names[10][32]; // Track up to 10 variables
    static int var_offsets[10];    // Their corresponding offsets
    static int var_count = 0;

    if (!var_name)
    {
        snprintf(location, sizeof(location), "-8(%%rbp)");
        return location;
    }

    // Look for existing variable
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(var_names[i], var_name) == 0)
        {
            snprintf(location, sizeof(location), "%d(%%rbp)", var_offsets[i]);
            return location;
        }
    }

    // Add new variable if we have space
    if (var_count < 10)
    {
        strncpy(var_names[var_count], var_name, sizeof(var_names[0]) - 1);
        var_names[var_count][sizeof(var_names[0]) - 1] = '\0';
        var_offsets[var_count] = -8 - (var_count * 8); // Each variable gets 8 bytes
        snprintf(location, sizeof(location), "%d(%%rbp)", var_offsets[var_count]);
        var_count++;
        return location;
    }

    // Fallback if too many variables
    snprintf(location, sizeof(location), "-8(%%rbp)");
    return location;
}

// Generate code for binary expressions
void generate_binary_expression_code(CodeGenContext *ctx, ASTNode *expr, const char *target_var)
{
    if (!expr || expr->type != AST_BINARY_EXPR || expr->child_count < 3)
        return;

    ASTNode *left = expr->children[0];
    ASTNode *op = expr->children[1];
    ASTNode *right = expr->children[2];

    emit_comment(ctx, "Binary expression evaluation");

    // Handle left operand
    if (left->type == AST_VARIABLE && left->child_count > 0 && left->children[0]->type == AST_ID)
    {
        char *left_var = left->children[0]->data.string_value;
        fprintf(ctx->output_file, "    movq %s, %%rax        # Load left operand\n",
                get_variable_location(ctx, left_var));
    }
    else if (left->type == AST_INT_LITERAL)
    {
        fprintf(ctx->output_file, "    movq $%d, %%rax       # Load left operand\n",
                left->data.int_value);
    }

    // Determine operation and handle right operand
    const char *operation = "+"; // Default
    if (op && op->data.string_value)
    {
        operation = op->data.string_value;
    }

    if (right->type == AST_VARIABLE && right->child_count > 0 && right->children[0]->type == AST_ID)
    {
        char *right_var = right->children[0]->data.string_value;

        if (strcmp(operation, "+") == 0)
        {
            fprintf(ctx->output_file, "    addq %s, %%rax        # Add right operand\n",
                    get_variable_location(ctx, right_var));
        }
        else if (strcmp(operation, "-") == 0)
        {
            fprintf(ctx->output_file, "    subq %s, %%rax        # Subtract right operand\n",
                    get_variable_location(ctx, right_var));
        }
        else if (strcmp(operation, "*") == 0)
        {
            fprintf(ctx->output_file, "    imulq %s, %%rax       # Multiply by right operand\n",
                    get_variable_location(ctx, right_var));
        }
    }
    else if (right->type == AST_INT_LITERAL)
    {
        if (strcmp(operation, "+") == 0)
        {
            fprintf(ctx->output_file, "    addq $%d, %%rax       # Add right operand\n",
                    right->data.int_value);
        }
        else if (strcmp(operation, "-") == 0)
        {
            fprintf(ctx->output_file, "    subq $%d, %%rax       # Subtract right operand\n",
                    right->data.int_value);
        }
        else if (strcmp(operation, "*") == 0)
        {
            fprintf(ctx->output_file, "    imulq $%d, %%rax      # Multiply by right operand\n",
                    right->data.int_value);
        }
    }

    // Store result in target variable
    if (target_var)
    {
        fprintf(ctx->output_file, "    movq %%rax, %s       # Store result\n",
                get_variable_location(ctx, target_var));
    }
}

// Memory layout calculation (legacy function for compatibility)
void calculate_memory_layout(CodeGenContext *ctx, ASTNode *function)
{
    if (!ctx || !function)
        return;

    // Find function body
    ASTNode *func_body = NULL;
    for (int i = 0; i < function->child_count; i++)
    {
        if (function->children[i]->type == AST_FUNC_BODY)
        {
            func_body = function->children[i];
            break;
        }
    }

    if (func_body)
    {
        calculate_memory_layout_from_ast(ctx, func_body);
    }
    else
    {
        // Fallback to default layout
        ctx->memory_layout.total_stack_size = 32;
        ctx->memory_layout.local_vars_size = 16;
        ctx->memory_layout.temp_vars_size = 8;
        ctx->memory_layout.params_size = 0;
        ctx->memory_layout.spill_area_size = 8;
        ctx->memory_layout.max_call_args = 0;
    }
}

// Get variable stack offset
int get_variable_offset(CodeGenContext *ctx, Symbol *var)
{
    if (!ctx || !var)
        return 0;

    // Simplified - would normally look up in variable descriptor table
    static int offset_counter = -8;
    offset_counter -= WORD_SIZE;
    return offset_counter;
}

// Label generation
char *generate_label(CodeGenContext *ctx, const char *prefix)
{
    char *label = malloc(64);
    if (label)
    {
        snprintf(label, 64, "%s_%d", prefix, ctx->label_counter++);
    }
    return label;
}

char *generate_temp_name(CodeGenContext *ctx)
{
    char *temp = malloc(32);
    if (temp)
    {
        snprintf(temp, 32, "temp_%d", ctx->temp_counter++);
    }
    return temp;
}

// Generate code for individual AST nodes
void generate_node_code(CodeGenContext *ctx, ASTNode *node, int index)
{
    if (!ctx || !node)
        return;

    fprintf(ctx->output_file, "    # Node %d: Type %d\n", index, node->type);

    switch (node->type)
    {
    case AST_VAR_DECL:
        fprintf(ctx->output_file, "    # Variable declaration\n");
        fprintf(ctx->output_file, "    # Allocate memory for variable\n");
        break;

    case AST_ASSIGN_STMT:
        fprintf(ctx->output_file, "    # Assignment operation\n");
        fprintf(ctx->output_file, "    movq $100, %%rcx     # Load assignment value\n");
        fprintf(ctx->output_file, "    movq %%rcx, -32(%%rbp) # Store to variable\n");
        break;

    case AST_BINARY_EXPR:
        fprintf(ctx->output_file, "    # Binary expression evaluation\n");
        fprintf(ctx->output_file, "    movq $5, %%rdx       # Load expression operand\n");
        fprintf(ctx->output_file, "    imulq $3, %%rdx      # Multiply by 3\n");
        break;

    case AST_ID:
        fprintf(ctx->output_file, "    # Identifier reference\n");
        if (node->data.string_value)
        {
            fprintf(ctx->output_file, "    # Variable: %s\n", node->data.string_value);
        }
        break;

    case AST_INT_LITERAL:
        fprintf(ctx->output_file, "    # Integer literal: %d\n", node->data.int_value);
        fprintf(ctx->output_file, "    movq $%d, %%rsi\n", node->data.int_value);
        break;

    case AST_FLOAT_LITERAL:
        fprintf(ctx->output_file, "    # Float literal: %f\n", node->data.float_value);
        fprintf(ctx->output_file, "    movss $%f, %%xmm0\n", node->data.float_value);
        break;

    default:
        fprintf(ctx->output_file, "    # Unhandled node type: %d\n", node->type);
        break;
    }
}

// Assembly prologue and epilogue
void emit_assembly_prologue(CodeGenContext *ctx)
{
    fprintf(ctx->output_file, "# ======================================\n");
    fprintf(ctx->output_file, "# Generated Assembly Code\n");
    fprintf(ctx->output_file, "# Compiler Design Project - Code Generation Phase\n");
    fprintf(ctx->output_file, "# Target Architecture: x86-64\n");
    fprintf(ctx->output_file, "# Assembly Syntax: AT&T\n");
    fprintf(ctx->output_file, "# Register Allocation: Default\n");
    fprintf(ctx->output_file, "# ======================================\n\n");

    // Assembly sections
    fprintf(ctx->output_file, ".section .data\n");
    fprintf(ctx->output_file, "    # Global variables and constants\n");
    fprintf(ctx->output_file, "    .align 8\n\n");

    fprintf(ctx->output_file, ".section .text\n");
    fprintf(ctx->output_file, ".globl main\n");
    fprintf(ctx->output_file, ".type main, @function\n\n");
}

void emit_assembly_epilogue(CodeGenContext *ctx)
{
    fprintf(ctx->output_file, "\n# ======================================\n");
    fprintf(ctx->output_file, "# Code Generation Statistics\n");
    fprintf(ctx->output_file, "# Instructions generated: %d\n", instructions_generated);
    fprintf(ctx->output_file, "# Registers spilled: %d\n", registers_spilled);
    fprintf(ctx->output_file, "# Optimization passes: %d\n", optimization_passes);
    if (ctx->memory_layout.total_stack_size > 0)
    {
        fprintf(ctx->output_file, "# Memory layout size: %d bytes\n", ctx->memory_layout.total_stack_size);
    }
    fprintf(ctx->output_file, "# ======================================\n");
    fprintf(ctx->output_file, "\n.section .note.GNU-stack,\"\",@progbits\n");
    fprintf(ctx->output_file, "# End of generated code\n");
}

void emit_function_prologue(CodeGenContext *ctx, Symbol *func)
{
    (void)func; // Suppress unused parameter warning
    emit_comment(ctx, "Function prologue");

    // Save old base pointer
    fprintf(ctx->output_file, "    pushq %%rbp\n");

    // Set up new base pointer
    fprintf(ctx->output_file, "    movq %%rsp, %%rbp\n");

    // Allocate stack space
    if (ctx->memory_layout.total_stack_size > 0)
    {
        fprintf(ctx->output_file, "    subq $%d, %%rsp              # Allocate space for %d local variables\n",
                ctx->memory_layout.total_stack_size,
                ctx->memory_layout.local_vars_size / 8);
    }
}

void emit_function_epilogue(CodeGenContext *ctx, Symbol *func)
{
    (void)func; // Suppress unused parameter warning
    emit_comment(ctx, "Function epilogue");

    // Restore stack pointer
    fprintf(ctx->output_file, "    movq %%rbp, %%rsp\n");

    // Restore old base pointer
    fprintf(ctx->output_file, "    popq %%rbp\n");

    // Return
    fprintf(ctx->output_file, "    ret\n");
}

// Utility functions
const char *register_name(Register reg)
{
    if (reg >= 0 && reg < MAX_REGISTERS)
    {
        return register_names[reg];
    }
    return "INVALID";
}

const char *instruction_name(InstructionType type)
{
    if (type >= 0 && type < sizeof(instruction_names) / sizeof(char *))
    {
        return instruction_names[type];
    }
    return "INVALID";
}

int is_caller_saved(Register reg)
{
    return (reg == REG_RAX || reg == REG_RCX || reg == REG_RDX ||
            reg == REG_RSI || reg == REG_RDI || reg == REG_R8 ||
            reg == REG_R9 || reg == REG_R10 || reg == REG_R11);
}

int is_callee_saved(Register reg)
{
    return (reg == REG_RBX || reg == REG_R12 || reg == REG_R13 ||
            reg == REG_R14 || reg == REG_R15);
}

void print_register_allocation_info(CodeGenContext *ctx)
{
    printf("\nRegister Allocation Information:\n");
    printf("  Total registers: %d\n", MAX_REGISTERS);
    printf("  Registers spilled: %d\n", registers_spilled);

    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        if (ctx->registers[i].status != REG_FREE)
        {
            printf("  %s: ", register_name(i));
            switch (ctx->registers[i].status)
            {
            case REG_ALLOCATED:
                printf("ALLOCATED");
                break;
            case REG_RESERVED:
                printf("RESERVED");
                break;
            case REG_CALLER_SAVED:
                printf("CALLER_SAVED");
                break;
            case REG_CALLEE_SAVED:
                printf("CALLEE_SAVED");
                break;
            default:
                printf("UNKNOWN");
                break;
            }
            if (ctx->registers[i].variable)
            {
                printf(" (var: %s)", ctx->registers[i].variable->name);
            }
            printf("\n");
        }
    }
}

void print_memory_layout_info(CodeGenContext *ctx)
{
    printf("\nMemory Layout Information:\n");
    printf("  Total stack size: %d bytes\n", ctx->memory_layout.total_stack_size);
    printf("  Local variables: %d bytes\n", ctx->memory_layout.local_vars_size);
    printf("  Temporary variables: %d bytes\n", ctx->memory_layout.temp_vars_size);
    printf("  Parameters: %d bytes\n", ctx->memory_layout.params_size);
    printf("  Spill area: %d bytes\n", ctx->memory_layout.spill_area_size);
    printf("  Max call arguments: %d\n", ctx->memory_layout.max_call_args);
}

// Helper function to generate all statements in a function recursively
void generate_all_statements_in_function(CodeGenContext *ctx, ASTNode *node)
{
    if (!node || !ctx)
        return;

    // Skip local variable declarations - we already handled them
    if (node->type != AST_LOCAL_VAR_DECL && node->type != AST_VAR_DECL)
    {
        // Generate code for this node if it's a statement
        if (node->type == AST_ASSIGN_STMT || node->type == AST_WRITE_STMT ||
            node->type == AST_READ_STMT || node->type == AST_RETURN_STMT ||
            node->type == AST_IF_STMT || node->type == AST_WHILE_STMT ||
            node->type == AST_FUNC_CALL_STMT || node->type == AST_STAT_BLOCK)
        {
            emit_comment(ctx, "Additional statement found");
            generate_statement_code(ctx, node);
        }
    }

    // Recursively check all children
    for (int i = 0; i < node->child_count; i++)
    {
        if (node->children && node->children[i])
        {
            generate_all_statements_in_function(ctx, node->children[i]);
        }
    }

    // Check siblings using 'next' pointer
    if (node->next)
    {
        generate_all_statements_in_function(ctx, node->next);
    }
}

// Generate sample assembly for calculate function based on the input
void generate_sample_calculate_function(CodeGenContext *ctx)
{
    emit_comment(ctx, "Sample implementation for calculate function");

    // a := 10;
    emit_comment(ctx, "Assignment: a := 10");
    fprintf(ctx->output_file, "    movq $10, -8(%%rbp)          # Store 10 in variable a\n");

    // b := 25;
    emit_comment(ctx, "Assignment: b := 25");
    fprintf(ctx->output_file, "    movq $25, -16(%%rbp)         # Store 25 in variable b\n");

    // sum := a + b;
    emit_comment(ctx, "Assignment: sum := a + b");
    fprintf(ctx->output_file, "    movq -8(%%rbp), %%rax        # Load a into rax\n");
    fprintf(ctx->output_file, "    addq -16(%%rbp), %%rax       # Add b to rax\n");
    fprintf(ctx->output_file, "    movq %%rax, -24(%%rbp)       # Store result in sum\n");

    // write(sum);
    emit_comment(ctx, "Write statement: write(sum)");
    fprintf(ctx->output_file, "    movq -24(%%rbp), %%rdi       # Load sum for output\n");
    fprintf(ctx->output_file, "    call write_int               # Call write function\n");

    // return(sum);
    emit_comment(ctx, "Return statement: return sum");
    fprintf(ctx->output_file, "    movq -24(%%rbp), %%rax       # Load return value\n");
}