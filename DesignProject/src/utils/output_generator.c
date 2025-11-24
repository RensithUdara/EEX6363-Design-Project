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
    fprintf(output, "AST_%s", get_ast_type_name(node->type));

    switch (node->type)
    {
    case AST_ID:
        if (node->data.string_value)
            fprintf(output, "(\"%s\")", node->data.string_value);
        break;
    case AST_INT_LITERAL:
        fprintf(output, "(%d)", node->data.int_value);
        break;
    case AST_FLOAT_LITERAL:
        fprintf(output, "(%.2f)", node->data.float_value);
        break;
    case AST_TYPE:
        fprintf(output, " [%s]", type_to_string(node->data.data_type));
        break;
    case AST_BINARY_EXPR:
    case AST_UNARY_EXPR:
        if (node->data.operator)
            fprintf(output, "(\"%s\")", node->data.operator);
        break;
    default:
        if (node->data.string_value)
            fprintf(output, "(\"%s\")", node->data.string_value);
        break;
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
    
    // Print siblings (linked list traversal)
    if (node->next)
    {
        print_ast_tree(node->next, output, depth);
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

// Generate execution output file with computed values
void generate_execution_output_file(const char *input_file, ASTNode *ast)
{
    if (!input_file || !ast)
        return;

    // Extract base filename for output file
    const char *basename = strrchr(input_file, '/');
    if (!basename) basename = strrchr(input_file, '\\');
    if (!basename) basename = input_file;
    else basename++;
    
    char base_name[256];
    strncpy(base_name, basename, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    char *dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';
    
    // Create execution output filename
    char output_filename[512];
    snprintf(output_filename, sizeof(output_filename), "%s_execution_output.txt", base_name);
    
    FILE *output = fopen(output_filename, "w");
    if (!output)
        return;
    
    // Header
    fprintf(output, "================================================================================\n");
    fprintf(output, "                        PROGRAM EXECUTION OUTPUT                                \n");
    fprintf(output, "================================================================================\n");
    fprintf(output, "Source File:     %s\n", input_file);
    fprintf(output, "Generated:       %s %s\n", __DATE__, __TIME__);
    fprintf(output, "Execution Mode:  AST Interpretation Simulation\n");
    fprintf(output, "Compiler:        EEX6363 Design Project Compiler\n");
    fprintf(output, "================================================================================\n\n");
    
    // Execute the program
    Variable variables[10];
    int var_count = 0;
    
    fprintf(output, "🚀 PROGRAM EXECUTION TRACE:\n");
    fprintf(output, "════════════════════════════════════════════════════════════════════════════\n\n");
    
    int return_value = execute_program(ast, variables, &var_count, output);
    
    fprintf(output, "\n════════════════════════════════════════════════════════════════════════════\n");
    fprintf(output, "📊 EXECUTION SUMMARY:\n");
    fprintf(output, "════════════════════════════════════════════════════════════════════════════\n\n");
    
    fprintf(output, "💾 FINAL VARIABLE STATES:\n");
    print_variable_state(variables, var_count, output);
    
    fprintf(output, "\n🎯 PROGRAM RESULT:\n");
    fprintf(output, "   └─ Return Value: %d\n", return_value);
    fprintf(output, "   └─ Execution Status: ✅ SUCCESS\n");
    
    fprintf(output, "\n================================================================================\n");
    fprintf(output, "                           END OF EXECUTION                                     \n");
    fprintf(output, "================================================================================\n");
    
    fclose(output);
    printf("✅ Execution output generated: %s\n", output_filename);
}

// Execute program AST and compute actual values
int execute_program(ASTNode *node, Variable *variables, int *var_count, FILE *output)
{
    if (!node)
        return 0;
    
    int return_value = 0;
    
    switch (node->type)
    {
    case AST_PROGRAM:
        fprintf(output, "📋 Starting program execution...\n\n");
        for (int i = 0; i < node->child_count; i++)
        {
            return_value = execute_program(node->children[i], variables, var_count, output);
        }
        break;
        
    case AST_FUNC_DEF:
        // Find function name
        char *func_name = "unknown";
        if (node->child_count > 0 && node->children[0]->type == AST_FUNC_HEAD)
        {
            ASTNode *head = node->children[0];
            for (int i = 0; i < head->child_count; i++)
            {
                if (head->children[i]->type == AST_ID)
                {
                    func_name = head->children[i]->data.string_value;
                    break;
                }
            }
        }
        
        fprintf(output, "🔧 Entering function: %s()\n", func_name);
        fprintf(output, "   ├─ Setting up local scope...\n\n");
        
        // Execute function body
        for (int i = 0; i < node->child_count; i++)
        {
            if (node->children[i]->type == AST_FUNC_BODY)
            {
                return_value = execute_program(node->children[i], variables, var_count, output);
            }
        }
        break;
        
    case AST_FUNC_BODY:
        for (int i = 0; i < node->child_count; i++)
        {
            return_value = execute_program(node->children[i], variables, var_count, output);
        }
        break;
        
    case AST_LOCAL_VAR_DECL:
        // Variable declaration
        if (node->child_count > 0 && node->children[0]->type == AST_VAR_DECL)
        {
            ASTNode *var_decl = node->children[0];
            for (int i = 0; i < var_decl->child_count; i++)
            {
                if (var_decl->children[i]->type == AST_ID)
                {
                    char *var_name = var_decl->children[i]->data.string_value;
                    if (*var_count < 10)
                    {
                        strncpy(variables[*var_count].name, var_name, sizeof(variables[*var_count].name) - 1);
                        variables[*var_count].name[sizeof(variables[*var_count].name) - 1] = '\0';
                        variables[*var_count].value = 0;
                        variables[*var_count].initialized = 0;
                        (*var_count)++;
                        
                        fprintf(output, "📝 Variable Declaration: %s\n", var_name);
                        fprintf(output, "   ├─ Type: integer\n");
                        fprintf(output, "   └─ Initial value: uninitialized\n\n");
                    }
                    break;
                }
            }
        }
        break;
        
    case AST_ASSIGN_STMT:
        // Assignment statement
        if (node->child_count >= 2)
        {
            ASTNode *lvalue = node->children[0];
            ASTNode *rvalue = node->children[1];
            
            // Get variable name
            char *var_name = NULL;
            if (lvalue->type == AST_VARIABLE && lvalue->child_count > 0 && 
                lvalue->children[0]->type == AST_ID)
            {
                var_name = lvalue->children[0]->data.string_value;
            }
            
            if (var_name)
            {
                int var_index = find_variable(variables, *var_count, var_name);
                if (var_index != -1)
                {
                    int new_value = evaluate_expression(rvalue, variables, *var_count);
                    variables[var_index].value = new_value;
                    variables[var_index].initialized = 1;
                    
                    fprintf(output, "🔄 Assignment: %s := ", var_name);
                    if (rvalue->type == AST_INT_LITERAL)
                    {
                        fprintf(output, "%d\n", new_value);
                    }
                    else if (rvalue->type == AST_BINARY_EXPR)
                    {
                        // Show the expression evaluation
                        fprintf(output, "expression result\n");
                        fprintf(output, "   ├─ Expression: ");
                        print_expression_trace(rvalue, variables, *var_count, output);
                        fprintf(output, "\n   └─ Computed value: %d\n", new_value);
                    }
                    else
                    {
                        fprintf(output, "%d\n", new_value);
                    }
                    
                    fprintf(output, "   └─ %s now contains: %d\n\n", var_name, new_value);
                }
            }
        }
        break;
        
    case AST_WRITE_STMT:
        // Write statement
        if (node->child_count > 0)
        {
            int value = evaluate_expression(node->children[0], variables, *var_count);
            fprintf(output, "📤 Output: write(%d)\n", value);
            fprintf(output, "   └─ Program outputs: %d\n\n", value);
        }
        break;
        
    case AST_RETURN_STMT:
        // Return statement
        if (node->child_count > 0)
        {
            return_value = evaluate_expression(node->children[0], variables, *var_count);
            fprintf(output, "🏁 Return: return(%d)\n", return_value);
            fprintf(output, "   └─ Function returns: %d\n\n", return_value);
        }
        break;
    }
    
    // Process siblings
    if (node->next)
    {
        return_value = execute_program(node->next, variables, var_count, output);
    }
    
    return return_value;
}

// Evaluate expressions and return computed values
int evaluate_expression(ASTNode *expr, Variable *variables, int var_count)
{
    if (!expr)
        return 0;
    
    switch (expr->type)
    {
    case AST_INT_LITERAL:
        return expr->data.int_value;
        
    case AST_VARIABLE:
        if (expr->child_count > 0 && expr->children[0]->type == AST_ID)
        {
            char *var_name = expr->children[0]->data.string_value;
            int var_index = find_variable(variables, var_count, var_name);
            if (var_index != -1 && variables[var_index].initialized)
            {
                return variables[var_index].value;
            }
        }
        return 0;
        
    case AST_BINARY_EXPR:
        if (expr->child_count >= 2)
        {
            int left_val = evaluate_expression(expr->children[0], variables, var_count);
            int right_val = evaluate_expression(expr->children[1], variables, var_count);
            
            // Get operator from the node itself
            char op = '+';  // Default
            if (expr->data.operator && expr->data.operator[0])
            {
                op = expr->data.operator[0];
            }
            
            switch (op)
            {
            case '+': return left_val + right_val;
            case '-': return left_val - right_val;
            case '*': return left_val * right_val;
            case '/': return (right_val != 0) ? left_val / right_val : 0;
            default: return left_val + right_val;  // Default to addition
            }
        }
        return 0;
        
    default:
        return 0;
    }
}

// Helper function to find variable by name
int find_variable(Variable *variables, int var_count, const char *name)
{
    for (int i = 0; i < var_count; i++)
    {
        if (strcmp(variables[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

// Print current state of all variables
void print_variable_state(Variable *variables, int var_count, FILE *output)
{
    if (var_count == 0)
    {
        fprintf(output, "   └─ No variables declared\n");
        return;
    }
    
    for (int i = 0; i < var_count; i++)
    {
        fprintf(output, "   ├─ %s = %d", variables[i].name, variables[i].value);
        if (!variables[i].initialized)
        {
            fprintf(output, " (uninitialized)");
        }
        fprintf(output, "\n");
    }
}

// Print expression evaluation trace
void print_expression_trace(ASTNode *expr, Variable *variables, int var_count, FILE *output)
{
    if (!expr)
        return;
        
    switch (expr->type)
    {
    case AST_BINARY_EXPR:
        if (expr->child_count >= 2)
        {
            print_expression_trace(expr->children[0], variables, var_count, output);
            
            // Print operator
            if (expr->data.operator)
            {
                fprintf(output, " %s ", expr->data.operator);
            }
            else
            {
                fprintf(output, " + ");  // Default
            }
            
            print_expression_trace(expr->children[1], variables, var_count, output);
        }
        break;
        
    case AST_VARIABLE:
        if (expr->child_count > 0 && expr->children[0]->type == AST_ID)
        {
            char *var_name = expr->children[0]->data.string_value;
            int var_index = find_variable(variables, var_count, var_name);
            if (var_index != -1)
            {
                fprintf(output, "%s(%d)", var_name, variables[var_index].value);
            }
            else
            {
                fprintf(output, "%s(?)", var_name);
            }
        }
        break;
        
    case AST_INT_LITERAL:
        fprintf(output, "%d", expr->data.int_value);
        break;
        
    default:
        fprintf(output, "?");
        break;
    }
}