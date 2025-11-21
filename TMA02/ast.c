#include "ast.h"

// Global variables
SymbolTable *global_scope = NULL;
SymbolTable *current_scope = NULL;
SemanticError *error_list = NULL;
int semantic_error_count = 0;
int semantic_warning_count = 0;
FILE *symbol_table_file = NULL;
FILE *semantic_error_file = NULL;

// AST creation functions
ASTNode *create_ast_node(ASTNodeType type, int line, int column)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Memory allocation failed for AST node\n");
        exit(1);
    }

    node->type = type;
    node->line = line;
    node->column = column;
    node->semantic_type = TYPE_UNKNOWN;
    node->symbol = NULL;
    node->array_dimensions = 0;
    node->array_sizes = NULL;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->next = NULL;

    // Initialize data union to zero
    memset(&node->data, 0, sizeof(node->data));

    return node;
}

ASTNode *create_string_node(ASTNodeType type, char *value, int line, int column)
{
    ASTNode *node = create_ast_node(type, line, column);
    node->data.string_value = duplicate_string(value);
    return node;
}

ASTNode *create_int_node(int value, int line, int column)
{
    ASTNode *node = create_ast_node(AST_INT_LITERAL, line, column);
    node->data.int_value = value;
    node->semantic_type = TYPE_INTEGER;
    return node;
}

ASTNode *create_float_node(float value, int line, int column)
{
    ASTNode *node = create_ast_node(AST_FLOAT_LITERAL, line, column);
    node->data.float_value = value;
    node->semantic_type = TYPE_FLOAT;
    return node;
}

ASTNode *create_type_node(DataType type, int line, int column)
{
    ASTNode *node = create_ast_node(AST_TYPE, line, column);
    node->data.data_type = type;
    node->semantic_type = type;
    return node;
}

ASTNode *create_binary_expr(char *operator, ASTNode *left, ASTNode *right, int line, int column)
{
    ASTNode *node = create_ast_node(AST_BINARY_EXPR, line, column);
    node->data.operator = duplicate_string(operator);
    add_child(node, left);
    add_child(node, right);
    return node;
}

ASTNode *create_unary_expr(char *operator, ASTNode *operand, int line, int column)
{
    ASTNode *node = create_ast_node(AST_UNARY_EXPR, line, column);
    node->data.operator = duplicate_string(operator);
    add_child(node, operand);
    return node;
}

// AST manipulation functions
void add_child(ASTNode *parent, ASTNode *child)
{
    if (!parent || !child)
        return;

    if (parent->child_count >= parent->child_capacity)
    {
        parent->child_capacity = parent->child_capacity == 0 ? 4 : parent->child_capacity * 2;
        parent->children = (ASTNode **)realloc(parent->children,
                                               parent->child_capacity * sizeof(ASTNode *));
        if (!parent->children)
        {
            fprintf(stderr, "Error: Memory allocation failed for AST children\n");
            exit(1);
        }
    }

    parent->children[parent->child_count++] = child;
}

void add_sibling(ASTNode *node, ASTNode *sibling)
{
    if (!node || !sibling)
        return;

    ASTNode *current = node;
    while (current->next)
    {
        current = current->next;
    }
    current->next = sibling;
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;

    // Free children
    for (int i = 0; i < node->child_count; i++)
    {
        free_ast(node->children[i]);
    }
    free(node->children);

    // Free next sibling
    free_ast(node->next);

    // Free node-specific data
    if (node->data.string_value)
    {
        free(node->data.string_value);
    }
    if (node->data.operator)
    {
        free(node->data.operator);
    }
    if (node->array_sizes)
    {
        free(node->array_sizes);
    }

    free(node);
}

void print_ast(ASTNode *node, int indent)
{
    if (!node)
        return;

    // Print indentation
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }

    // Print node type and data
    printf("AST_%s", get_ast_type_name(node->type));

    switch (node->type)
    {
    case AST_ID:
        printf(" (%s)", node->data.string_value);
        break;
    case AST_INT_LITERAL:
        printf(" (%d)", node->data.int_value);
        break;
    case AST_FLOAT_LITERAL:
        printf(" (%.2f)", node->data.float_value);
        break;
    case AST_TYPE:
        printf(" (%s)", type_to_string(node->data.data_type));
        break;
    case AST_BINARY_EXPR:
    case AST_UNARY_EXPR:
        printf(" (%s)", node->data.operator);
        break;
    default:
        break;
    }

    if (node->semantic_type != TYPE_UNKNOWN)
    {
        printf(" [type: %s]", type_to_string(node->semantic_type));
    }

    printf("\n");

    // Print children
    for (int i = 0; i < node->child_count; i++)
    {
        print_ast(node->children[i], indent + 1);
    }

    // Print siblings
    if (node->next)
    {
        print_ast(node->next, indent);
    }
}

