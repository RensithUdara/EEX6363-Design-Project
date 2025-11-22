#include "../../include/core/ast.h"
#include "../../include/utils/error_handler.h"

// Global variables
SymbolTable *global_scope = NULL;
SymbolTable *current_scope = NULL;
SemanticError *error_list = NULL;
SyntaxError *syntax_error_list = NULL;
int semantic_error_count = 0;
int semantic_warning_count = 0;
int syntax_error_count = 0;
FILE *symbol_table_file = NULL;
FILE *semantic_error_file = NULL;
FILE *ast_file = NULL;

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

void print_ast_to_file(ASTNode *node, FILE *file, int indent)
{
    if (!node || !file)
        return;

    // Print indentation
    for (int i = 0; i < indent; i++)
    {
        fprintf(file, "  ");
    }

    // Print node type and data
    fprintf(file, "AST_%s", get_ast_type_name(node->type));

    switch (node->type)
    {
    case AST_ID:
        fprintf(file, " (%s)", node->data.string_value);
        break;
    case AST_INT_LITERAL:
        fprintf(file, " (%d)", node->data.int_value);
        break;
    case AST_FLOAT_LITERAL:
        fprintf(file, " (%.2f)", node->data.float_value);
        break;
    case AST_TYPE:
        fprintf(file, " (%s)", type_to_string(node->data.data_type));
        break;
    case AST_BINARY_EXPR:
    case AST_UNARY_EXPR:
        fprintf(file, " (%s)", node->data.operator);
        break;
    default:
        break;
    }

    if (node->semantic_type != TYPE_UNKNOWN)
    {
        fprintf(file, " [type: %s]", type_to_string(node->semantic_type));
    }

    fprintf(file, "\n");

    // Print children
    for (int i = 0; i < node->child_count; i++)
    {
        print_ast_to_file(node->children[i], file, indent + 1);
    }

    // Print siblings
    if (node->next)
    {
        print_ast_to_file(node->next, file, indent);
    }
}

