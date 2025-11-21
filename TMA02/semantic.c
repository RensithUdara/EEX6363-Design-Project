#include "ast.h"

// Global variables for semantic analysis phase tracking
static int in_function_scope = 0;
static DataType current_function_return_type = TYPE_UNKNOWN;
static char *current_class_name = NULL;

// Forward declarations for semantic analysis phases
static void phase1_build_symbol_tables(ASTNode *node);
static void phase2_type_checking(ASTNode *node);

// Forward declarations for analysis functions (only static/internal functions)
static void analyze_class_declaration(ASTNode *node);
static void analyze_function_definition(ASTNode *node);
static Symbol *analyze_function_header(ASTNode *node);
static void analyze_variable_declaration(ASTNode *node);
static void analyze_assignment(ASTNode *node);
static void analyze_function_call(ASTNode *node);
static void analyze_binary_expression(ASTNode *node);
static void analyze_unary_expression(ASTNode *node);
static void analyze_variable_access(ASTNode *node);

// Main semantic analysis entry point
void semantic_analysis(ASTNode *root)
{
    if (!root)
    {
        fprintf(stderr, "Error: No AST provided for semantic analysis\n");
        return;
    }

    printf("\n=== Starting Semantic Analysis ===\n");

    // Initialize global scope
    global_scope = create_symbol_table("global", NULL);
    current_scope = global_scope;

    // Phase 1: Build symbol tables and perform declaration analysis
    printf("Phase 1: Building symbol tables and analyzing declarations...\n");
    phase1_build_symbol_tables(root);

    // Reset to global scope for phase 2
    current_scope = global_scope;

    // Phase 2: Type checking and semantic validation
    printf("Phase 2: Performing type checking and semantic validation...\n");
    phase2_type_checking(root);

    printf("Semantic analysis completed with %d errors and %d warnings.\n",
           semantic_error_count, semantic_warning_count);
}

// Phase 1: Build symbol tables and analyze declarations
static void phase1_build_symbol_tables(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_PROGRAM:
        // Process all top-level declarations
        for (int i = 0; i < node->child_count; i++)
        {
            phase1_build_symbol_tables(node->children[i]);
        }
        break;

    case AST_CLASS_DECL:
        analyze_class_declaration(node);
        break;

    case AST_FUNC_DEF:
        analyze_function_definition(node);
        break;

    case AST_IMPL_DEF:
        analyze_implementation_definition(node);
        break;

    case AST_VAR_DECL:
    case AST_ATTR_DECL:
    case AST_LOCAL_VAR_DECL:
        analyze_variable_declaration(node);
        break;

    case AST_PARAM_DECL:
        analyze_parameter_declaration(node);
        break;

    default:
        // Process children for other node types
        for (int i = 0; i < node->child_count; i++)
        {
            phase1_build_symbol_tables(node->children[i]);
        }
        break;
    }

    // Process sibling nodes
    if (node->next)
    {
        phase1_build_symbol_tables(node->next);
    }
}

// Phase 2: Type checking and semantic validation
static void phase2_type_checking(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_PROGRAM:
    case AST_CLASS_DECL:
    case AST_IMPL_DEF:
    case AST_FUNC_DEF:
        // Enter appropriate scope and analyze children
        analyze_scoped_node(node);
        break;

    case AST_ASSIGN_STMT:
        analyze_assignment(node);
        break;

    case AST_IF_STMT:
        analyze_if_statement(node);
        break;

    case AST_WHILE_STMT:
        analyze_while_statement(node);
        break;

    case AST_READ_STMT:
    case AST_WRITE_STMT:
    case AST_RETURN_STMT:
        analyze_simple_statement(node);
        break;

    case AST_FUNC_CALL_STMT:
    case AST_FUNC_CALL_EXPR:
        analyze_function_call(node);
        break;

    case AST_BINARY_EXPR:
        analyze_binary_expression(node);
        break;

    case AST_UNARY_EXPR:
        analyze_unary_expression(node);
        break;

    case AST_VARIABLE:
    case AST_ARRAY_ACCESS:
    case AST_MEMBER_ACCESS:
        analyze_variable_access(node);
        break;

    default:
        // Process children for other node types
        for (int i = 0; i < node->child_count; i++)
        {
            phase2_type_checking(node->children[i]);
        }
        break;
    }

    // Process sibling nodes
    if (node->next)
    {
        phase2_type_checking(node->next);
    }
}

