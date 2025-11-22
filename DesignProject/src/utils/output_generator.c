#include "../../include/utils/output_generator.h"
#include "../../include/core/ast.h"
#include "../../include/utils/error_handler.h"
#include "../../include/codegen/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to print AST tree structure
void print_ast_tree(ASTNode *node, FILE *output, int depth)
{
    if (!node || !output)
        return;

    // Print indentation
    for (int i = 0; i < depth; i++)
    {
        fprintf(output, "  ");
    }

    // Print node type and data
    fprintf(output, "%s", get_ast_type_name(node->type));

    if (node->data.string_value)
    {
        fprintf(output, "(\"%s\")", node->data.string_value);
    }
    else if (node->type == AST_INT_LITERAL)
    {
        fprintf(output, "(%d)", node->data.int_value);
    }
    else if (node->type == AST_FLOAT_LITERAL)
    {
        fprintf(output, "(%.2f)", node->data.float_value);
    }

    fprintf(output, "\n");

    // Print children
    for (int i = 0; i < node->child_count; i++)
    {
        if (node->children && node->children[i])
        {
            print_ast_tree(node->children[i], output, depth + 1);
        }
    }
}

// Generate three-address code from AST
void generate_three_address_code(ASTNode *ast, FILE *output)
{
    if (!ast || !output)
        return;

    static int temp_counter = 0;

    switch (ast->type)
    {
    case AST_PROGRAM:
        fprintf(output, "// Program start\n");
        for (int i = 0; i < ast->child_count; i++)
        {
            if (ast->children && ast->children[i])
            {
                generate_three_address_code(ast->children[i], output);
            }
        }
        fprintf(output, "// Program end\n");
        break;

    case AST_VAR_DECL:
        if (ast->children && ast->children[0] && ast->children[0]->data.string_value)
        {
            fprintf(output, "ALLOC %s, %d\n",
                    ast->children[0]->data.string_value,
                    4); // Assume 4 bytes for integer
        }
        break;

    case AST_ASSIGN_STMT:
        if (ast->child_count >= 2)
        {
            fprintf(output, "t%d = ", temp_counter++);
            generate_three_address_code(ast->children[1], output);
            fprintf(output, "\n");
            if (ast->children[0]->data.string_value)
            {
                fprintf(output, "%s = t%d\n",
                        ast->children[0]->data.string_value,
                        temp_counter - 1);
            }
        }
        break;

    case AST_BINARY_EXPR:
        if (ast->child_count >= 2)
        {
            fprintf(output, "t%d = ", temp_counter);
            generate_three_address_code(ast->children[0], output);
            fprintf(output, " %s ", ast->data.operator ? ast->data.operator : "+");
            generate_three_address_code(ast->children[1], output);
            temp_counter++;
        }
        break;

    case AST_INT_LITERAL:
        fprintf(output, "%d", ast->data.int_value);
        break;

    case AST_ID:
        if (ast->data.string_value)
        {
            fprintf(output, "%s", ast->data.string_value);
        }
        break;

    case AST_FUNC_CALL_EXPR:
        if (ast->data.string_value)
        {
            fprintf(output, "CALL %s\n", ast->data.string_value);
        }
        break;

    default:
        // Handle other node types
        for (int i = 0; i < ast->child_count; i++)
        {
            if (ast->children && ast->children[i])
            {
                generate_three_address_code(ast->children[i], output);
            }
        }
        break;
    }
}

// Print register allocation information
void print_register_info_to_file(FILE *output)
{
    if (!output)
        return;

    // Get actual register allocation data from register allocator
    extern int get_registers_used(void);
    extern int get_spill_operations(void);
    extern const char *get_allocation_strategy(void);

    int registers_used = get_registers_used();
    int spill_ops = get_spill_operations();
    const char *strategy = get_allocation_strategy();

    fprintf(output, "Register Allocation Strategy: %s\n", strategy ? strategy : "Linear Scan");
    fprintf(output, "Available Registers: %d (x86-64)\n", MAX_REGISTERS);
    fprintf(output, "Registers Used: %d\n", registers_used);
    fprintf(output, "Spill Operations: %d\n", spill_ops);
    fprintf(output, "\nRegister Assignments:\n");

    if (registers_used > 0)
    {
        fprintf(output, "- %%rax: Return values, arithmetic operations\n");
        if (registers_used > 1)
            fprintf(output, "- %%rbx: Base register, persistent variables\n");
        if (registers_used > 2)
            fprintf(output, "- %%rcx: Counter, loop variables\n");
        if (registers_used > 3)
            fprintf(output, "- %%rdx: Data operations, arithmetic\n");
        if (registers_used > 4)
            fprintf(output, "- %%rsi: Source index, function arguments\n");
        if (registers_used > 5)
            fprintf(output, "- %%rdi: Destination index, function arguments\n");
        if (registers_used > 6)
            fprintf(output, "- %%r8-%%r%d: Additional variable storage\n", 7 + (registers_used - 7));
    }
    else
    {
        fprintf(output, "- No registers allocated\n");
    }

    if (spill_ops > 0)
    {
        fprintf(output, "\nSpill Operations:\n");
        fprintf(output, "- %d variable(s) spilled to memory\n", spill_ops);
    }
}