void write_comprehensive_ast_report(ASTNode *root, FILE *file)
{
    if (!file)
        return;

    // Header
    fprintf(file, "================================================================================\n");
    fprintf(file, "                      TMA03 ABSTRACT SYNTAX TREE REPORT                        \n");
    fprintf(file, "================================================================================\n");
    fprintf(file, "Features: Hierarchical AST structure with semantic type annotations\n");
    fprintf(file, "Parser Type: Recursive descent parser with predictive parsing grammar\n");
    fprintf(file, "================================================================================\n\n");

    if (!root)
    {
        // Generate comprehensive error report in same format as semantic error files
        int syntax_errors = get_syntax_error_count();
        int semantic_errors = get_semantic_error_count();
        int warning_count = 0; // TODO: Add warning count if available
        (void)syntax_errors;
        (void)semantic_errors;
        (void)warning_count; // Suppress unused warnings

        fprintf(file, "================================================================================\n");
        fprintf(file, "                    TMA03 COMPREHENSIVE ERROR REPORT                           \n");
        fprintf(file, "================================================================================\n");
        fprintf(file, "Features: Detailed error context, suggestions, and comprehensive diagnostics\n");
        fprintf(file, "================================================================================\n\n");

        fprintf(file, "COMPILATION SUMMARY:\n");
        fprintf(file, "┌─────────────────────────────────────────────────────────────────────────────┐\n");
        fprintf(file, "│ Error Type           │ Count │ Severity │ Status                           │\n");
        fprintf(file, "├─────────────────────────────────────────────────────────────────────────────┤\n");
        fprintf(file, "│ Syntax Errors        │ %5d │   HIGH   │ %s │\n",
                syntax_errors, syntax_errors > 0 ? "✗ FAILED                     " : "✓ PASSED                     ");
        fprintf(file, "│ Semantic Errors      │ %5d │   HIGH   │ %s │\n",
                semantic_errors, semantic_errors > 0 ? "✗ FAILED                     " : "✓ PASSED                     ");
        fprintf(file, "│ Semantic Warnings    │ %5d │   LOW    │ %s │\n",
                warning_count, warning_count > 0 ? "⚠ WARNINGS                  " : "✓ CLEAN                      ");
        fprintf(file, "└─────────────────────────────────────────────────────────────────────────────┘\n\n");

        // Print syntax errors if any
        if (syntax_errors > 0 && global_error_manager && global_error_manager->syntax_errors)
        {
            fprintf(file, "================================================================================\n");
            fprintf(file, "                              SYNTAX ERRORS                                    \n");
            fprintf(file, "================================================================================\n\n");

            ComprehensiveError *error = global_error_manager->syntax_errors;
            int error_num = 1;
            while (error)
            {
                fprintf(file, "❌ SYNTAX ERROR #%d\n", error_num++);
                fprintf(file, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
                fprintf(file, "   ├─ Type: %s\n", error->error_code ? error->error_code : "SYNTAX_ERROR");
                fprintf(file, "   ├─ Description: %s\n", error->message);

                // Show source context if we can extract it
                if (error->filename && error->line > 0)
                {
                    fprintf(file, "   ├─ Source Context:\n");
                    char *source_line = extract_source_line(error->filename, error->line);
                    if (source_line)
                    {
                        // Show context lines
                        for (int i = -2; i <= 2; i++)
                        {
                            if (error->line + i > 0)
                            {
                                char *context_line = extract_source_line(error->filename, error->line + i);
                                if (context_line)
                                {
                                    if (i == 0)
                                    {
                                        fprintf(file, "   │    >>  %3d: %s\n", error->line + i, context_line);
                                    }
                                    else
                                    {
                                        fprintf(file, "   │        %3d: %s\n", error->line + i, context_line);
                                    }
                                    free(context_line);
                                }
                            }
                        }
                        free(source_line);
                    }
                }

                if (error->suggestion)
                {
                    fprintf(file, "   └─ 💡 Suggestion: %s\n", error->suggestion);
                }
                fprintf(file, "\n");
                error = error->next;
            }
        }

        // Print semantic errors if any
        if (semantic_errors > 0 && global_error_manager && global_error_manager->semantic_errors)
        {
            fprintf(file, "================================================================================\n");
            fprintf(file, "                             SEMANTIC ERRORS                                   \n");
            fprintf(file, "================================================================================\n\n");

            ComprehensiveError *error = global_error_manager->semantic_errors;
            int error_num = 1;
            while (error)
            {
                fprintf(file, "❌ SEMANTIC ERROR #%d\n", error_num++);
                fprintf(file, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
                fprintf(file, "   ├─ Type: %s\n", error->error_code ? error->error_code : "SEMANTIC_ERROR");
                fprintf(file, "   ├─ Description: %s\n", error->message);

                if (error->context)
                {
                    fprintf(file, "   ├─ Context: %s\n", error->context);
                }

                if (error->suggestion)
                {
                    fprintf(file, "   └─ 💡 Suggestion: %s\n", error->suggestion);
                }
                fprintf(file, "\n");
                error = error->next;
            }
        }

        fprintf(file, "================================================================================\n");
        fprintf(file, "                            RESOLUTION GUIDE                                   \n");
        fprintf(file, "================================================================================\n\n");

        if (syntax_errors > 0)
        {
            fprintf(file, "🔧 SYNTAX ERROR RESOLUTION:\n");
            fprintf(file, "   1. Fix syntax errors first - they prevent proper parsing\n");
            fprintf(file, "   2. Check for missing semicolons, braces, or parentheses\n");
            fprintf(file, "   3. Verify proper token spelling and casing\n");
            fprintf(file, "   4. Ensure operators and literals follow language rules\n\n");
        }

        if (semantic_errors > 0)
        {
            fprintf(file, "🔍 SEMANTIC ERROR RESOLUTION:\n");
            fprintf(file, "   1. Verify variable declarations before use\n");
            fprintf(file, "   2. Check type compatibility in assignments\n");
            fprintf(file, "   3. Ensure function calls match signatures\n");
            fprintf(file, "   4. Validate scope and visibility rules\n\n");
        }

        fprintf(file, "📚 ADDITIONAL RESOURCES:\n");
        fprintf(file, "   • Language Grammar: Check semantic.c for syntax rules and grammar implementation\n");
        fprintf(file, "   • Semantic Rules: Review TMA03 specification document\n");
        fprintf(file, "   • Test Cases: Examine valid*.txt files for examples\n\n");

        fprintf(file, "================================================================================\n");
        return;
    }

    // AST Statistics
    int total_nodes = count_ast_nodes(root);
    int max_depth = get_ast_depth(root);
    int leaf_nodes = count_leaf_nodes(root);
    int semantic_nodes = count_semantic_nodes(root);

    fprintf(file, "AST STRUCTURE OVERVIEW:\n");
    fprintf(file, "┌─────────────────────────────────────────────────────────────────────────────┐\n");
    fprintf(file, "│ Metric                │ Value │ Description                                 │\n");
    fprintf(file, "├─────────────────────────────────────────────────────────────────────────────┤\n");
    fprintf(file, "│ Total Nodes           │ %5d │ Complete count of all AST nodes             │\n", total_nodes);
    fprintf(file, "│ Maximum Depth         │ %5d │ Deepest nesting level in the tree          │\n", max_depth);
    fprintf(file, "│ Leaf Nodes            │ %5d │ Terminal nodes (identifiers, literals)     │\n", leaf_nodes);
    fprintf(file, "│ Semantically Typed    │ %5d │ Nodes with semantic type information       │\n", semantic_nodes);
    fprintf(file, "│ Coverage Ratio        │ %4.1f%% │ Percentage of nodes with semantic types    │\n",
            total_nodes > 0 ? (semantic_nodes * 100.0 / total_nodes) : 0.0);
    fprintf(file, "└─────────────────────────────────────────────────────────────────────────────┘\n\n");

    // Node type distribution
    fprintf(file, "NODE TYPE DISTRIBUTION:\n");
    print_node_type_statistics(root, file);
    fprintf(file, "\n");

    // Full AST Structure
    fprintf(file, "COMPLETE AST STRUCTURE:\n");
    fprintf(file, "────────────────────────────────────────────────────────────────────────────\n");
    fprintf(file, "Legend: AST_NODE_TYPE (value) [semantic_type]\n");
    fprintf(file, "├─ Child nodes are indented with \"  \"\n");
    fprintf(file, "├─ Sibling nodes appear at the same indentation level\n");
    fprintf(file, "└─ Semantic types are shown in brackets when available\n");
    fprintf(file, "────────────────────────────────────────────────────────────────────────────\n\n");

    print_ast_to_file(root, file, 0);

    fprintf(file, "\n================================================================================\n");
    fprintf(file, "                              AST ANALYSIS NOTES                               \n");
    fprintf(file, "================================================================================\n");
    fprintf(file, "🌳 Tree Structure: The AST represents the hierarchical structure of your program\n");
    fprintf(file, "🏷️  Node Types: Each node represents a syntactic construct (function, expression, etc.)\n");
    fprintf(file, "🔤 Values: Terminal nodes show actual values (identifiers, numbers, operators)\n");
    fprintf(file, "🏷️  Semantic Types: Type annotations added during semantic analysis phase\n");
    fprintf(file, "📊 Statistics: Use metrics above to understand program complexity\n");
    fprintf(file, "\n💡 Tips for Reading the AST:\n");
    fprintf(file, "   • Start from AST_PROGRAM (root) and follow the tree structure\n");
    fprintf(file, "   • Each indentation level represents a parent-child relationship\n");
    fprintf(file, "   • Nodes at the same level are siblings in the parse tree\n");
    fprintf(file, "   • Look for semantic types to understand inferred/checked types\n");
    fprintf(file, "================================================================================\n");
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

    // Initialize warning tracking fields
    symbol->is_used = 0;
    symbol->is_initialized = 0;

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

    // Print header for global scope
    if (indent == 0)
    {
        fprintf(file, "================================================================================\n");
        fprintf(file, "                    TMA03 COMPREHENSIVE SYMBOL TABLE                           \n");
        fprintf(file, "================================================================================\n");
        fprintf(file, "Description: Complete hierarchical scope structure with detailed symbol info\n");
        fprintf(file, "Features: Types, visibility, parameters, inheritance, array dimensions\n");
        fprintf(file, "================================================================================\n\n");
    }

    // Print scope header with enhanced formatting
    for (int i = 0; i < indent; i++)
    {
        fprintf(file, "  ");
    }
    fprintf(file, "┌─── SCOPE: %s (Level %d) ", table->scope_name, table->scope_level);

    // Count symbols in this scope
    int symbol_count = 0;
    Symbol *temp = table->symbols;
    while (temp)
    {
        symbol_count++;
        temp = temp->next;
    }
    fprintf(file, "─── [%d symbols] ", symbol_count);

    // Add dashes to complete the line
    int name_len = (int)strlen(table->scope_name);
    for (int i = 0; i < 40 - name_len; i++)
    {
        fprintf(file, "─");
    }
    fprintf(file, "┐\n");

    // Print symbols with enhanced formatting
    Symbol *symbol = table->symbols;
    if (!symbol)
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "│   ⚬ (no symbols declared in this scope)\n");
    }
    else
    {
        // Print table header
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "│\n");
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "│   %-18s │ %-12s │ %-12s │ %-10s │ %-8s │ Details\n",
                "Symbol Name", "Symbol Type", "Data Type", "Visibility", "Location");
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "│   ──────────────────┼──────────────┼──────────────┼────────────┼──────────┼────────────\n");
    }

    while (symbol)
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }

        fprintf(file, "│   %-18s │ %-12s │ %-12s │ %-10s │ Line %-3d │ ",
                symbol->name,
                get_symbol_type_name(symbol->type),
                type_to_string(symbol->data_type),
                symbol->visibility == VIS_PUBLIC ? "public" : "private",
                symbol->line);

        // Print detailed information based on symbol type
        switch (symbol->type)
        {
        case SYM_FUNC:
            fprintf(file, "Returns: %s", type_to_string(symbol->attributes.func.return_type));
            if (symbol->attributes.func.param_list)
            {
                fprintf(file, ", Has parameters");
            }
            else
            {
                fprintf(file, ", No parameters");
            }
            break;

        case SYM_CLASS:
            fprintf(file, "Class definition");
            if (symbol->attributes.class_info.parent_count > 0)
            {
                fprintf(file, ", Inherits from %d parent(s)", symbol->attributes.class_info.parent_count);
            }
            break;

        case SYM_VAR:
        case SYM_PARAM:
        case SYM_ATTR:
            if (symbol->data_type == TYPE_ARRAY)
            {
                fprintf(file, "Array[%dD], Element: %s",
                        symbol->attributes.array.dimensions,
                        type_to_string(symbol->attributes.array.element_type));
            }
            else
            {
                fprintf(file, "Simple %s", type_to_string(symbol->data_type));
            }
            break;

        default:
            fprintf(file, "Standard symbol");
            break;
        }

        fprintf(file, "\n");

        // Print additional detailed information for complex symbols
        if (symbol->type == SYM_FUNC && symbol->attributes.func.param_list)
        {
            for (int i = 0; i < indent; i++)
            {
                fprintf(file, "  ");
            }
            fprintf(file, "│   ⮕ Function Parameters: ");

            // Print parameter information
            ASTNode *param = symbol->attributes.func.param_list;
            int param_count = 0;
            while (param)
            {
                if (param_count > 0)
                    fprintf(file, ", ");
                if (param->type == AST_PARAM_DECL && param->child_count > 0)
                {
                    if (param->children[0] && param->children[0]->type == AST_ID)
                    {
                        fprintf(file, "%s", param->children[0]->data.string_value);
                        if (param->child_count > 1)
                        {
                            DataType param_type = get_data_type_from_node(param->children[1]);
                            fprintf(file, ":%s", type_to_string(param_type));
                        }
                    }
                }
                param = param->next;
                param_count++;
            }
            fprintf(file, "\n");
        }

        if (symbol->type == SYM_CLASS && symbol->attributes.class_info.parent_count > 0)
        {
            for (int i = 0; i < indent; i++)
            {
                fprintf(file, "  ");
            }
            fprintf(file, "│   ⮕ Inheritance: ");
            for (int j = 0; j < symbol->attributes.class_info.parent_count; j++)
            {
                if (j > 0)
                    fprintf(file, ", ");
                fprintf(file, "%s", symbol->attributes.class_info.parent_classes[j]);
            }
            fprintf(file, "\n");
        }

        if (symbol->data_type == TYPE_ARRAY)
        {
            for (int i = 0; i < indent; i++)
            {
                fprintf(file, "  ");
            }
            fprintf(file, "│   ⮕ Array Info: %dD array, Element type: %s",
                    symbol->attributes.array.dimensions,
                    type_to_string(symbol->attributes.array.element_type));
            if (symbol->attributes.array.sizes)
            {
                fprintf(file, ", Sizes: [");
                for (int j = 0; j < symbol->attributes.array.dimensions; j++)
                {
                    if (j > 0)
                        fprintf(file, ", ");
                    if (symbol->attributes.array.sizes[j] == -1)
                    {
                        fprintf(file, "dynamic");
                    }
                    else
                    {
                        fprintf(file, "%d", symbol->attributes.array.sizes[j]);
                    }
                }
                fprintf(file, "]");
            }
            fprintf(file, "\n");
        }

        symbol = symbol->next;
    }

    // Close the scope box
    if (table->symbols)
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "└");
        for (int j = 0; j < 80; j++)
        {
            fprintf(file, "─");
        }
        fprintf(file, "┘\n");
    }
    else
    {
        for (int i = 0; i < indent; i++)
        {
            fprintf(file, "  ");
        }
        fprintf(file, "└─────────────────────────────────────────────────────────────────────────┘\n");
    }

    // Print child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        fprintf(file, "\n");
        print_symbol_table(child, file, indent + 1);
        child = child->next_sibling;
    }

    // Print summary for global scope
    if (indent == 0)
    {
        fprintf(file, "\n");
        fprintf(file, "================================================================================\n");
        fprintf(file, "                              SYMBOL TABLE SUMMARY                             \n");
        fprintf(file, "================================================================================\n");

        // Count totals
        int total_scopes = count_scopes(table);
        int total_symbols = count_symbols_recursive(table);
        int total_functions = count_symbols_by_type_recursive(table, SYM_FUNC);
        int total_classes = count_symbols_by_type_recursive(table, SYM_CLASS);
        int total_variables = count_symbols_by_type_recursive(table, SYM_VAR) +
                              count_symbols_by_type_recursive(table, SYM_PARAM) +
                              count_symbols_by_type_recursive(table, SYM_ATTR);

        fprintf(file, "Total Scopes:       %d\n", total_scopes);
        fprintf(file, "Total Symbols:      %d\n", total_symbols);
        fprintf(file, "  ├─ Functions:     %d\n", total_functions);
        fprintf(file, "  ├─ Classes:       %d\n", total_classes);
        fprintf(file, "  └─ Variables:     %d (includes parameters and attributes)\n", total_variables);
        fprintf(file, "\nScope Hierarchy:    %d levels deep\n", get_max_scope_depth(table));
        fprintf(file, "Analysis Status:    Two-phase semantic analysis completed\n");
        fprintf(file, "Parser Type:        Recursive descent parser with syntax-directed translation\n");
        fprintf(file, "================================================================================\n");
    }
}