// Analyze class declaration
void analyze_class_declaration(ASTNode *node)
{
    if (!node || node->type != AST_CLASS_DECL)
        return;

    ASTNode *class_id = node->children[0];     // Class name
    ASTNode *inherit_list = node->children[1]; // Inheritance list (may be NULL)
    ASTNode *member_list = node->children[2];  // Member declarations

    if (!class_id || class_id->type != AST_ID)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid class declaration - missing class name", "SYNTAX_ERROR");
        return;
    }

    // Create class symbol
    Symbol *class_symbol = create_symbol(class_id->data.string_value, SYM_CLASS,
                                         TYPE_CLASS, class_id->line, class_id->column);

    // Process inheritance
    if (inherit_list)
    {
        analyze_inheritance(class_symbol, inherit_list);
    }

    // Insert class into current scope
    insert_symbol(current_scope, class_symbol);

    // Create class scope and analyze members
    char scope_name[256];
    snprintf(scope_name, sizeof(scope_name), "class_%s", class_id->data.string_value);
    enter_scope(scope_name);

    // Store class scope in symbol
    class_symbol->attributes.class_info.members = current_scope;

    // Set current class context
    char *old_class_name = current_class_name;
    current_class_name = class_id->data.string_value;

    // Analyze class members
    if (member_list)
    {
        phase1_build_symbol_tables(member_list);
    }

    // Restore context
    current_class_name = old_class_name;
    exit_scope();
}

// Analyze function definition
void analyze_function_definition(ASTNode *node)
{
    if (!node || node->type != AST_FUNC_DEF)
        return;

    ASTNode *func_head = node->children[0];
    ASTNode *func_body = node->children[1];

    if (!func_head)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid function definition - missing function header", "SYNTAX_ERROR");
        return;
    }

    // Analyze function header
    Symbol *func_symbol = analyze_function_header(func_head);
    if (!func_symbol)
        return;

    // Insert function into current scope
    insert_symbol(current_scope, func_symbol);

    // Create function scope
    char scope_name[256];
    snprintf(scope_name, sizeof(scope_name), "func_%s", func_symbol->name);
    enter_scope(scope_name);

    func_symbol->attributes.func.local_scope = current_scope;

    // Set function context
    int old_in_function = in_function_scope;
    DataType old_return_type = current_function_return_type;
    in_function_scope = 1;
    current_function_return_type = func_symbol->attributes.func.return_type;

    // Add parameters to function scope
    if (func_symbol->attributes.func.param_list)
    {
        analyze_parameter_list(func_symbol->attributes.func.param_list);
    }

    // Analyze function body
    if (func_body)
    {
        phase1_build_symbol_tables(func_body);
    }

    // Restore context
    in_function_scope = old_in_function;
    current_function_return_type = old_return_type;
    exit_scope();
}

// Analyze function header
Symbol *analyze_function_header(ASTNode *node)
{
    if (!node || node->type != AST_FUNC_HEAD)
        return NULL;

    ASTNode *func_id = node->children[0];     // Function name
    ASTNode *param_list = node->children[1];  // Parameters
    ASTNode *return_type = node->children[2]; // Return type

    if (!func_id || func_id->type != AST_ID)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid function header - missing function name", "SYNTAX_ERROR");
        return NULL;
    }

    // Determine return type
    DataType ret_type = TYPE_VOID;
    if (return_type)
    {
        ret_type = get_data_type_from_node(return_type);
    }

    // Create function symbol
    Symbol *func_symbol = create_symbol(func_id->data.string_value, SYM_FUNC,
                                        ret_type, func_id->line, func_id->column);

    func_symbol->attributes.func.return_type = ret_type;
    func_symbol->attributes.func.param_list = param_list;

    return func_symbol;
}