// Symbol table functions
SymbolTable *create_symbol_table(char *name, SymbolTable *parent)
{
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!table)
    {
        fprintf(stderr, "Error: Memory allocation failed for symbol table\n");
        exit(1);
    }

    table->scope_name = duplicate_string(name);
    table->symbols = NULL;
    table->parent = parent;
    table->children = NULL;
    table->next_sibling = NULL;
    table->scope_level = parent ? parent->scope_level + 1 : 0;

    // Add to parent's children if parent exists
    if (parent)
    {
        if (!parent->children)
        {
            parent->children = table;
        }
        else
        {
            SymbolTable *sibling = parent->children;
            while (sibling->next_sibling)
            {
                sibling = sibling->next_sibling;
            }
            sibling->next_sibling = table;
        }
    }

    return table;
}

Symbol *create_symbol(char *name, SymbolType type, DataType data_type, int line, int column)
{
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    if (!symbol)
    {
        fprintf(stderr, "Error: Memory allocation failed for symbol\n");
        exit(1);
    }

    symbol->name = duplicate_string(name);
    symbol->type = type;
    symbol->data_type = data_type;
    symbol->visibility = VIS_PUBLIC;
    symbol->line = line;
    symbol->column = column;
    symbol->scope = current_scope;
    symbol->next = NULL;

    // Initialize type-specific attributes
    memset(&symbol->attributes, 0, sizeof(symbol->attributes));

    return symbol;
}

void insert_symbol(SymbolTable *table, Symbol *symbol)
{
    if (!table || !symbol)
        return;

    // Check for duplicate symbols in current scope
    Symbol *existing = table->symbols;
    while (existing)
    {
        if (strcmp(existing->name, symbol->name) == 0)
        {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg),
                     "Redeclaration of '%s' (previously declared at line %d)",
                     symbol->name, existing->line);
            report_semantic_error(symbol->line, symbol->column, error_msg, "REDECLARATION");
            return;
        }
        existing = existing->next;
    }

    // Insert at beginning of list
    symbol->next = table->symbols;
    table->symbols = symbol;
}

Symbol *lookup_symbol(SymbolTable *table, char *name)
{
    if (!table || !name)
        return NULL;

    Symbol *symbol = table->symbols;
    while (symbol)
    {
        if (strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
        symbol = symbol->next;
    }

    return NULL;
}

Symbol *lookup_symbol_global(SymbolTable *table, char *name)
{
    if (!table || !name)
        return NULL;

    // Search in current scope
    Symbol *symbol = lookup_symbol(table, name);
    if (symbol)
        return symbol;

    // Search in parent scopes
    if (table->parent)
    {
        return lookup_symbol_global(table->parent, name);
    }

    return NULL;
}

void enter_scope(char *scope_name)
{
    SymbolTable *new_scope = create_symbol_table(scope_name, current_scope);
    current_scope = new_scope;
}

void exit_scope()
{
    if (current_scope && current_scope->parent)
    {
        current_scope = current_scope->parent;
    }
}

void print_symbol_table(SymbolTable *table, FILE *file, int indent)
{
    if (!table || !file)
        return;

    // Print scope header
    for (int i = 0; i < indent; i++)
    {
        fprintf(file, "  ");
    }
    fprintf(file, "=== Scope: %s (Level %d) ===\n", table->scope_name, table->scope_level);

    // Print symbols
    Symbol *symbol = table->symbols;
    if (!symbol)
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "(no symbols)\n");
    }

    while (symbol)
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }

        fprintf(file, "%-15s | %-10s | %-10s | %-10s | Line %d\n",
                symbol->name,
                get_symbol_type_name(symbol->type),
                type_to_string(symbol->data_type),
                symbol->visibility == VIS_PUBLIC ? "public" : "private",
                symbol->line);

        // Print additional information for functions
        if (symbol->type == SYM_FUNC)
        {
            for (int i = 0; i < indent; i++)
            {
                fprintf(file, "  ");
            }
            fprintf(file, "    Return Type: %s\n", type_to_string(symbol->attributes.func.return_type));
        }

        // Print additional information for arrays
        if (symbol->data_type == TYPE_ARRAY)
        {
            for (int i = 0; i < indent; i++)
            {
                fprintf(file, "  ");
            }
            fprintf(file, "    Element Type: %s, Dimensions: %d\n",
                    type_to_string(symbol->attributes.array.element_type),
                    symbol->attributes.array.dimensions);
        }

        symbol = symbol->next;
    }

    // Print child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        fprintf(file, "\n");
        print_symbol_table(child, file, indent + 1);
        child = child->next_sibling;
    }
}