char *extract_source_context(const char *filename, int line, int context_lines)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;

    char **lines = malloc(sizeof(char *) * 1000); // Assume max 1000 lines
    int line_count = 0;
    char buffer[1024];

    // Read all lines
    while (fgets(buffer, sizeof(buffer), file) && line_count < 1000)
    {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;
        lines[line_count] = duplicate_string(buffer);
        line_count++;
    }
    fclose(file);

    if (line <= 0 || line > line_count)
    {
        // Cleanup
        for (int i = 0; i < line_count; i++)
        {
            free(lines[i]);
        }
        free(lines);
        return NULL;
    }

    // Calculate context range
    int start = (line - context_lines - 1 < 0) ? 0 : line - context_lines - 1;
    int end = (line + context_lines > line_count) ? line_count : line + context_lines;

    // Build context string
    char *context = malloc(4096); // Assume max 4KB context
    context[0] = '\0';

    for (int i = start; i < end; i++)
    {
        char line_str[20];
        sprintf(line_str, "%4d", i + 1);

        if (i == line - 1)
        {
            // Highlight the error line
            strcat(context, " >> ");
        }
        else
        {
            strcat(context, "    ");
        }
        strcat(context, line_str);
        strcat(context, ": ");
        strcat(context, lines[i]);
        strcat(context, "\n");
    }

    // Cleanup
    for (int i = 0; i < line_count; i++)
    {
        free(lines[i]);
    }
    free(lines);

    return context;
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

    fprintf(file, "=== TMA03 SEMANTIC ANALYSIS ERROR REPORT ===\n");
    fprintf(file, "Generated by two-phase semantic analyzer with comprehensive\n");
    fprintf(file, "type checking, scope validation, and semantic error recovery.\n\n");

    fprintf(file, "Analysis Summary:\n");
    fprintf(file, "- Total Errors: %d\n", semantic_error_count);
    fprintf(file, "- Total Warnings: %d\n", semantic_warning_count);
    fprintf(file, "- Analysis Status: %s\n\n",
            semantic_error_count == 0 ? "PASSED" : "FAILED");

    if (semantic_error_count == 0)
    {
        fprintf(file, "✓ No semantic errors found. All semantic checks passed successfully!\n");
        fprintf(file, "The program is semantically correct and ready for code generation.\n");
        return;
    }

    fprintf(file, "Detailed Error Listing:\n");
    fprintf(file, "%-20s %-10s %-50s\n", "Error Type", "Location", "Description");
    fprintf(file, "%-20s %-10s %-50s\n", "===================", "=========", "==================================================");

    SemanticError *error = error_list;
    while (error)
    {
        fprintf(file, "%-20s (%3d,%3d)   %s\n",
                error->error_type, error->line, error->column, error->message);
        error = error->next;
    }

    fprintf(file, "\nRecommendation: Fix the above semantic errors before proceeding to code generation.\n");
}