// Analyze variable declaration
void analyze_variable_declaration(ASTNode *node)
{
    if (!node)
        return;

    ASTNode *var_id = NULL;
    ASTNode *type_node = NULL;
    ASTNode *array_sizes = NULL;

    // Extract components based on node type
    switch (node->type)
    {
    case AST_VAR_DECL:
    case AST_LOCAL_VAR_DECL:
        var_id = node->children[0];
        type_node = node->children[1];
        if (node->child_count > 2)
        {
            array_sizes = node->children[2];
        }
        break;
    case AST_ATTR_DECL:
        if (node->child_count > 0 && node->children[0]->type == AST_VAR_DECL)
        {
            ASTNode *var_decl = node->children[0];
            var_id = var_decl->children[0];
            type_node = var_decl->children[1];
            if (var_decl->child_count > 2)
            {
                array_sizes = var_decl->children[2];
            }
        }
        break;
    }

    if (!var_id || var_id->type != AST_ID || !type_node)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid variable declaration", "SYNTAX_ERROR");
        return;
    }

    // Get variable type
    DataType var_type = get_data_type_from_node(type_node);
    SymbolType sym_type = (node->type == AST_ATTR_DECL) ? SYM_ATTR : SYM_VAR;

    // Create variable symbol
    Symbol *var_symbol = create_symbol(var_id->data.string_value, sym_type,
                                       var_type, var_id->line, var_id->column);

    // Handle array types
    if (array_sizes)
    {
        analyze_array_declaration(var_symbol, array_sizes);
    }

    // Set visibility for attributes
    if (node->type == AST_ATTR_DECL)
    {
        // Visibility should be set by parent node analysis
        var_symbol->visibility = get_current_visibility();
    }

    // Insert into current scope
    insert_symbol(current_scope, var_symbol);
}

// Analyze assignment statement
void analyze_assignment(ASTNode *node)
{
    if (!node || node->type != AST_ASSIGN_STMT)
        return;

    ASTNode *lvalue = node->children[0]; // Left-hand side
    ASTNode *rvalue = node->children[1]; // Right-hand side

    if (!lvalue || !rvalue)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid assignment - missing operands", "SYNTAX_ERROR");
        return;
    }

    // Analyze both sides
    DataType ltype = analyze_expression(lvalue);
    DataType rtype = analyze_expression(rvalue);

    // Check type compatibility
    if (!types_compatible(ltype, rtype))
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Type mismatch in assignment: cannot assign %s to %s",
                 type_to_string(rtype), type_to_string(ltype));
        report_semantic_error(node->line, node->column, error_msg, "TYPE_MISMATCH");
    }

    // Check that left-hand side is assignable
    if (!is_assignable(lvalue))
    {
        report_semantic_error(lvalue->line, lvalue->column,
                              "Left-hand side of assignment is not assignable", "ASSIGNMENT_ERROR");
    }
}

// Analyze binary expression
void analyze_binary_expression(ASTNode *node)
{
    if (!node || node->type != AST_BINARY_EXPR)
        return;

    ASTNode *left = node->children[0];
    ASTNode *right = node->children[1];
    char *operator = node->data.operator;

    if (!left || !right || !operator)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid binary expression", "SYNTAX_ERROR");
        node->semantic_type = TYPE_ERROR;
        return;
    }

    // Analyze operands
    DataType left_type = analyze_expression(left);
    DataType right_type = analyze_expression(right);

    // Determine result type
    DataType result_type = get_binary_result_type(left_type, right_type, operator);

    if (result_type == TYPE_ERROR)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Invalid operands for binary operator '%s': %s and %s",
                 operator, type_to_string(left_type), type_to_string(right_type));
        report_semantic_error(node->line, node->column, error_msg, "TYPE_ERROR");
    }

    node->semantic_type = result_type;
}

// Analyze unary expression
void analyze_unary_expression(ASTNode *node)
{
    if (!node || node->type != AST_UNARY_EXPR)
        return;

    ASTNode *operand = node->children[0];
    char *operator = node->data.operator;

    if (!operand || !operator)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid unary expression", "SYNTAX_ERROR");
        node->semantic_type = TYPE_ERROR;
        return;
    }

    // Analyze operand
    DataType operand_type = analyze_expression(operand);

    // Determine result type
    DataType result_type = get_unary_result_type(operand_type, operator);

    if (result_type == TYPE_ERROR)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Invalid operand for unary operator '%s': %s",
                 operator, type_to_string(operand_type));
        report_semantic_error(node->line, node->column, error_msg, "TYPE_ERROR");
    }

    node->semantic_type = result_type;
}

