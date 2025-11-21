#ifndef SEMANTIC_INTEGRATION_H
#define SEMANTIC_INTEGRATION_H

#include "../TMA03/ast.h"
#include "../TMA03/error_handler.h"

// Forward declarations from TMA03
extern ASTNode *ast_root;
extern int error_count;
extern int max_errors;
extern int semantic_error_count;
extern int semantic_warning_count;

// Semantic analysis integration functions
int perform_semantic_analysis(ASTNode *root);
void initialize_semantic_analyzer(void);
void cleanup_semantic_analyzer(void);
int get_semantic_error_count(void);
int get_semantic_warning_count(void);

// Parser integration functions
ASTNode *parse_input_file(const char *filename);
void setup_parser_input(FILE *input_file);

// Symbol table functions
void print_all_symbol_tables(void);
void write_symbol_tables_to_file(const char *filename);

// Type checking functions
DataType get_expression_type_analysis(ASTNode *expr);
int check_type_compatibility(DataType type1, DataType type2);
int validate_assignment(ASTNode *lhs, ASTNode *rhs);

// Scope management
void enter_scope(char *scope_name);
void exit_scope(void);
Symbol *lookup_symbol_in_scope(const char *name);

#endif // SEMANTIC_INTEGRATION_H