void print_comprehensive_error_report(FILE *file)
{
    if (!file)
        return;

    // Header
    fprintf(file, "================================================================================\n");
    fprintf(file, "                    TMA03 COMPREHENSIVE ERROR REPORT                           \n");
    fprintf(file, "================================================================================\n");
    fprintf(file, "Generated by: Recursive Descent Parser with Two-Phase Semantic Analysis\n");
    fprintf(file, "Features: Detailed error context, suggestions, and comprehensive diagnostics\n");
    fprintf(file, "================================================================================\n\n");

    // Summary - Get actual error counts from global error manager
    int actual_syntax_errors = get_syntax_error_count();
    int actual_semantic_errors = get_semantic_error_count();
    int actual_warnings = semantic_warning_count; // Keep using local for warnings for now

    fprintf(file, "COMPILATION SUMMARY:\n");
    fprintf(file, "┌─────────────────────────────────────────────────────────────────────────────┐\n");
    fprintf(file, "│ Error Type           │ Count │ Severity │ Status                           │\n");
    fprintf(file, "├─────────────────────────────────────────────────────────────────────────────┤\n");
    fprintf(file, "│ Syntax Errors        │ %5d │   HIGH   │ %s │\n",
            actual_syntax_errors, actual_syntax_errors == 0 ? "✓ PASSED                    " : "✗ FAILED                    ");
    fprintf(file, "│ Semantic Errors      │ %5d │   HIGH   │ %s │\n",
            actual_semantic_errors, actual_semantic_errors == 0 ? "✓ PASSED                    " : "✗ FAILED                    ");
    fprintf(file, "│ Semantic Warnings    │ %5d │   LOW    │ %s │\n",
            actual_warnings, actual_warnings == 0 ? "✓ CLEAN                     " : "⚠ REVIEW RECOMMENDED        ");
    fprintf(file, "└─────────────────────────────────────────────────────────────────────────────┘\n\n");

    int total_issues = actual_syntax_errors + actual_semantic_errors + actual_warnings;
    if (total_issues == 0)
    {
        fprintf(file, "🎉 CONGRATULATIONS! No issues found.\n");
        fprintf(file, "   Your code is syntactically correct and semantically sound.\n");
        fprintf(file, "   Ready for code generation and execution.\n\n");
        return;
    }

    // Detailed syntax errors
    if (syntax_error_count > 0)
    {
        fprintf(file, "================================================================================\n");
        fprintf(file, "                              SYNTAX ERRORS                                    \n");
        fprintf(file, "================================================================================\n\n");

        SyntaxError *error = syntax_error_list;
        int error_num = 1;
        while (error)
        {
            fprintf(file, "❌ SYNTAX ERROR #%d\n", error_num++);
            fprintf(file, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            fprintf(file, "   ├─ Type: %s\n", error->error_type ? error->error_type : "SYNTAX_ERROR");
            fprintf(file, "   ├─ Description: %s\n", error->message);

            if (error->expected && error->found)
            {
                fprintf(file, "   ├─ Expected: %s\n", error->expected);
                fprintf(file, "   ├─ Found: %s\n", error->found);
            }

            if (error->context)
            {
                fprintf(file, "   ├─ Source Context:\n");
                // Indent each line of context
                char *ctx = duplicate_string(error->context);
                char *line = strtok(ctx, "\n");
                while (line)
                {
                    fprintf(file, "   │   %s\n", line);
                    line = strtok(NULL, "\n");
                }
                free(ctx);
            }

            if (error->suggestion)
            {
                fprintf(file, "   └─ 💡 Suggestion: %s\n", error->suggestion);
            }
            else
            {
                fprintf(file, "   └─ 💡 Suggestion: Check syntax around the indicated location\n");
            }
            fprintf(file, "\n");
            error = error->next;
        }
    }

    // Detailed semantic errors
    if (actual_semantic_errors > 0)
    {
        fprintf(file, "================================================================================\n");
        fprintf(file, "                             SEMANTIC ERRORS                                   \n");
        fprintf(file, "================================================================================\n\n");

        SemanticError *error = error_list;
        int error_num = 1;
        while (error)
        {
            char severity_str[20];
            char icon[10];

            switch (error->severity)
            {
            case 1:
                strcpy(severity_str, "INFO");
                strcpy(icon, "ℹ️");
                break;
            case 2:
                strcpy(severity_str, "WARNING");
                strcpy(icon, "⚠️");
                break;
            case 3:
                strcpy(severity_str, "ERROR");
                strcpy(icon, "❌");
                break;
            case 4:
                strcpy(severity_str, "FATAL");
                strcpy(icon, "💥");
                break;
            default:
                strcpy(severity_str, "ERROR");
                strcpy(icon, "❌");
                break;
            }

            fprintf(file, "%s SEMANTIC %s #%d\n", icon, severity_str, error_num++);
            fprintf(file, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            fprintf(file, "   ├─ Type: %s\n", error->error_type ? error->error_type : "SEMANTIC_ERROR");
            fprintf(file, "   ├─ Description: %s\n", error->message);

            if (error->context)
            {
                fprintf(file, "   ├─ Source Context:\n");
                char *ctx = duplicate_string(error->context);
                char *line = strtok(ctx, "\n");
                while (line)
                {
                    fprintf(file, "   │   %s\n", line);
                    line = strtok(NULL, "\n");
                }
                free(ctx);
            }

            if (error->suggestion)
            {
                fprintf(file, "   └─ 💡 Suggestion: %s\n", error->suggestion);
            }
            else
            {
                fprintf(file, "   └─ 💡 Suggestion: Review the semantic rules for this construct\n");
            }
            fprintf(file, "\n");
            error = error->next;
        }
    }

    // Resolution guide
    fprintf(file, "================================================================================\n");
    fprintf(file, "                            RESOLUTION GUIDE                                   \n");
    fprintf(file, "================================================================================\n");

    if (actual_syntax_errors > 0)
    {
        fprintf(file, "\n🔧 SYNTAX ERROR RESOLUTION:\n");
        fprintf(file, "   1. Fix syntax errors first - they prevent proper parsing\n");
        fprintf(file, "   2. Check for missing semicolons, braces, or parentheses\n");
        fprintf(file, "   3. Verify proper token spelling and casing\n");
        fprintf(file, "   4. Ensure operators and literals follow language rules\n");
    }

    if (actual_semantic_errors > 0)
    {
        fprintf(file, "\n🔍 SEMANTIC ERROR RESOLUTION:\n");
        fprintf(file, "   1. Resolve type mismatches and incompatible operations\n");
        fprintf(file, "   2. Ensure all variables are declared before use\n");
        fprintf(file, "   3. Check function calls match their declarations\n");
        fprintf(file, "   4. Verify scope and visibility rules are followed\n");
    }

    if (semantic_warning_count > 0)
    {
        fprintf(file, "\n⚠️  WARNING RESOLUTION:\n");
        fprintf(file, "   1. Review warnings for potential issues\n");
        fprintf(file, "   2. Consider improving code quality and clarity\n");
        fprintf(file, "   3. Warnings don't prevent compilation but may indicate problems\n");
    }

    fprintf(file, "\n📚 ADDITIONAL RESOURCES:\n");
    fprintf(file, "   • Language Grammar: Check topdown_parser.c for syntax rules\n");
    fprintf(file, "   • Semantic Rules: Review TMA03 specification document\n");
    fprintf(file, "   • Test Cases: Examine valid*.txt files for examples\n");

    fprintf(file, "\n================================================================================\n");
}

// Type compatibility functions - Strict type checking
int types_compatible(DataType type1, DataType type2)
{
    // Exact type match
    if (type1 == type2)
        return 1;

    // Error types are compatible to prevent cascading errors
    if (type1 == TYPE_ERROR || type2 == TYPE_ERROR)
        return 1;

    // Unknown types are compatible to prevent cascading errors during initial analysis
    if (type1 == TYPE_UNKNOWN || type2 == TYPE_UNKNOWN)
        return 1;

    // STRICT MODE: Integer and float are NOT compatible
    // NO implicit conversions allowed - exact type match required

    // No other types are compatible
    return 0;
}

// Check if types are compatible with implicit conversion allowed
// UPDATED: Now using strict type checking - NO implicit conversions
int types_compatible_with_conversion(DataType type1, DataType type2)
{
    // Use strict compatibility - no implicit conversions
    return types_compatible(type1, type2);
}

// Extended compatibility check that uses optional AST node information.
// When available, node1/node2 can provide symbol information for arrays
// (element type) and classes (class name / inheritance).
int types_compatible_extended(DataType type1, DataType type2, ASTNode *node1, ASTNode *node2)
{
    // Exact match
    if (type1 == type2)
    {
        return 1;
    }

    // Error/unknown short-circuit to avoid cascading errors
    if (type1 == TYPE_ERROR || type2 == TYPE_ERROR)
    {
        return 1;
    }
    if (type1 == TYPE_UNKNOWN || type2 == TYPE_UNKNOWN)
    {
        return 1;
    }

    // STRICT TYPE CHECKING: No implicit conversions allowed
    // Integer is NOT compatible with float and vice versa
    // This will catch all type mismatches including int->float, float->int

    // Array compatibility: if both are arrays and we have symbol info, compare element types
    if (type1 == TYPE_ARRAY && type2 == TYPE_ARRAY)
    {
        if (node1 && node1->symbol && node2 && node2->symbol)
        {
            DataType e1 = node1->symbol->attributes.array.element_type;
            DataType e2 = node2->symbol->attributes.array.element_type;
            return types_compatible_extended(e1, e2, NULL, NULL);
        }
        // Fallback: treat both arrays as compatible to avoid false positives
        return 1;
    }

    // Class compatibility: if both are classes and we have symbol info, compare names or inheritance
    if (type1 == TYPE_CLASS && type2 == TYPE_CLASS)
    {
        if (node1 && node1->symbol && node2 && node2->symbol)
        {
            char *c1 = node1->symbol->name;
            char *c2 = node2->symbol->name;
            if (c1 && c2)
            {
                if (strcmp(c1, c2) == 0)
                    return 1;
                // Try inheritance check helper (in_same_class function is always available)
                if (in_same_class(c1, c2))
                    return 1;
                if (in_same_class(c2, c1))
                    return 1;
            }
        }
        // Fallback: require exact match if we don't have symbol info
        return 0;
    }

    // Void is only compatible with void
    if (type1 == TYPE_VOID || type2 == TYPE_VOID)
        return (type1 == TYPE_VOID && type2 == TYPE_VOID);

    // Default: not compatible
    printf("DEBUG: Types are NOT compatible\n");
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
    case AST_ARRAY_SIZE_LIST:
        return "ARRAY_SIZE_LIST";
    case AST_ARG_LIST:
        return "ARG_LIST";
    case AST_STMT_LIST:
        return "STMT_LIST";
    case AST_VAR_DECL_LIST:
        return "VAR_DECL_LIST";
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
    // Free semantic error list
    SemanticError *error = error_list;
    while (error)
    {
        SemanticError *next = error->next;
        free(error->message);
        free(error->error_type);
        free(error->context);
        free(error->suggestion);
        free(error);
        error = next;
    }
    error_list = NULL;

    // Free syntax error list
    SyntaxError *syntax_error = syntax_error_list;
    while (syntax_error)
    {
        SyntaxError *next = syntax_error->next;
        free(syntax_error->message);
        free(syntax_error->error_type);
        free(syntax_error->expected);
        free(syntax_error->found);
        free(syntax_error->context);
        free(syntax_error->suggestion);
        free(syntax_error);
        syntax_error = next;
    }
    syntax_error_list = NULL;

    // Close files
    if (symbol_table_file && symbol_table_file != stdout)
    {
        fclose(symbol_table_file);
    }
    if (semantic_error_file && semantic_error_file != stderr)
    {
        fclose(semantic_error_file);
    }
    if (ast_file && ast_file != stdout)
    {
        fclose(ast_file);
    }
}

// Count scopes for reporting
int count_scopes(SymbolTable *table)
{
    if (!table)
        return 0;

    int count = 1; // Count this scope

    // Count child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        count += count_scopes(child);
        child = child->next_sibling;
    }

    return count;
}

// Count symbols recursively
int count_symbols_recursive(SymbolTable *table)
{
    if (!table)
        return 0;

    int count = 0;

    // Count symbols in this scope
    Symbol *symbol = table->symbols;
    while (symbol)
    {
        count++;
        symbol = symbol->next;
    }

    // Count symbols in child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        count += count_symbols_recursive(child);
        child = child->next_sibling;
    }

    return count;
}