// Analyze variable access
void analyze_variable_access(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_VARIABLE:
    {
        ASTNode *var_id = node->children[0];
        if (!var_id || var_id->type != AST_ID)
        {
            report_semantic_error(node->line, node->column,
                                  "Invalid variable reference", "SYNTAX_ERROR");
            node->semantic_type = TYPE_ERROR;
            return;
        }

        Symbol *symbol = lookup_symbol_global(current_scope, var_id->data.string_value);
        if (!symbol)
        {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg),
                     "Undefined variable '%s'", var_id->data.string_value);
            report_semantic_error(var_id->line, var_id->column, error_msg, "UNDEFINED_VARIABLE");
            node->semantic_type = TYPE_ERROR;
            return;
        }

        node->symbol = symbol;
        node->semantic_type = symbol->data_type;
        break;
    }

    case AST_ARRAY_ACCESS:
    {
        ASTNode *array = node->children[0];
        ASTNode *index = node->children[1];

        DataType array_type = analyze_expression(array);
        DataType index_type = analyze_expression(index);

        if (array_type != TYPE_ARRAY && array_type != TYPE_ERROR)
        {
            report_semantic_error(node->line, node->column,
                                  "Array access on non-array type", "TYPE_ERROR");
            node->semantic_type = TYPE_ERROR;
            return;
        }

        if (index_type != TYPE_INTEGER && index_type != TYPE_ERROR)
        {
            report_semantic_error(node->line, node->column,
                                  "Array index must be integer", "TYPE_ERROR");
        }

        // For arrays, determine element type
        if (array->symbol && array->symbol->data_type == TYPE_ARRAY)
        {
            node->semantic_type = array->symbol->attributes.array.element_type;
        }
        else
        {
            node->semantic_type = TYPE_ERROR;
        }
        break;
    }

    case AST_MEMBER_ACCESS:
    {
        ASTNode *object = node->children[0];
        ASTNode *member = node->children[1];

        DataType object_type = analyze_expression(object);

        if (object_type != TYPE_CLASS && object_type != TYPE_ERROR)
        {
            report_semantic_error(node->line, node->column,
                                  "Member access on non-class type", "TYPE_ERROR");
            node->semantic_type = TYPE_ERROR;
            return;
        }

        // Look up member in class scope
        if (object->symbol && object->symbol->type == SYM_CLASS)
        {
            SymbolTable *class_scope = object->symbol->attributes.class_info.members;
            Symbol *member_symbol = lookup_symbol(class_scope, member->data.string_value);

            if (!member_symbol)
            {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg),
                         "Undefined member '%s' in class", member->data.string_value);
                report_semantic_error(member->line, member->column, error_msg, "UNDEFINED_MEMBER");
                node->semantic_type = TYPE_ERROR;
                return;
            }

            // Check visibility
            if (member_symbol->visibility == VIS_PRIVATE && !in_same_class(current_class_name, object->symbol->name))
            {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg),
                         "Cannot access private member '%s'", member->data.string_value);
                report_semantic_error(member->line, member->column, error_msg, "ACCESS_VIOLATION");
            }

            node->symbol = member_symbol;
            node->semantic_type = member_symbol->data_type;
        }
        else
        {
            node->semantic_type = TYPE_ERROR;
        }
        break;
    }
    }
}

