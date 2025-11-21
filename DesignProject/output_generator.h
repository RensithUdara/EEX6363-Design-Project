#ifndef OUTPUT_GENERATOR_H
#define OUTPUT_GENERATOR_H

#include "../TMA03/ast.h"
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
void print_register_allocation_info(FILE *output);
void print_detailed_symbol_tables(FILE *output);
int count_total_symbols(void);
int get_syntax_error_count(void);

// AST utility functions that may be missing
void print_ast_tree(ASTNode *node, FILE *output, int depth);

#endif // OUTPUT_GENERATOR_H