// Error reporting functions
void report_semantic_error(int line, int column, char *message, char *error_type)
{
    SemanticError *error = (SemanticError *)malloc(sizeof(SemanticError));
    if (!error)
    {
        fprintf(stderr, "Error: Memory allocation failed for semantic error\n");
        return;
    }

    error->line = line;
    error->column = column;
    error->message = duplicate_string(message);
    error->error_type = duplicate_string(error_type);
    error->next = NULL;

    // Insert in order by line number
    if (!error_list || error_list->line > line)
    {
        error->next = error_list;
        error_list = error;
    }
    else
    {
        SemanticError *current = error_list;
        while (current->next && current->next->line <= line)
        {
            current = current->next;
        }
        error->next = current->next;
        current->next = error;
    }

    semantic_error_count++;

    // Also print to stderr for immediate feedback
    fprintf(stderr, "Semantic Error at line %d, column %d: %s\n", line, column, message);
}

void report_semantic_warning(int line, int column, char *message)
{
    semantic_warning_count++;
    fprintf(stderr, "Semantic Warning at line %d, column %d: %s\n", line, column, message);

    if (semantic_error_file)
    {
        fprintf(semantic_error_file, "WARNING at line %d, column %d: %s\n", line, column, message);
    }
}

void print_semantic_errors(FILE *file)
{
    if (!file)
        return;

    fprintf(file, "=== Semantic Analysis Report ===\n");
    fprintf(file, "Total Errors: %d\n", semantic_error_count);
    fprintf(file, "Total Warnings: %d\n\n", semantic_warning_count);

    if (semantic_error_count == 0)
    {
        fprintf(file, "No semantic errors found.\n");
        return;
    }

    SemanticError *error = error_list;
    while (error)
    {
        fprintf(file, "%s at line %d, column %d: %s\n",
                error->error_type, error->line, error->column, error->message);
        error = error->next;
    }
}

// Type compatibility functions
int types_compatible(DataType type1, DataType type2)
{
    if (type1 == type2)
        return 1;
    if (type1 == TYPE_ERROR || type2 == TYPE_ERROR)
        return 1;
    if (type1 == TYPE_UNKNOWN || type2 == TYPE_UNKNOWN)
        return 1;

    // Integer and float are compatible for some operations
    if ((type1 == TYPE_INTEGER && type2 == TYPE_FLOAT) ||
        (type1 == TYPE_FLOAT && type2 == TYPE_INTEGER))
    {
        return 1;
    }

    return 0;
}

DataType get_binary_result_type(DataType left, DataType right, char *operator)
{
    if (left == TYPE_ERROR || right == TYPE_ERROR)
    {
        return TYPE_ERROR;
    }

    // Arithmetic operators
    if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0 ||
        strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0)
    {

        if (!types_compatible(left, right))
        {
            return TYPE_ERROR;
        }

        if (left == TYPE_FLOAT || right == TYPE_FLOAT)
        {
            return TYPE_FLOAT;
        }

        if (left == TYPE_INTEGER && right == TYPE_INTEGER)
        {
            return TYPE_INTEGER;
        }
    }

    // Relational operators
    if (strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0 ||
        strcmp(operator, "<") == 0 || strcmp(operator, ">") == 0 ||
        strcmp(operator, "<=") == 0 || strcmp(operator, ">=") == 0)
    {

        if (types_compatible(left, right))
        {
            return TYPE_INTEGER; // Boolean result represented as integer
        }
    }

    // Logical operators
    if (strcmp(operator, "and") == 0 || strcmp(operator, "or") == 0)
    {
        if (left == TYPE_INTEGER && right == TYPE_INTEGER)
        {
            return TYPE_INTEGER;
        }
    }

    return TYPE_ERROR;
}