// Analyze function call
void analyze_function_call(ASTNode *node)
{
    if (!node || (node->type != AST_FUNC_CALL_STMT && node->type != AST_FUNC_CALL_EXPR))
    {
        return;
    }

    ASTNode *func_name = node->children[0];
    ASTNode *arg_list = node->child_count > 1 ? node->children[1] : NULL;

    if (!func_name || func_name->type != AST_ID)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid function call", "SYNTAX_ERROR");
        node->semantic_type = TYPE_ERROR;
        return;
    }

    // Look up function
    Symbol *func_symbol = lookup_symbol_global(current_scope, func_name->data.string_value);
    if (!func_symbol || func_symbol->type != SYM_FUNC)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Undefined function '%s'", func_name->data.string_value);
        report_semantic_error(func_name->line, func_name->column, error_msg, "UNDEFINED_FUNCTION");
        node->semantic_type = TYPE_ERROR;
        return;
    }

    // Check argument types and count
    check_function_arguments(func_symbol, arg_list, node->line, node->column);

    node->symbol = func_symbol;
    node->semantic_type = func_symbol->attributes.func.return_type;
}

// Main expression analysis function
DataType analyze_expression(ASTNode *node)
{
    if (!node)
        return TYPE_ERROR;

    switch (node->type)
    {
    case AST_INT_LITERAL:
        node->semantic_type = TYPE_INTEGER;
        return TYPE_INTEGER;

    case AST_FLOAT_LITERAL:
        node->semantic_type = TYPE_FLOAT;
        return TYPE_FLOAT;

    case AST_BINARY_EXPR:
        analyze_binary_expression(node);
        return node->semantic_type;

    case AST_UNARY_EXPR:
        analyze_unary_expression(node);
        return node->semantic_type;

    case AST_VARIABLE:
    case AST_ARRAY_ACCESS:
    case AST_MEMBER_ACCESS:
        analyze_variable_access(node);
        return node->semantic_type;

    case AST_FUNC_CALL_EXPR:
        analyze_function_call(node);
        return node->semantic_type;

    default:
        // For other expression types, analyze children and return first child's type
        if (node->child_count > 0)
        {
            return analyze_expression(node->children[0]);
        }
        return TYPE_ERROR;
    }
}

// Additional analysis functions
void analyze_inheritance(Symbol *class_symbol, ASTNode *inherit_list)
{
    if (!class_symbol || !inherit_list)
        return;

    // Count parent classes
    int parent_count = 0;
    ASTNode *parent = inherit_list;
    while (parent)
    {
        parent_count++;
        parent = parent->next;
    }

    // Allocate space for parent class names
    class_symbol->attributes.class_info.parent_classes =
        (char **)malloc(parent_count * sizeof(char *));
    class_symbol->attributes.class_info.parent_count = parent_count;

    // Store parent class names and check if they exist
    int i = 0;
    parent = inherit_list;
    while (parent && i < parent_count)
    {
        if (parent->type == AST_ID)
        {
            class_symbol->attributes.class_info.parent_classes[i] =
                duplicate_string(parent->data.string_value);

            // Check if parent class is declared
            Symbol *parent_symbol = lookup_symbol_global(current_scope, parent->data.string_value);
            if (!parent_symbol || parent_symbol->type != SYM_CLASS)
            {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg),
                         "Undefined parent class '%s'", parent->data.string_value);
                report_semantic_error(parent->line, parent->column, error_msg, "UNDEFINED_CLASS");
            }
        }
        parent = parent->next;
        i++;
    }
}

void analyze_implementation_definition(ASTNode *node)
{
    if (!node || node->type != AST_IMPL_DEF)
        return;

    ASTNode *class_id = node->children[0];
    ASTNode *func_list = node->children[1];

    if (!class_id || class_id->type != AST_ID)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid implementation - missing class name", "SYNTAX_ERROR");
        return;
    }

    // Check if class exists
    Symbol *class_symbol = lookup_symbol_global(current_scope, class_id->data.string_value);
    if (!class_symbol || class_symbol->type != SYM_CLASS)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Implementation for undefined class '%s'", class_id->data.string_value);
        report_semantic_error(class_id->line, class_id->column, error_msg, "UNDEFINED_CLASS");
        return;
    }

    // Enter class scope for implementation
    current_scope = class_symbol->attributes.class_info.members;
    char *old_class_name = current_class_name;
    current_class_name = class_id->data.string_value;

    // Analyze function implementations
    if (func_list)
    {
        phase1_build_symbol_tables(func_list);
    }

    // Restore context
    current_class_name = old_class_name;
    current_scope = global_scope;
}

