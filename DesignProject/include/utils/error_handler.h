#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Error Types Enumeration
typedef enum
{
    // Lexical Errors
    LEX_INVALID_TOKEN,
    LEX_INVALID_NUMBER,
    LEX_INVALID_FLOAT,
    LEX_INVALID_IDENTIFIER,
    LEX_UNTERMINATED_COMMENT,
    LEX_UNTERMINATED_STRING,
    LEX_INVALID_CHARACTER,
    LEX_DOUBLE_SEMICOLON,

    // Syntax Errors
    SYNTAX_MISSING_TOKEN,
    SYNTAX_UNEXPECTED_TOKEN,
    SYNTAX_MISSING_SEMICOLON,
    SYNTAX_MISSING_BRACE,
    SYNTAX_MISSING_PARENTHESIS,
    SYNTAX_MISSING_BRACKET,
    SYNTAX_INVALID_EXPRESSION,
    SYNTAX_INVALID_STATEMENT,
    SYNTAX_INVALID_DECLARATION,
    SYNTAX_MALFORMED_FUNCTION,
    SYNTAX_MALFORMED_CLASS,
    SYNTAX_INVALID_OPERATOR,

    // Semantic Errors - Declarations
    SEM_DUPLICATE_DECLARATION,
    SEM_UNDEFINED_VARIABLE,
    SEM_UNDEFINED_FUNCTION,
    SEM_UNDEFINED_CLASS,
    SEM_UNDEFINED_MEMBER,
    SEM_REDEFINITION_ERROR,

    // Semantic Errors - Types
    SEM_TYPE_MISMATCH,
    SEM_INCOMPATIBLE_ASSIGNMENT,
    SEM_INCOMPATIBLE_OPERATION,
    SEM_INVALID_ARRAY_INDEX,
    SEM_INVALID_CAST,
    SEM_RETURN_TYPE_MISMATCH,

    // Semantic Errors - Functions
    SEM_FUNCTION_CALL_ERROR,
    SEM_WRONG_ARGUMENT_COUNT,
    SEM_ARGUMENT_TYPE_MISMATCH,
    SEM_NO_RETURN_STATEMENT,
    SEM_UNREACHABLE_CODE,
    SEM_RECURSIVE_DEFINITION,

    // Semantic Errors - Classes and Inheritance
    SEM_INHERITANCE_ERROR,
    SEM_CIRCULAR_INHERITANCE,
    SEM_INVALID_INHERITANCE,
    SEM_MEMBER_ACCESS_ERROR,
    SEM_VISIBILITY_VIOLATION,
    SEM_CONSTRUCTOR_ERROR,

    // Semantic Errors - Scopes and Variables
    SEM_SCOPE_ERROR,
    SEM_VARIABLE_NOT_INITIALIZED,
    SEM_CONSTANT_ASSIGNMENT,
    SEM_ARRAY_BOUNDS_ERROR,
    SEM_MEMORY_ACCESS_ERROR,

    // Control Flow Errors
    SEM_CONDITION_TYPE_ERROR,
    SEM_BREAK_OUTSIDE_LOOP,
    SEM_CONTINUE_OUTSIDE_LOOP,
    SEM_INVALID_CONTROL_FLOW,

    // General Errors
    SEM_GENERAL_ERROR,
    SEM_INVALID_DECLARATION,
    SEM_INVALID_IDENTIFIER,
    SEM_INVALID_TYPE,
    SEM_INVALID_PARAMETER,
    SEM_MALFORMED_FUNCTION,
    SEM_INVALID_EXPRESSION,
    SEM_INVALID_VARIABLE_ACCESS,
    SEM_INVALID_OPERATOR,
    SEM_INVALID_FUNCTION_CALL,
    SEM_INVALID_ASSIGNMENT,
    SEM_INVALID_ARRAY_ACCESS,
    INTERNAL_ERROR,

    // Warning Types
    WARN_UNUSED_VARIABLE,
    WARN_UNUSED_PARAMETER,
    WARN_UNUSED_FUNCTION,
    WARN_IMPLICIT_CONVERSION,
    WARN_UNREACHABLE_CODE,
    WARN_UNINITIALIZED_VAR,
    WARN_DEPRECATED_FEATURE,
    WARN_PERFORMANCE_ISSUE,
    WARN_STYLE_VIOLATION,
    WARN_POTENTIAL_ERROR,
    WARN_SHADOWED_VARIABLE,
    WARN_MISSING_RETURN,
    WARN_EMPTY_STATEMENT
} ErrorType;

