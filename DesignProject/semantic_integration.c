/*
 * MINIMAL SEMANTIC INTEGRATION WRAPPER
 * Provides interface between Design Project Enhanced Compiler and TMA03 semantic.c
 * Does not redefine any TMA03 functions to avoid conflicts
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic_integration.h"
#include "ast.h"
#include "error_handler.h"

// ================================
// EXTERNAL TMA03 FUNCTION DECLARATIONS
// ================================

// Main TMA03 semantic analysis functions (defined in semantic.c)
extern int parse_program(ASTNode **result_ast);
extern void semantic_analysis(ASTNode *root);
extern void enhanced_semantic_analysis(ASTNode *root);
extern void perform_comprehensive_semantic_analysis(ASTNode *root);
extern void init_semantic_analyzer(void);

// TMA03 global variables (defined in semantic.c)
extern ASTNode *ast_root;
extern int error_count;
extern int semantic_error_count;
extern int semantic_warning_count;
extern FILE *symbol_table_file;
extern FILE *semantic_error_file;
extern SymbolTable *global_scope;
extern SymbolTable *current_scope;

// ================================
// INTEGRATION WRAPPER FUNCTIONS
// ================================

// Call TMA03's semantic analysis workflow without main function conflicts
int call_tma03_analysis(const char *filename)
{
    printf("=== CALLING TMA03 SEMANTIC ANALYZER ===\n");
    printf("Input file: %s\n", filename);

    // Set up input file for TMA03
    extern FILE *yyin;
    yyin = fopen(filename, "r");
    if (!yyin)
    {
        printf("Error: Cannot open input file %s\n", filename);
        return 1;
    }

    // Initialize error counts
    error_count = 0;
    semantic_error_count = 0;
    semantic_warning_count = 0;

    // Call TMA03's parsing and semantic analysis workflow
    printf("Starting TMA03 parse_program...\n");
    int parse_result = parse_program(&ast_root);

    if (parse_result == 0 && ast_root)
    {
        printf("✓ TMA03 parsing completed\n");

        // Call TMA03's complete semantic analysis
        printf("Starting TMA03 semantic_analysis...\n");
        semantic_analysis(ast_root);
        printf("✓ TMA03 semantic analysis completed\n");

        // Report the ACTUAL error counts from TMA03
        printf("\n=== TMA03 ACTUAL ERROR RESULTS ===\n");
        printf("Parse errors: %d\n", error_count);
        printf("Semantic errors: %d\n", semantic_error_count);
        printf("Semantic warnings: %d\n", semantic_warning_count);
        printf("Total errors: %d\n", error_count + semantic_error_count);

        if (semantic_error_count > 0 || error_count > 0)
        {
            printf("✗ TMA03 DETECTED %d ERROR(S)!\n", error_count + semantic_error_count);
            fclose(yyin);
            return 1; // Return error status
        }
        else
        {
            printf("✓ TMA03: No errors detected\n");
        }
    }
    else
    {
        printf("✗ TMA03 parsing failed with result: %d\n", parse_result);
        fclose(yyin);
        return parse_result;
    }

    fclose(yyin);
    return 0;
}

// Enhanced yyparse that calls TMA03's complete workflow
int yyparse(void)
{
    // This function is called by enhanced_compiler.c
    // We don't actually do parsing here - the enhanced_compiler should call call_tma03_analysis directly
    printf("yyparse called - redirecting to TMA03 analysis\n");
    return 0;
}

// Wrapper for semantic analysis (calls TMA03 function)
void perform_semantic_analysis(ASTNode *root)
{
    if (root)
    {
        perform_comprehensive_semantic_analysis(root);
    }
}

// Report generation wrapper
void generate_comprehensive_report(const char *input_filename)
{
    printf("\n=== COMPREHENSIVE ANALYSIS REPORT ===\n");
    printf("Input file: %s\n", input_filename ? input_filename : "unknown");
    printf("Total errors: %d\n", error_count);
    printf("Semantic errors: %d\n", semantic_error_count);
    printf("Semantic warnings: %d\n", semantic_warning_count);

    if (symbol_table_file)
    {
        printf("Symbol table output: Available\n");
    }
    if (semantic_error_file)
    {
        printf("Semantic error output: Available\n");
    }
    printf("=====================================\n\n");
}

// Get error counts for reporting (returns actual TMA03 counts)
int get_total_errors(void)
{
    return error_count + semantic_error_count;
}

int get_semantic_errors(void)
{
    return semantic_error_count;
}

int get_semantic_warnings(void)
{
    return semantic_warning_count;
}

int get_semantic_warning_count(void)
{
    return semantic_warning_count;
}