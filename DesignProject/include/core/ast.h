#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
typedef struct ASTNode ASTNode;
typedef struct SymbolTable SymbolTable;
typedef struct Symbol Symbol;

// AST Node Types
typedef enum
{
    // Program structure
    AST_PROGRAM,
    AST_CLASS_DECL,
    AST_IMPL_DEF,
    AST_FUNC_DEF,
    AST_FUNC_DECL,
    AST_FUNC_HEAD,
    AST_FUNC_BODY,

    // Variable declarations
    AST_VAR_DECL,
    AST_ATTR_DECL,
    AST_LOCAL_VAR_DECL,
    AST_PARAM_DECL,

    // Types
    AST_TYPE,
    AST_ARRAY_SIZE,

    // Statements
    AST_ASSIGN_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_READ_STMT,
    AST_WRITE_STMT,
    AST_RETURN_STMT,
    AST_FUNC_CALL_STMT,
    AST_STAT_BLOCK,

    // Expressions
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_FUNC_CALL_EXPR,
    AST_VARIABLE,
    AST_ARRAY_ACCESS,
    AST_MEMBER_ACCESS,
    AST_INT_LITERAL,
    AST_FLOAT_LITERAL,

    // Lists
    AST_CLASS_LIST,
    AST_INHERIT_LIST,
    AST_MEMBER_LIST,
    AST_PARAM_LIST,
    AST_ARG_LIST,
    AST_STMT_LIST,
    AST_VAR_DECL_LIST,
    AST_ARRAY_SIZE_LIST,

    // Identifiers and literals
    AST_ID,
    AST_VISIBILITY,

    // New grammar rule AST nodes
    AST_ASSIGN_OP,
    AST_IDNEST_LIST,
    AST_IDNEST,
    AST_SELF,
    AST_INDICE_LIST,
    AST_INDICE,
    AST_FPARAMS_TAIL_LIST,
    AST_APARAMS_TAIL_LIST
} ASTNodeType;

// Data types for semantic analysis
typedef enum
{
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_VOID,
    TYPE_CLASS,
    TYPE_ARRAY,
    TYPE_ERROR,
    TYPE_UNKNOWN
} DataType;

// Visibility types
typedef enum
{
    VIS_PUBLIC,
    VIS_PRIVATE
} Visibility;

// Symbol types
typedef enum
{
    SYM_CLASS,
    SYM_FUNC,
    SYM_VAR,
    SYM_PARAM,
    SYM_ATTR
} SymbolType;

// AST Node structure
struct ASTNode
{
    ASTNodeType type;
    int line;
    int column;

    // Node-specific data
    union
    {
        char *string_value;    // For identifiers, literals
        int int_value;         // For integer literals
        float float_value;     // For float literals
        DataType data_type;    // For type nodes
        Visibility visibility; // For visibility nodes
        char *operator;        // For binary/unary operators
    } data;

    // Semantic attributes
    DataType semantic_type; // Type computed during semantic analysis
    Symbol *symbol;         // Associated symbol table entry
    int array_dimensions;   // For array types
    int *array_sizes;       // Array dimension sizes

    // Children (flexible structure)
    ASTNode **children;
    int child_count;
    int child_capacity;

    // Sibling for linked lists
    ASTNode *next;
};

// Symbol structure
struct Symbol
{
    char *name;
    SymbolType type;
    DataType data_type;
    Visibility visibility;
    int line;
    int column;

    // Type-specific attributes
    union
    {
        struct
        {
            ASTNode *param_list;
            DataType return_type;
            SymbolTable *local_scope;
        } func;

        struct
        {
            SymbolTable *members;
            char **parent_classes;
            int parent_count;
        } class_info;

        struct
        {
            int dimensions;
            int *sizes;
            DataType element_type;
        } array;
    } attributes;

    // Scope information
    SymbolTable *scope;
    Symbol *next;

    // Warning system tracking
    int is_used;        // Track if symbol is used (for unused warnings)
    int is_initialized; // Track if variable is initialized (for uninitialized warnings)
};

// Symbol Table structure
struct SymbolTable
{
    char *scope_name;
    Symbol *symbols;
    SymbolTable *parent;
    SymbolTable *children;
    SymbolTable *next_sibling;
    int scope_level;
};

// Error structure
typedef struct SemanticError
{
    int line;
    int column;
    char *message;
    char *error_type;
    char *context;    // Source code context around the error
    char *suggestion; // Suggested fix for the error
    int severity;     // Error severity: 1=Info, 2=Warning, 3=Error, 4=Fatal
    struct SemanticError *next;
} SemanticError;

typedef struct SyntaxError
{
    int line;
    int column;
    char *message;
    char *error_type;
    char *expected;   // What was expected
    char *found;      // What was actually found
    char *context;    // Source code context around the error
    char *suggestion; // Suggested fix
    int severity;     // Error severity
    struct SyntaxError *next;
} SyntaxError;