// Error Severity Levels
typedef enum
{
    ERROR_INFO = 1,
    ERROR_WARNING = 2,
    ERROR_ERROR = 3,
    ERROR_FATAL = 4
} ErrorSeverity;

// Enhanced Error Structure
typedef struct ComprehensiveError
{
    ErrorType type;
    ErrorSeverity severity;
    int line;
    int column;
    char *message;
    char *filename;
    char *expected;
    char *found;
    char *context;
    char *suggestion;
    char *error_code;
    struct ComprehensiveError *next;
} ComprehensiveError;

// Error Manager Structure
typedef struct ErrorManager
{
    ComprehensiveError *syntax_errors;
    ComprehensiveError *semantic_errors;
    ComprehensiveError *lexical_errors;
    int syntax_error_count;
    int semantic_error_count;
    int lexical_error_count;
    int warning_count;
    int max_errors;
    int suppress_warnings;
    char *current_filename;
} ErrorManager;

// Global Error Manager
extern ErrorManager *global_error_manager;

// Error Manager Functions
ErrorManager *create_error_manager(void);
void destroy_error_manager(ErrorManager *manager);
void reset_error_manager(ErrorManager *manager);
void set_current_filename(const char *filename);
void init_error_manager(void);

// Error Reporting Functions
void report_lexical_error(ErrorType type, int line, int col, const char *message,
                          const char *found, const char *suggestion);
void report_syntax_error_enhanced(ErrorType type, int line, int col, const char *message,
                                  const char *expected, const char *found, const char *suggestion);
void report_semantic_error_enhanced(ErrorType type, int line, int col, const char *message,
                                    const char *context, const char *suggestion);
void report_semantic_error(int line, int col, const char *message, const char *error_type);
void report_semantic_error_detailed(int line, int col, const char *message, const char *error_type,
                                    const char *context, const char *suggestion);
void report_warning(ErrorType type, int line, int col, const char *message, const char *suggestion);

// Specific Error Reporting Functions
void report_duplicate_declaration(int line, int col, const char *name, const char *type);
void report_undefined_symbol(int line, int col, const char *name, const char *symbol_type);
void report_type_mismatch(int line, int col, DataType expected, DataType found, const char *context);
void report_function_call_error(int line, int col, const char *func_name, int expected_args, int found_args);
void report_argument_type_error(int line, int col, const char *func_name, int arg_num,
                                DataType expected, DataType found);
void report_inheritance_error(int line, int col, const char *child, const char *parent, const char *reason);
void report_member_access_error(int line, int col, const char *member, const char *class_name);
void report_visibility_violation(int line, int col, const char *member, const char *visibility);

// Comprehensive Type and Assignment Error Functions
void report_incompatible_assignment(int line, int col, DataType lhs_type, DataType rhs_type, const char *variable_name);
void report_incompatible_operation(int line, int col, const char *operator, DataType left_type, DataType right_type);
void report_invalid_array_index(int line, int col, DataType index_type, const char *array_name);
void report_invalid_cast(int line, int col, DataType from_type, DataType to_type);
void report_return_type_mismatch(int line, int col, const char *function_name, DataType expected_type, DataType actual_type);

// Function and Control Flow Error Functions
void report_no_return_statement(int line, int col, const char *function_name, DataType return_type);
void report_unreachable_code(int line, int col, const char *context);
void report_recursive_definition(int line, int col, const char *name, const char *definition_type);
void report_circular_inheritance(int line, int col, const char *class1, const char *class2);
void report_invalid_inheritance(int line, int col, const char *child, const char *parent, const char *reason);

// Scope and Variable Error Functions
void report_scope_error(int line, int col, const char *identifier, const char *scope_context);
void report_variable_not_initialized(int line, int col, const char *variable_name);
void report_constant_assignment(int line, int col, const char *constant_name);
void report_array_bounds_error(int line, int col, const char *array_name, int index, int array_size);
void report_memory_access_error(int line, int col, const char *operation, const char *context);

// Control Flow and Loop Error Functions
void report_condition_type_error(int line, int col, DataType condition_type, const char *statement_type);
void report_break_outside_loop(int line, int col);
void report_continue_outside_loop(int line, int col);
void report_invalid_control_flow(int line, int col, const char *statement, const char *context);

// Class and Constructor Error Functions
void report_constructor_error(int line, int col, const char *class_name, const char *error_details);
void report_redefinition_error(int line, int col, const char *name, const char *entity_type);

