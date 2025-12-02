#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "../core/ast.h"
#include <stdio.h>

// Function declarations for dynamic output generation
int initialize_output_files(const char *input_filename);
void close_output_files(void);
void generate_lexical_output(const char *input_file);
void generate_syntax_output(const char *input_file, ASTNode *ast);
void generate_semantic_output(const char *input_file);
void generate_symbol_table_output(const char *input_file);
void generate_intermediate_code_output(const char *input_file, ASTNode *ast);
void generate_test_results_file(const char *input_file, int success);

// Helper functions for code generation
void generate_three_address_code(ASTNode *ast, FILE *output);
void generate_quadruple_code(ASTNode *ast, FILE *output);
void generate_triple_code(ASTNode *ast, FILE *output);
void generate_quadruples_recursive(ASTNode *ast, FILE *output, int *counter);
void generate_triples_recursive(ASTNode *ast, FILE *output, int *counter);
void print_register_info_to_file(FILE *output);
void print_detailed_symbol_tables(FILE *output);
int count_total_symbols(void);
int get_syntax_error_count(void);

// AST utility functions that may be missing
void print_ast_tree(ASTNode *node, FILE *output, int depth);

// Execution output generation
void generate_execution_output_file(const char *input_file, ASTNode *ast);
typedef struct {
    char name[64];
    int value;
    int initialized;
} Variable;
int execute_program(ASTNode *node, Variable *variables, int *var_count, FILE *output);
int evaluate_expression(ASTNode *expr, Variable *variables, int var_count);
int find_variable(Variable *variables, int var_count, const char *name);
void print_variable_state(Variable *variables, int var_count, FILE *output);
void print_expression_trace(ASTNode *expr, Variable *variables, int var_count, FILE *output);

#endif // OUTPUT_GENERATOR_H