// Count symbols of specific type recursively
int count_symbols_by_type_recursive(SymbolTable *table, SymbolType type)
{
    if (!table)
        return 0;

    int count = 0;

    // Count symbols of specified type in this scope
    Symbol *symbol = table->symbols;
    while (symbol)
    {
        if (symbol->type == type)
        {
            count++;
        }
        symbol = symbol->next;
    }

    // Count symbols in child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        count += count_symbols_by_type_recursive(child, type);
        child = child->next_sibling;
    }

    return count;
}

// Get maximum scope depth
int get_max_scope_depth(SymbolTable *table)
{
    if (!table)
        return 0;

    int max_depth = table->scope_level;

    // Check child scopes
    SymbolTable *child = table->children;
    while (child)
    {
        int child_depth = get_max_scope_depth(child);
        if (child_depth > max_depth)
        {
            max_depth = child_depth;
        }
        child = child->next_sibling;
    }

    return max_depth;
}

// AST statistics helper functions
int count_ast_nodes(ASTNode *node)
{
    if (!node)
        return 0;

    int count = 1; // Count this node

    // Count children
    for (int i = 0; i < node->child_count; i++)
    {
        count += count_ast_nodes(node->children[i]);
    }

    // Count siblings
    if (node->next)
    {
        count += count_ast_nodes(node->next);
    }

    return count;
}