void analyze_parameter_declaration(ASTNode *node)
{
    if (!node || node->type != AST_PARAM_DECL)
        return;

    ASTNode *param_id = node->children[0];
    ASTNode *type_node = node->children[1];
    ASTNode *array_sizes = node->child_count > 2 ? node->children[2] : NULL;

    if (!param_id || param_id->type != AST_ID || !type_node)
    {
        report_semantic_error(node->line, node->column,
                              "Invalid parameter declaration", "SYNTAX_ERROR");
        return;
    }

    DataType param_type = get_data_type_from_node(type_node);
    Symbol *param_symbol = create_symbol(param_id->data.string_value, SYM_PARAM,
                                         param_type, param_id->line, param_id->column);

    if (array_sizes)
    {
        analyze_array_declaration(param_symbol, array_sizes);
    }

    insert_symbol(current_scope, param_symbol);
}

void analyze_parameter_list(ASTNode *param_list)
{
    if (!param_list)
        return;

    ASTNode *param = param_list;
    while (param)
    {
        if (param->type == AST_PARAM_DECL)
        {
            analyze_parameter_declaration(param);
        }
        param = param->next;
    }
}

void analyze_array_declaration(Symbol *symbol, ASTNode *array_sizes)
{
    if (!symbol || !array_sizes)
        return;

    symbol->data_type = TYPE_ARRAY;

    // Count dimensions
    int dimensions = 0;
    ASTNode *size_node = array_sizes;
    while (size_node)
    {
        if (size_node->type == AST_ARRAY_SIZE)
        {
            dimensions++;
        }
        size_node = size_node->next;
    }

    symbol->attributes.array.dimensions = dimensions;
    symbol->attributes.array.sizes = (int *)malloc(dimensions * sizeof(int));

    // Extract sizes
    int i = 0;
    size_node = array_sizes;
    while (size_node && i < dimensions)
    {
        if (size_node->type == AST_ARRAY_SIZE && size_node->child_count > 0)
        {
            ASTNode *size_literal = size_node->children[0];
            if (size_literal && size_literal->type == AST_INT_LITERAL)
            {
                symbol->attributes.array.sizes[i] = size_literal->data.int_value;
            }
            else
            {
                symbol->attributes.array.sizes[i] = -1; // Dynamic size
            }
        }
        size_node = size_node->next;
        i++;
    }

    // For now, assume element type is the original type before array conversion
    // This should be enhanced to handle array element types properly
    symbol->attributes.array.element_type = TYPE_INTEGER; // Default
}

void analyze_scoped_node(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_PROGRAM:
        for (int i = 0; i < node->child_count; i++)
        {
            phase2_type_checking(node->children[i]);
        }
        break;

    case AST_CLASS_DECL:
    {
        ASTNode *class_id = node->children[0];
        if (class_id && class_id->type == AST_ID)
        {
            Symbol *class_symbol = lookup_symbol_global(current_scope, class_id->data.string_value);
            if (class_symbol && class_symbol->attributes.class_info.members)
            {
                SymbolTable *old_scope = current_scope;
                current_scope = class_symbol->attributes.class_info.members;

                for (int i = 1; i < node->child_count; i++)
                {
                    phase2_type_checking(node->children[i]);
                }

                current_scope = old_scope;
            }
        }
        break;
    }

    case AST_FUNC_DEF:
    {
        ASTNode *func_head = node->children[0];
        if (func_head && func_head->child_count > 0)
        {
            ASTNode *func_id = func_head->children[0];
            if (func_id && func_id->type == AST_ID)
            {
                Symbol *func_symbol = lookup_symbol_global(current_scope, func_id->data.string_value);
                if (func_symbol && func_symbol->attributes.func.local_scope)
                {
                    SymbolTable *old_scope = current_scope;
                    current_scope = func_symbol->attributes.func.local_scope;

                    int old_in_function = in_function_scope;
                    DataType old_return_type = current_function_return_type;
                    in_function_scope = 1;
                    current_function_return_type = func_symbol->attributes.func.return_type;

                    for (int i = 1; i < node->child_count; i++)
                    {
                        phase2_type_checking(node->children[i]);
                    }

                    in_function_scope = old_in_function;
                    current_function_return_type = old_return_type;
                    current_scope = old_scope;
                }
            }
        }
        break;
    }

    default:
        for (int i = 0; i < node->child_count; i++)
        {
            phase2_type_checking(node->children[i]);
        }
        break;
    }
}