// Comprehensive Lexical Error Functions
void report_invalid_token(int line, int col, const char *token);
void report_invalid_number(int line, int col, const char *number_str);
void report_invalid_float(int line, int col, const char *float_str);
void report_invalid_identifier(int line, int col, const char *identifier);
void report_unterminated_comment(int line, int col);
void report_unterminated_string(int line, int col, const char *string_start);
void report_invalid_character(int line, int col, char invalid_char);
void report_double_semicolon(int line, int col);

// Comprehensive Syntax Error Functions
void report_missing_token(int line, int col, const char *expected_token, const char *context);
void report_unexpected_token(int line, int col, const char *found_token, const char *expected_tokens);
void report_missing_semicolon(int line, int col, const char *statement_type);
void report_missing_brace(int line, int col, const char *brace_type, const char *context);
void report_missing_parenthesis(int line, int col, const char *paren_type, const char *context);
void report_missing_bracket(int line, int col, const char *bracket_type, const char *context);
void report_invalid_expression(int line, int col, const char *expression_context);
void report_invalid_statement(int line, int col, const char *statement_context);
void report_invalid_declaration(int line, int col, const char *declaration_type);
void report_malformed_function(int line, int col, const char *function_name, const char *issue);
void report_malformed_class(int line, int col, const char *class_name, const char *issue);
void report_invalid_operator(int line, int col, const char *operator_str, const char *context);

// Error Analysis and Recovery
int has_fatal_errors(void);
int should_continue_analysis(void);

// Error Count Functions
int get_lexical_error_count(void);
int get_syntax_error_count(void);
int get_semantic_error_count(void);
int get_total_error_count(void);
int get_warning_count(void);
void suggest_error_fix(ErrorType type, const char *context, char *suggestion, size_t suggestion_size);

// Comprehensive Warning System
void perform_warning_analysis(ASTNode *root);
void report_semantic_warnings(void);

// Specific Warning Detection Functions
void check_unused_variables(SymbolTable *scope);
void check_unused_parameters(Symbol *function_symbol);
void check_unused_functions(SymbolTable *scope);
void check_implicit_conversions(ASTNode *node);
void check_unreachable_code_warnings(ASTNode *node);
void check_uninitialized_variables(ASTNode *node, SymbolTable *scope);
void check_shadowed_variables(SymbolTable *scope);
void check_missing_returns(ASTNode *function_node);
void check_performance_issues(ASTNode *node);
void check_style_violations(ASTNode *node);

// Warning Reporting Functions
void report_unused_variable_warning(int line, int col, const char *var_name);
void report_unused_parameter_warning(int line, int col, const char *param_name, const char *func_name);
void report_unused_function_warning(int line, int col, const char *func_name);
void report_implicit_conversion_warning(int line, int col, DataType from, DataType to);
void report_unreachable_code_warning(int line, int col, const char *context);
void report_uninitialized_variable_warning(int line, int col, const char *var_name);
void report_shadowed_variable_warning(int line, int col, const char *var_name, const char *outer_scope);
void report_missing_return_warning(int line, int col, const char *func_name);
void report_performance_warning(int line, int col, const char *issue, const char *suggestion);
void report_style_warning(int line, int col, const char *issue, const char *suggestion);

// Warning helper functions
DataType get_expression_type(ASTNode *node);
DataType string_to_data_type(const char *type_str);
char *type_to_string(DataType type); // Use existing function name
int has_return_statement(ASTNode *node);
void check_for_string_concat_in_loop(ASTNode *node, int loop_line, int loop_col);
void check_for_repeated_calls(ASTNode *node);
char *extract_source_line(const char *filename, int line_num);
char *get_error_type_string(ErrorType type);
char *get_error_code(ErrorType type);

// Error Output Functions
void print_error_summary(FILE *output);
void print_detailed_error_report(FILE *output);
void print_error_statistics(FILE *output);
void write_comprehensive_error_report(FILE *file);
void write_error_report_to_file(const char *filename);

// Error Recovery Functions
void attempt_error_recovery(ErrorType type);
void skip_to_synchronization_point(void);
int find_recovery_token(void);

// Utility Functions
const char *get_severity_string(ErrorSeverity severity);
const char *get_severity_icon(ErrorSeverity severity);
int compare_errors_by_line(const void *a, const void *b);
void sort_errors_by_location(void);

// Enhanced Error Context Functions
char *get_detailed_context(const char *filename, int line, int col, int context_lines);
void highlight_error_position(char *line_text, int column);
char *format_error_message(ComprehensiveError *error);

// Error Prevention and Validation
int validate_identifier(const char *id);
int validate_number_format(const char *number);
int validate_float_format(const char *float_str);
int validate_operator_usage(const char *operator, DataType left, DataType right);

#endif // ERROR_HANDLER_H