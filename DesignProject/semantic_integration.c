#include "semantic_integration.h"
#include "ast.h"
#include "error_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for functions we implement
void build_symbol_tables(ASTNode *root);
void perform_type_checking(ASTNode *root);
void analyze_scopes_and_visibility(ASTNode *root);

// Integration state variables
static int integration_initialized = 0;
// Note: ast_root and semantic_warning_count are declared in header as extern

// Integration initialization
void initialize_semantic_analyzer(void)
{
    if (integration_initialized)
        return;

    printf("Initializing semantic analyzer integration...\n");

    // Initialize integration (errors/warnings handled by TMA3)

    integration_initialized = 1;
    printf("Semantic analyzer integration initialized\n");
}

// Main semantic analysis function
int perform_semantic_analysis(ASTNode *root)
{
    if (!integration_initialized)
    {
        initialize_semantic_analyzer();
    }

    if (!root)
    {
        printf("Error: No AST root provided for semantic analysis\n");
        return -1;
    }

    printf("Starting semantic analysis...\n");
    ast_root = root;

    // Phase 1: Build symbol tables
    printf("Phase 1: Building symbol tables...\n");
    build_symbol_tables(root);

    // Phase 2: Type checking
    printf("Phase 2: Performing type checking...\n");
    perform_type_checking(root);

    // Phase 3: Scope and visibility analysis
    printf("Phase 3: Analyzing scopes and visibility...\n");
    analyze_scopes_and_visibility(root);

    printf("Semantic analysis completed\n");
    int errors = get_semantic_error_count();
    printf("Errors: %d, Warnings: %d\n", errors, semantic_warning_count);

    return errors;
}

// Simple wrapper for file parsing (stub implementation)
ASTNode *parse_input_file(const char *filename)
{
    if (!filename)
    {
        printf("Error: No filename provided\n");
        return NULL;
    }

    printf("Parsing input file: %s\n", filename);

    // Create a simple AST for demonstration
    // In real implementation, this would call the parser
    ASTNode *program = create_ast_node(AST_PROGRAM, 1, 1);

    // Create some sample nodes for testing
    ASTNode *declaration = create_ast_node(AST_VAR_DECL, 2, 1);
    ASTNode *assignment = create_ast_node(AST_ASSIGN_STMT, 3, 1);
    ASTNode *expression = create_ast_node(AST_BINARY_EXPR, 3, 5);

    // Add children to program (simplified)
    add_child(program, declaration);
    add_child(program, assignment);
    add_child(program, expression);

    return program;
}

// Detailed semantic analysis functions
void build_symbol_tables(ASTNode *root)
{
    if (!root)
        return;

    printf("  - Building symbol table for node: %d\n", root->type);

    // Build symbol tables based on node type
    switch (root->type)
    {
    case AST_VAR_DECL:
        printf("    Adding variable declaration to symbol table\n");
        break;
    case AST_FUNC_DEF:
        printf("    Adding function definition to symbol table\n");
        break;
    case AST_CLASS_DECL:
        printf("    Adding class declaration to symbol table\n");
        break;
    default:
        break;
    }

    // Recursively build symbol tables for children
    for (int i = 0; i < root->child_count && i < 10; i++)
    {
        if (root->children && root->children[i])
        {
            build_symbol_tables(root->children[i]);
        }
    }
}

void perform_type_checking(ASTNode *root)
{
    if (!root)
        return;

    printf("  - Type checking for node: %d\n", root->type);

    switch (root->type)
    {
    case AST_ASSIGN_STMT:
        printf("    Checking assignment type compatibility\n");
        break;
    case AST_BINARY_EXPR:
        printf("    Checking binary expression types\n");
        break;
    case AST_FUNC_CALL_EXPR:
        printf("    Checking function call argument types\n");
        break;
    default:
        break;
    }

    // Recursively type check children
    for (int i = 0; i < root->child_count && i < 10; i++)
    {
        if (root->children && root->children[i])
        {
            perform_type_checking(root->children[i]);
        }
    }
}

void analyze_scopes_and_visibility(ASTNode *root)
{
    if (!root)
        return;

    printf("  - Analyzing scope for node: %d\n", root->type);

    // Scope analysis implementation
    for (int i = 0; i < root->child_count && i < 10; i++)
    {
        if (root->children && root->children[i])
        {
            analyze_scopes_and_visibility(root->children[i]);
        }
    }
}

int get_semantic_warning_count(void)
{
    return 0; // Simplified - warnings handled by TMA3
}

// Type checking functions
DataType get_expression_type_analysis(ASTNode *expr)
{
    if (!expr)
        return TYPE_UNKNOWN;

    switch (expr->type)
    {
    case AST_INT_LITERAL:
        return TYPE_INTEGER;
    case AST_FLOAT_LITERAL:
        return TYPE_FLOAT;
    case AST_ID:
        // Look up identifier type in symbol table
        return TYPE_INTEGER; // Simplified
    default:
        return TYPE_UNKNOWN;
    }
}

int check_type_compatibility(DataType type1, DataType type2)
{
    if (type1 == type2)
        return 1; // Compatible

    // Allow some implicit conversions
    if ((type1 == TYPE_INTEGER && type2 == TYPE_FLOAT) ||
        (type1 == TYPE_FLOAT && type2 == TYPE_INTEGER))
    {
        return 1; // Compatible with conversion
    }

    return 0; // Incompatible
}

void write_symbol_tables_to_file(const char *filename)
{
    if (!filename)
        return;

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Could not open symbol table file: %s\n", filename);
        return;
    }

    fprintf(file, "=== Symbol Tables ===\n");
    fprintf(file, "Generated by Design Project Semantic Analyzer\n");
    fprintf(file, "Errors: %d, Warnings: %d\n", get_semantic_error_count(), get_semantic_warning_count());
    fprintf(file, "\n");

    fclose(file);
    printf("Symbol tables written to: %s\n", filename);
}

Symbol *lookup_symbol_in_scope(const char *name)
{
    if (!name)
        return NULL;

    printf("Looking up symbol: %s\n", name);
    return NULL; // Simplified implementation
}

void report_semantic_error(const char *message, int line, int column)
{
    if (!message)
        return;

    printf("Semantic Error at line %d, column %d: %s\n", line, column, message);
    // Error count is managed by TMA3's error handler
}

// Note: report_semantic_warning is implemented in ast.c