int get_ast_depth(ASTNode *node)
{
    if (!node)
        return 0;

    int max_child_depth = 0;

    // Find maximum depth among children
    for (int i = 0; i < node->child_count; i++)
    {
        int child_depth = get_ast_depth(node->children[i]);
        if (child_depth > max_child_depth)
        {
            max_child_depth = child_depth;
        }
    }

    // Check siblings at same level
    int sibling_depth = 0;
    if (node->next)
    {
        sibling_depth = get_ast_depth(node->next);
    }

    return 1 + (max_child_depth > sibling_depth ? max_child_depth : sibling_depth);
}

int count_leaf_nodes(ASTNode *node)
{
    if (!node)
        return 0;

    int count = 0;

    // If this node has no children, it's a leaf
    if (node->child_count == 0)
    {
        count = 1;
    }
    else
    {
        // Count leaves in children
        for (int i = 0; i < node->child_count; i++)
        {
            count += count_leaf_nodes(node->children[i]);
        }
    }

    // Count leaves in siblings
    if (node->next)
    {
        count += count_leaf_nodes(node->next);
    }

    return count;
}

int count_semantic_nodes(ASTNode *node)
{
    if (!node)
        return 0;

    int count = 0;

    // Count this node if it has semantic type information
    if (node->semantic_type != TYPE_UNKNOWN)
    {
        count = 1;
    }

    // Count semantic nodes in children
    for (int i = 0; i < node->child_count; i++)
    {
        count += count_semantic_nodes(node->children[i]);
    }

    // Count semantic nodes in siblings
    if (node->next)
    {
        count += count_semantic_nodes(node->next);
    }

    return count;
}

