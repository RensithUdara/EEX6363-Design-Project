#ifndef SEMANTIC_INTEGRATION_H
#define SEMANTIC_INTEGRATION_H

#include "ast.h"
#include "error_handler.h"

// Global variable declarations (defined in semantic_integration.c)
extern ASTNode *ast_root;
extern int error_count;
extern int max_errors;
extern int semantic_error_count;
extern int semantic_warning_count;

// Core semantic analysis functions
void semantic_analysis(ASTNode *root);
int call_tma03_analysis(const char *filename);  // Direct TMA03 main call
int parse_program_comprehensive(void);
int parse_prog(void);
ASTNode *parse_class_list(void);
ASTNode *parse_class_decl(void);
ASTNode *parse_inherit_list(void);
ASTNode *parse_member_list(void);

// Phase-based analysis functions
void perform_phase1_analysis(ASTNode *node);
void perform_phase2_analysis(ASTNode *node);
void perform_advanced_semantic_checks(ASTNode *node);
void perform_comprehensive_warning_analysis(ASTNode *node);

// Class and function analysis
void analyze_class_declaration_phase1(ASTNode *node);
void analyze_function_definition_phase1(ASTNode *node);
void analyze_variable_declaration_phase1(ASTNode *node);
void analyze_assignment_statement(ASTNode *node);
void analyze_function_call(ASTNode *node);
void analyze_binary_expression(ASTNode *node);
void analyze_unary_expression(ASTNode *node);
void analyze_variable_access(ASTNode *node);
void analyze_if_statement(ASTNode *node);
void analyze_while_statement(ASTNode *node);
void analyze_return_statement(ASTNode *node);

// Advanced semantic checks
void check_unreachable_code(ASTNode *node);
void check_uninitialized_variables(ASTNode *node, SymbolTable *scope);
void check_infinite_loops(ASTNode *node);
void check_missing_returns(ASTNode *node);
void check_array_bounds(ASTNode *node);
void check_unused_variables(SymbolTable *scope);
void check_unreachable_patterns(ASTNode *node);
void check_performance_issues(ASTNode *node);
void check_style_issues(ASTNode *node);
void check_security_issues(ASTNode *node);

// Parser interface functions
int yyparse(void);
int parse_program(ASTNode **ast);
ASTNode *parse_input_file(const char *filename);

// Error and warning reporting
int get_semantic_error_count(void);
int get_semantic_warning_count(void);
void handle_syntax_error(int line, int col, const char *message, const char *context);

// Utility functions
int count_ast_nodes(ASTNode *node);
int count_total_symbols(void);
int count_symbols_recursive(SymbolTable *table);
int count_symbols_in_table(SymbolTable *table);
int count_total_scopes(void);

// Type analysis functions
DataType get_expression_type(ASTNode *node);
int is_lvalue(ASTNode *node);
char *get_node_description(ASTNode *node);
int types_compatible(DataType type1, DataType type2);

// Helper functions for analysis
void analyze_children_phase1(ASTNode *node);
void analyze_children_phase2(ASTNode *node);

// Report generation functions
void generate_comprehensive_semantic_report(void);
void generate_detailed_error_report(FILE *file);
void generate_detailed_symbol_table_report(FILE *file);
void generate_performance_metrics_report(void);
void generate_architecture_analysis_report(void);

// System initialization and cleanup
void initialize_comprehensive_semantic_system(void);
void cleanup_comprehensive_semantic_system(void);
void initialize_symbol_tables(void);
void cleanup_symbol_tables(void);

// Symbol table printing functions
void print_symbol_table_comprehensive(SymbolTable *table, FILE *file, int indent);

// Missing parser functions
ASTNode *parse_function_declaration(void);
ASTNode *parse_attribute_declaration(void);

// Additional helper functions
int count_scopes_recursive(SymbolTable *table);
DataType get_function_return_type(ASTNode *func_node);
int is_constant_true(ASTNode *node);
int has_return_statement(ASTNode *node);

// Backward compatibility functions
void perform_semantic_analysis(ASTNode *root);
void initialize_semantic_analyzer(void);
void cleanup_semantic_analyzer(void);
void print_all_symbol_tables(void);
void write_symbol_tables_to_file(const char *filename);

#endif // SEMANTIC_INTEGRATION_H