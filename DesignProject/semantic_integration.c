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

// Enhanced yyparse that calls TMA03's parse_program
int yyparse(void) {
    printf("Starting TMA03 semantic analysis integration...\n");
    
    // Call TMA03's parse_program function
    int result = parse_program(&ast_root);
    
    if (result == 0 && ast_root) {
        printf("✓ TMA03 parsing successful\n");
        
        // Call TMA03's comprehensive semantic analysis
        printf("Starting comprehensive semantic analysis...\n");
        perform_comprehensive_semantic_analysis(ast_root);
        printf("✓ Semantic analysis completed\n");
    } else {
        printf("✗ TMA03 parsing failed with result: %d\n", result);
    }
    
    return result;
}

// Wrapper for semantic analysis (calls TMA03 function)
void perform_semantic_analysis(ASTNode *root) {
    if (root) {
        perform_comprehensive_semantic_analysis(root);
    }
}

// Report generation wrapper
void generate_comprehensive_report(const char *input_filename) {
    printf("\n=== COMPREHENSIVE ANALYSIS REPORT ===\n");
    printf("Input file: %s\n", input_filename ? input_filename : "unknown");
    printf("Total errors: %d\n", error_count);
    printf("Semantic errors: %d\n", semantic_error_count);
    printf("Semantic warnings: %d\n", semantic_warning_count);
    
    if (symbol_table_file) {
        printf("Symbol table output: Available\n");
    }
    if (semantic_error_file) {
        printf("Semantic error output: Available\n");
    }
    printf("=====================================\n\n");
}

// Get error counts for reporting
int get_total_errors(void) {
    return error_count;
}

int get_semantic_errors(void) {
    return semantic_error_count;
}

int get_semantic_warnings(void) {
    return semantic_warning_count;
}

int get_semantic_warning_count(void) {
    return semantic_warning_count;
}