void print_node_type_statistics(ASTNode *node, FILE *file)
{
    if (!node || !file)
        return;

    // Count different node types
    int type_counts[50] = {0}; // Assume max 50 node types
    count_node_types(node, type_counts);

    fprintf(file, "┌─────────────────────────────────────────────────────────────┐\n");
    fprintf(file, "│ Node Type                    │ Count │ Percentage         │\n");
    fprintf(file, "├─────────────────────────────────────────────────────────────┤\n");

    int total_nodes = count_ast_nodes(node);

    // Print statistics for common node types
    const char *common_types[] = {
        "PROGRAM", "FUNC_DEF", "FUNC_HEAD", "FUNC_BODY", "STMT_LIST",
        "LOCAL_VAR_DECL", "VAR_DECL", "ASSIGN_STMT", "IF_STMT", "WHILE_STMT",
        "BINARY_EXPR", "UNARY_EXPR", "VARIABLE", "ID", "INT_LITERAL",
        "FLOAT_LITERAL", "TYPE", "RETURN_STMT", "WRITE_STMT", "CLASS_DECL"};

    for (int i = 0; i < 20; i++)
    {
        int type_index = get_node_type_index(common_types[i]);
        if (type_index >= 0 && type_counts[type_index] > 0)
        {
            fprintf(file, "│ %-28s │ %5d │ %6.1f%%           │\n",
                    common_types[i], type_counts[type_index],
                    (type_counts[type_index] * 100.0) / total_nodes);
        }
    }

    fprintf(file, "└─────────────────────────────────────────────────────────────┘\n");
}

void count_node_types(ASTNode *node, int *type_counts)
{
    if (!node)
        return;

    // Increment count for this node type
    if (node->type < 50)
    { // Safety check
        type_counts[node->type]++;
    }

    // Count children
    for (int i = 0; i < node->child_count; i++)
    {
        count_node_types(node->children[i], type_counts);
    }

    // Count siblings
    if (node->next)
    {
        count_node_types(node->next, type_counts);
    }
}

int get_node_type_index(const char *type_name)
{
    // Map common type names to indices (simplified version)
    // In a full implementation, this would use the actual ASTNodeType enum
    if (strcmp(type_name, "PROGRAM") == 0)
        return AST_PROGRAM;
    if (strcmp(type_name, "FUNC_DEF") == 0)
        return AST_FUNC_DEF;
    if (strcmp(type_name, "ID") == 0)
        return AST_ID;
    if (strcmp(type_name, "INT_LITERAL") == 0)
        return AST_INT_LITERAL;
    if (strcmp(type_name, "BINARY_EXPR") == 0)
        return AST_BINARY_EXPR;
    // Add more mappings as needed
    return -1;
}