void analyze_if_statement(ASTNode *node)
{
    if (!node || node->type != AST_IF_STMT)
        return;

    ASTNode *condition = node->children[0];
    ASTNode *then_block = node->children[1];
    ASTNode *else_block = node->child_count > 2 ? node->children[2] : NULL;

    // Check condition type
    if (condition)
    {
        DataType cond_type = analyze_expression(condition);
        if (cond_type != TYPE_INTEGER && cond_type != TYPE_ERROR)
        {
            report_semantic_error(condition->line, condition->column,
                                  "If condition must be integer type", "TYPE_ERROR");
        }
    }

    // Analyze blocks
    if (then_block)
    {
        phase2_type_checking(then_block);
    }
    if (else_block)
    {
        phase2_type_checking(else_block);
    }
}

void analyze_while_statement(ASTNode *node)
{
    if (!node || node->type != AST_WHILE_STMT)
        return;

    ASTNode *condition = node->children[0];
    ASTNode *body = node->children[1];

    // Check condition type
    if (condition)
    {
        DataType cond_type = analyze_expression(condition);
        if (cond_type != TYPE_INTEGER && cond_type != TYPE_ERROR)
        {
            report_semantic_error(condition->line, condition->column,
                                  "While condition must be integer type", "TYPE_ERROR");
        }
    }

    // Analyze body
    if (body)
    {
        phase2_type_checking(body);
    }
}

void analyze_simple_statement(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_READ_STMT:
        if (node->child_count > 0)
        {
            ASTNode *var = node->children[0];
            DataType var_type = analyze_expression(var);
            if (!is_assignable(var))
            {
                report_semantic_error(var->line, var->column,
                                      "Read target must be assignable", "ASSIGNMENT_ERROR");
            }
        }
        break;

    case AST_WRITE_STMT:
        if (node->child_count > 0)
        {
            analyze_expression(node->children[0]);
        }
        break;

    case AST_RETURN_STMT:
        if (!in_function_scope)
        {
            report_semantic_error(node->line, node->column,
                                  "Return statement outside function", "SCOPE_ERROR");
            break;
        }

        if (node->child_count > 0)
        {
            DataType return_expr_type = analyze_expression(node->children[0]);
            if (!types_compatible(return_expr_type, current_function_return_type))
            {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg),
                         "Return type mismatch: expected %s, got %s",
                         type_to_string(current_function_return_type),
                         type_to_string(return_expr_type));
                report_semantic_error(node->line, node->column, error_msg, "TYPE_MISMATCH");
            }
        }
        else if (current_function_return_type != TYPE_VOID)
        {
            report_semantic_error(node->line, node->column,
                                  "Non-void function must return a value", "RETURN_ERROR");
        }
        break;
    }
}

// Helper functions

DataType get_data_type_from_node(ASTNode *node)
{
    if (!node || node->type != AST_TYPE)
        return TYPE_ERROR;

    if (node->data.data_type != TYPE_UNKNOWN)
    {
        return node->data.data_type;
    }

    // For class types, the type node might contain an ID
    if (node->child_count > 0 && node->children[0]->type == AST_ID)
    {
        // Look up class type
        Symbol *class_symbol = lookup_symbol_global(current_scope, node->children[0]->data.string_value);
        if (class_symbol && class_symbol->type == SYM_CLASS)
        {
            return TYPE_CLASS;
        }

        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Undefined type '%s'", node->children[0]->data.string_value);
        report_semantic_error(node->line, node->column, error_msg, "UNDEFINED_TYPE");
    }

    return TYPE_ERROR;
}

int is_assignable(ASTNode *node)
{
    if (!node)
        return 0;

    switch (node->type)
    {
    case AST_VARIABLE:
        return 1;
    case AST_ARRAY_ACCESS:
        return 1;
    case AST_MEMBER_ACCESS:
        return 1;
    default:
        return 0;
    }
}