DataType get_unary_result_type(DataType operand, char *operator)
{
    if (operand == TYPE_ERROR)
    {
        return TYPE_ERROR;
    }

    if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0)
    {
        if (operand == TYPE_INTEGER || operand == TYPE_FLOAT)
        {
            return operand;
        }
    }

    if (strcmp(operator, "not") == 0)
    {
        if (operand == TYPE_INTEGER)
        {
            return TYPE_INTEGER;
        }
    }

    return TYPE_ERROR;
}

char *type_to_string(DataType type)
{
    switch (type)
    {
    case TYPE_INTEGER:
        return "integer";
    case TYPE_FLOAT:
        return "float";
    case TYPE_VOID:
        return "void";
    case TYPE_CLASS:
        return "class";
    case TYPE_ARRAY:
        return "array";
    case TYPE_ERROR:
        return "error";
    case TYPE_UNKNOWN:
        return "unknown";
    default:
        return "undefined";
    }
}

// Utility functions
char *duplicate_string(const char *str)
{
    if (!str)
        return NULL;

    char *copy = (char *)malloc(strlen(str) + 1);
    if (!copy)
    {
        fprintf(stderr, "Error: Memory allocation failed for string duplication\n");
        exit(1);
    }

    strcpy(copy, str);
    return copy;
}

char *get_ast_type_name(ASTNodeType type)
{
    switch (type)
    {
    case AST_PROGRAM:
        return "PROGRAM";
    case AST_CLASS_DECL:
        return "CLASS_DECL";
    case AST_IMPL_DEF:
        return "IMPL_DEF";
    case AST_FUNC_DEF:
        return "FUNC_DEF";
    case AST_FUNC_DECL:
        return "FUNC_DECL";
    case AST_FUNC_HEAD:
        return "FUNC_HEAD";
    case AST_FUNC_BODY:
        return "FUNC_BODY";
    case AST_VAR_DECL:
        return "VAR_DECL";
    case AST_ATTR_DECL:
        return "ATTR_DECL";
    case AST_LOCAL_VAR_DECL:
        return "LOCAL_VAR_DECL";
    case AST_PARAM_DECL:
        return "PARAM_DECL";
    case AST_TYPE:
        return "TYPE";
    case AST_ARRAY_SIZE:
        return "ARRAY_SIZE";
    case AST_ASSIGN_STMT:
        return "ASSIGN_STMT";
    case AST_IF_STMT:
        return "IF_STMT";
    case AST_WHILE_STMT:
        return "WHILE_STMT";
    case AST_READ_STMT:
        return "READ_STMT";
    case AST_WRITE_STMT:
        return "WRITE_STMT";
    case AST_RETURN_STMT:
        return "RETURN_STMT";
    case AST_FUNC_CALL_STMT:
        return "FUNC_CALL_STMT";
    case AST_STAT_BLOCK:
        return "STAT_BLOCK";
    case AST_BINARY_EXPR:
        return "BINARY_EXPR";
    case AST_UNARY_EXPR:
        return "UNARY_EXPR";
    case AST_FUNC_CALL_EXPR:
        return "FUNC_CALL_EXPR";
    case AST_VARIABLE:
        return "VARIABLE";
    case AST_ARRAY_ACCESS:
        return "ARRAY_ACCESS";
    case AST_MEMBER_ACCESS:
        return "MEMBER_ACCESS";
    case AST_INT_LITERAL:
        return "INT_LITERAL";
    case AST_FLOAT_LITERAL:
        return "FLOAT_LITERAL";
    case AST_ID:
        return "ID";
    case AST_VISIBILITY:
        return "VISIBILITY";
    default:
        return "UNKNOWN";
    }
}

char *get_symbol_type_name(SymbolType type)
{
    switch (type)
    {
    case SYM_CLASS:
        return "CLASS";
    case SYM_FUNC:
        return "FUNCTION";
    case SYM_VAR:
        return "VARIABLE";
    case SYM_PARAM:
        return "PARAMETER";
    case SYM_ATTR:
        return "ATTRIBUTE";
    default:
        return "UNKNOWN";
    }
}

void cleanup_semantic_analyzer()
{
    // Free error list
    SemanticError *error = error_list;
    while (error)
    {
        SemanticError *next = error->next;
        free(error->message);
        free(error->error_type);
        free(error);
        error = next;
    }
    error_list = NULL;

    // Close files
    if (symbol_table_file && symbol_table_file != stdout)
    {
        fclose(symbol_table_file);
    }
    if (semantic_error_file && semantic_error_file != stderr)
    {
        fclose(semantic_error_file);
    }
}