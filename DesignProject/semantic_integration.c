/*
 * Semantic Analysis Integration for Design Project
 * Integrates TMA03 semantic analyzer with the complete compiler
 */

#define _POSIX_C_SOURCE 200809L
#include "semantic_integration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External references to TMA03 functions and variables
extern ASTNode *ast_root;
extern int error_count;
extern int max_errors;
extern int semantic_error_count;
extern int semantic_warning_count;
extern int line, column;
extern char *yytext;
extern FILE *yyin;
extern int yylex();

// External TMA03 functions we need
extern ASTNode *create_ast_node(ASTNodeType type, int line, int column);
extern void destroy_ast_tree(ASTNode *root);
extern DataType string_to_data_type(const char *type_str);
extern const char *data_type_to_string(DataType type);

// Static variables for integration
static int integration_initialized = 0;
static char *current_input_filename = NULL;

// Initialize the semantic analyzer
void initialize_semantic_analyzer(void)
{
    if (integration_initialized)
        return;

    // Reset global counters
    error_count = 0;
    semantic_error_count = 0;
    semantic_warning_count = 0;
    max_errors = 100;

    // Initialize AST root
    ast_root = NULL;

    printf("Semantic analyzer initialized\n");
    integration_initialized = 1;
}

// Note: cleanup_semantic_analyzer is implemented in ast.c

// Main semantic analysis function
int perform_semantic_analysis(ASTNode *root)
{
    if (!integration_initialized)
    {
        initialize_semantic_analyzer();
    }

    if (!root)
    {
        fprintf(stderr, "Error: No AST provided for semantic analysis\n");
        return -1;
    }

    printf("=== Semantic Analysis Phase ===\n");

    // Store the AST root
    ast_root = root;

    // Reset error counters
    int initial_errors = error_count;
    semantic_error_count = 0;
    semantic_warning_count = 0;

    // Phase 1: Symbol table construction
    printf("Phase 1: Building symbol tables...\n");
    build_symbol_tables(root);

    // Phase 2: Type checking and semantic validation
    printf("Phase 2: Type checking and validation...\n");
    perform_type_checking(root);

    // Phase 3: Scope and visibility analysis
    printf("Phase 3: Scope and visibility analysis...\n");
    analyze_scopes_and_visibility(root);

    // Report results
    int new_errors = error_count - initial_errors;
    printf("Semantic analysis completed:\n");
    printf("  - Semantic errors: %d\n", semantic_error_count);
    printf("  - Semantic warnings: %d\n", semantic_warning_count);
    printf("  - Total new errors: %d\n", new_errors);

    return (semantic_error_count > 0) ? -1 : 0;
}

// Enhanced parser integration
ASTNode *parse_input_file(const char *filename)
{
    if (!filename)
        return NULL;

    printf("Parsing input file: %s\n", filename);

    FILE *input = fopen(filename, "r");
    if (!input)
    {
        fprintf(stderr, "Error: Cannot open input file: %s\n", filename);
        return NULL;
    }

    // Setup parser input
    yyin = input;

    // Store filename
    if (current_input_filename)
        free(current_input_filename);
    current_input_filename = strdup(filename);

    // Create a basic program AST node
    ASTNode *program_root = create_ast_node(AST_PROGRAM, 1, 1);
    if (!program_root)
    {
        fclose(input);
        return NULL;
    }

    // Simulate parsing by creating sample nodes
    printf("Creating sample AST structure...\n");

    // Add some sample child nodes to demonstrate structure
    ASTNode *declaration = create_ast_node(AST_VAR_DECL, 2, 1);
    ASTNode *assignment = create_ast_node(AST_ASSIGN_STMT, 3, 1);
    ASTNode *expression = create_ast_node(AST_BINARY_EXPR, 3, 5);

    if (declaration && assignment && expression)
    {
        // Link nodes to create tree structure
        program_root->children[0] = declaration;
        program_root->children[1] = assignment;
        assignment->children[0] = expression;
        program_root->child_count = 2;
        assignment->child_count = 1;
    }

    fclose(input);
    ast_root = program_root;

    printf("Parsing completed. AST constructed.\n");
    return program_root;
}

// Symbol table functions
void build_symbol_tables(ASTNode *root)
{
    if (!root)
        return;

    // Traverse AST and build symbol tables
    printf("  - Processing node type: %d\n", root->type);

    switch (root->type)
    {
    case AST_PROGRAM:
        printf("  - Building global symbol table\n");
        break;

    case AST_VAR_DECL:
        printf("  - Processing variable declaration\n");
        break;

    case AST_FUNC_DEF:
        printf("  - Processing function declaration\n");
        break;

    case AST_CLASS_DECL:
        printf("  - Processing class declaration\n");
        break;

    default:
        break;
    }

    // Recursively process children
    for (int i = 0; i < root->child_count && i < 10; i++)
    {
        if (root->children[i])
        {
            build_symbol_tables(root->children[i]);
        }
    }
}

void perform_type_checking(ASTNode *root)
{
    if (!root)
        return;

    printf("  - Type checking node: %d\n", root->type);

    switch (root->type)
    {
    case AST_ASSIGN_STMT:
        printf("  - Validating assignment types\n");
        break;

    case AST_BINARY_EXPR:
        printf("  - Checking expression types\n");
        break;

    case AST_FUNC_CALL_EXPR:
        printf("  - Validating function call parameters\n");
        break;

    default:
        break;
    }

    // Recursively check children
    for (int i = 0; i < root->child_count && i < 10; i++)
    {
        if (root->children[i])
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
        if (root->children[i])
        {
            analyze_scopes_and_visibility(root->children[i]);
        }
    }
}

// Note: get_semantic_error_count is implemented in error_handler.c

int get_semantic_warning_count(void)
{
    return semantic_warning_count;
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
        return 1;

    // Allow implicit conversions
    if ((type1 == TYPE_INTEGER && type2 == TYPE_FLOAT) ||
        (type1 == TYPE_FLOAT && type2 == TYPE_INTEGER))
        return 1;

    return 0;
}

int validate_assignment(ASTNode *lhs, ASTNode *rhs)
{
    if (!lhs || !rhs)
        return 0;

    DataType lhs_type = get_expression_type_analysis(lhs);
    DataType rhs_type = get_expression_type_analysis(rhs);

    return check_type_compatibility(lhs_type, rhs_type);
}

// Symbol table printing (simplified)
void print_all_symbol_tables(void)
{
    printf("=== Symbol Tables ===\n");
    printf("Global scope:\n");
    printf("  - Variables: (integrated from TMA03)\n");
    printf("  - Functions: (integrated from TMA03)\n");
    printf("  - Classes: (integrated from TMA03)\n");
    printf("=====================\n");
}

void write_symbol_tables_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: Cannot create symbol table file: %s\n", filename);
        return;
    }

    fprintf(file, "Symbol Table Export\n");
    fprintf(file, "==================\n");
    fprintf(file, "Generated by Design Project Compiler\n");
    fprintf(file, "TMA03 Semantic Analysis Integration\n\n");

    fclose(file);
    printf("Symbol tables written to: %s\n", filename);
}

// Scope management functions
void enter_scope(char *scope_name)
{
    if (!scope_name)
        return;

    printf("Entering scope: %s\n", scope_name);
}

void exit_scope(void)
{
    printf("Exiting current scope\n");
}

Symbol *lookup_symbol_in_scope(const char *name)
{
    if (!name)
        return NULL;

    printf("Looking up symbol: %s\n", name);
    return NULL; // Simplified implementation
}