// Global variables for semantic analysis
extern SymbolTable *global_scope;
extern SymbolTable *current_scope;
extern SemanticError *error_list;
extern SyntaxError *syntax_error_list;
extern int semantic_error_count;
extern int semantic_warning_count;
extern int syntax_error_count;
extern FILE *symbol_table_file;
extern FILE *semantic_error_file;
extern FILE *ast_file;
extern char *current_filename;

// AST creation functions
ASTNode *create_ast_node(ASTNodeType type, int line, int column);
ASTNode *create_string_node(ASTNodeType type, char *value, int line, int column);
ASTNode *create_int_node(int value, int line, int column);
ASTNode *create_float_node(float value, int line, int column);
ASTNode *create_type_node(DataType type, int line, int column);
ASTNode *create_binary_expr(char *operator, ASTNode * left, ASTNode *right, int line, int column);
ASTNode *create_unary_expr(char *operator, ASTNode * operand, int line, int column);

// AST manipulation functions
void add_child(ASTNode *parent, ASTNode *child);
void add_sibling(ASTNode *node, ASTNode *sibling);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);
void print_ast_to_file(ASTNode *node, FILE *file, int indent);
void write_comprehensive_ast_report(ASTNode *root, FILE *file);

// Symbol table functions
SymbolTable *create_symbol_table(char *name, SymbolTable *parent);
Symbol *create_symbol(char *name, SymbolType type, DataType data_type, int line, int column);
void insert_symbol(SymbolTable *table, Symbol *symbol);
Symbol *lookup_symbol(SymbolTable *table, char *name);
Symbol *lookup_symbol_global(SymbolTable *table, char *name);
void enter_scope(char *scope_name);
void exit_scope();
void print_symbol_table(SymbolTable *table, FILE *file, int indent);

// Semantic analysis functions
void semantic_analysis(ASTNode *root);
void init_semantic_analyzer(void);
void analyze_declarations(ASTNode *node);
void analyze_statements(ASTNode *node);
DataType analyze_expression(ASTNode *node);
void type_check(ASTNode *node);

// Error reporting functions
void report_semantic_error(int line, int col, const char *message, const char *error_type);
void report_semantic_error_detailed(int line, int col, const char *message, const char *error_type,
                                    const char *context, const char *suggestion);
void report_syntax_error_detailed(int line, int column, char *message, char *error_type,
                                  char *expected, char *found, char *context, char *suggestion);
void report_semantic_warning(int line, int column, char *message);
void print_semantic_errors(FILE *file);
void print_comprehensive_error_report(FILE *file);
char *extract_source_context(const char *filename, int line, int context_lines);

// Type compatibility functions
int types_compatible(DataType type1, DataType type2);
int types_compatible_with_conversion(DataType type1, DataType type2);
// Extended compatibility check that accepts AST nodes (when available)
// so we can compare array element types and class identities.
int types_compatible_extended(DataType type1, DataType type2, ASTNode *node1, ASTNode *node2);
DataType get_binary_result_type(DataType left, DataType right, char *operator);
DataType get_unary_result_type(DataType operand, char *operator);
char *type_to_string(DataType type);

// Utility functions
char *duplicate_string(const char *str);
void cleanup_semantic_analyzer();
int count_scopes(SymbolTable *table);
int count_symbols_recursive(SymbolTable *table);
int count_symbols_by_type_recursive(SymbolTable *table, SymbolType type);
int get_max_scope_depth(SymbolTable *table);

// Additional helper functions for semantic analysis
void analyze_inheritance(Symbol *class_symbol, ASTNode *inherit_list);
void analyze_implementation_definition(ASTNode *node);
void analyze_parameter_declaration(ASTNode *node);
void analyze_parameter_list(ASTNode *param_list);
void analyze_array_declaration(Symbol *symbol, ASTNode *array_sizes);
void analyze_scoped_node(ASTNode *node);
void analyze_if_statement(ASTNode *node);
void analyze_while_statement(ASTNode *node);
void analyze_simple_statement(ASTNode *node);
DataType get_data_type_from_node(ASTNode *node);
int is_assignable(ASTNode *node);
Visibility get_current_visibility();
int in_same_class(char *class1, char *class2);
void check_function_arguments(Symbol *func_symbol, ASTNode *arg_list, int line, int column);
char *get_ast_type_name(ASTNodeType type);
char *get_symbol_type_name(SymbolType type);

// AST statistics functions
int count_ast_nodes(ASTNode *node);
int get_ast_depth(ASTNode *node);
int count_leaf_nodes(ASTNode *node);
int count_semantic_nodes(ASTNode *node);
void print_node_type_statistics(ASTNode *node, FILE *file);
void count_node_types(ASTNode *node, int *type_counts);
int get_node_type_index(const char *type_name);

#endif // AST_H