Visibility get_current_visibility()
{
    // This should be set by the parser based on the current visibility context
    // For now, default to public
    return VIS_PUBLIC;
}

int in_same_class(char *class1, char *class2)
{
    if (!class1 || !class2)
        return 0;
    return strcmp(class1, class2) == 0;
}

void check_function_arguments(Symbol *func_symbol, ASTNode *arg_list, int line, int column)
{
    if (!func_symbol)
        return;

    // Count actual arguments
    int arg_count = 0;
    ASTNode *arg = arg_list;
    while (arg)
    {
        arg_count++;
        if (arg->type == AST_ARG_LIST && arg->child_count > 0)
        {
            // Process argument expression
            analyze_expression(arg->children[0]);
        }
        arg = arg->next;
    }

    // Count expected parameters
    int param_count = 0;
    ASTNode *param = func_symbol->attributes.func.param_list;
    while (param)
    {
        param_count++;
        param = param->next;
    }

    if (arg_count != param_count)
    {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Function '%s' expects %d arguments, but %d were provided",
                 func_symbol->name, param_count, arg_count);
        report_semantic_error(line, column, error_msg, "ARGUMENT_COUNT_MISMATCH");
        return; // No point in type checking if argument count is wrong
    }

    // Detailed type checking for each argument
    ASTNode *current_arg = arg_list;
    ASTNode *current_param = func_symbol->attributes.func.param_list;
    int arg_index = 0;

    while (current_arg && current_param && arg_index < arg_count)
    {
        // Get argument expression and its type
        ASTNode *arg_expr = NULL;
        if (current_arg->type == AST_ARG_LIST && current_arg->child_count > 0)
        {
            arg_expr = current_arg->children[0];
        }
        else
        {
            arg_expr = current_arg;
        }

        DataType arg_type = TYPE_UNKNOWN;
        if (arg_expr)
        {
            arg_type = analyze_expression(arg_expr);
        }

        // Get parameter type
        DataType param_type = TYPE_UNKNOWN;
        if (current_param->type == AST_PARAM_DECL && current_param->child_count > 1)
        {
            ASTNode *param_type_node = current_param->children[1];
            param_type = get_data_type_from_node(param_type_node);

            // Handle array parameters
            if (current_param->child_count > 2 && current_param->children[2])
            {
                param_type = TYPE_ARRAY;
            }
        }

        // Check type compatibility
        if (arg_type != TYPE_ERROR && param_type != TYPE_ERROR &&
            !types_compatible(arg_type, param_type))
        {

            char error_msg[256];
            char param_name[64] = "unknown";

            // Try to get parameter name for better error message
            if (current_param->child_count > 0 &&
                current_param->children[0]->type == AST_ID)
            {
                strncpy(param_name, current_param->children[0]->data.string_value,
                        sizeof(param_name) - 1);
                param_name[sizeof(param_name) - 1] = '\0';
            }

            snprintf(error_msg, sizeof(error_msg),
                     "Type mismatch in argument %d of function '%s': "
                     "expected %s for parameter '%s', got %s",
                     arg_index + 1, func_symbol->name,
                     type_to_string(param_type), param_name,
                     type_to_string(arg_type));

            if (arg_expr)
            {
                report_semantic_error(arg_expr->line, arg_expr->column,
                                      error_msg, "ARGUMENT_TYPE_MISMATCH");
            }
            else
            {
                report_semantic_error(line, column, error_msg, "ARGUMENT_TYPE_MISMATCH");
            }
        }

        // Check for special cases
        if (param_type == TYPE_ARRAY && arg_type != TYPE_ARRAY && arg_type != TYPE_ERROR)
        {
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg),
                     "Argument %d of function '%s' expects array type, got %s",
                     arg_index + 1, func_symbol->name, type_to_string(arg_type));

            if (arg_expr)
            {
                report_semantic_error(arg_expr->line, arg_expr->column,
                                      error_msg, "ARRAY_EXPECTED");
            }
        }

        // Move to next argument and parameter
        current_arg = current_arg->next;
        current_param = current_param->next;
        arg_index++;
    }
}