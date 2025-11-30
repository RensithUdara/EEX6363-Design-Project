/*
 * COMPREHENSIVE ERROR HANDLER IMPLEMENTATION
 * Handles all types of errors: lexical, syntax, and semantic
 */

// Enable strdup and other POSIX functions
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "../../include/utils/error_handler.h"
#include "../../include/core/ast.h"
#include <stdarg.h>
#include <string.h>
#include <time.h>

// Simple strdup implementation for compatibility
static char *my_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy)
    {
        memcpy(copy, s, len);
    }
    return copy;
}

// Use our implementation
#define strdup my_strdup

// Global Error Manager Instance
ErrorManager *global_error_manager = NULL;

// Global error log file for real-time error writing
FILE *global_error_log_file = NULL;

// Error type to string mappings
static const char *error_type_names[] = {
    // Lexical Errors
    "LEXICAL_INVALID_TOKEN", "LEXICAL_INVALID_NUMBER", "LEXICAL_INVALID_FLOAT",
    "LEXICAL_INVALID_IDENTIFIER", "LEXICAL_UNTERMINATED_COMMENT", "LEXICAL_UNTERMINATED_STRING",
    "LEXICAL_INVALID_CHARACTER", "LEXICAL_DOUBLE_SEMICOLON",

    // Syntax Errors
    "SYNTAX_MISSING_TOKEN", "SYNTAX_UNEXPECTED_TOKEN", "SYNTAX_MISSING_SEMICOLON",
    "SYNTAX_MISSING_BRACE", "SYNTAX_MISSING_PARENTHESIS", "SYNTAX_MISSING_BRACKET",
    "SYNTAX_INVALID_EXPRESSION", "SYNTAX_INVALID_STATEMENT", "SYNTAX_INVALID_DECLARATION",
    "SYNTAX_MALFORMED_FUNCTION", "SYNTAX_MALFORMED_CLASS", "SYNTAX_INVALID_OPERATOR",

    // Semantic Errors - Declarations
    "SEMANTIC_DUPLICATE_DECLARATION", "SEMANTIC_UNDEFINED_VARIABLE", "SEMANTIC_UNDEFINED_FUNCTION",
    "SEMANTIC_UNDEFINED_CLASS", "SEMANTIC_UNDEFINED_MEMBER", "SEMANTIC_REDEFINITION_ERROR",

    // Semantic Errors - Types
    "SEMANTIC_TYPE_MISMATCH", "SEMANTIC_INCOMPATIBLE_ASSIGNMENT", "SEMANTIC_INCOMPATIBLE_OPERATION",
    "SEMANTIC_INVALID_ARRAY_INDEX", "SEMANTIC_INVALID_CAST", "SEMANTIC_RETURN_TYPE_MISMATCH",

    // Semantic Errors - Functions
    "SEMANTIC_FUNCTION_CALL_ERROR", "SEMANTIC_WRONG_ARGUMENT_COUNT", "SEMANTIC_ARGUMENT_TYPE_MISMATCH",
    "SEMANTIC_NO_RETURN_STATEMENT", "SEMANTIC_UNREACHABLE_CODE", "SEMANTIC_RECURSIVE_DEFINITION",

    // Semantic Errors - Classes and Inheritance
    "SEMANTIC_INHERITANCE_ERROR", "SEMANTIC_CIRCULAR_INHERITANCE", "SEMANTIC_INVALID_INHERITANCE",
    "SEMANTIC_MEMBER_ACCESS_ERROR", "SEMANTIC_VISIBILITY_VIOLATION", "SEMANTIC_CONSTRUCTOR_ERROR",

    // Semantic Errors - Scopes and Variables
    "SEMANTIC_SCOPE_ERROR", "SEMANTIC_VARIABLE_NOT_INITIALIZED", "SEMANTIC_CONSTANT_ASSIGNMENT",
    "SEMANTIC_ARRAY_BOUNDS_ERROR", "SEMANTIC_MEMORY_ACCESS_ERROR",

    // Control Flow Errors
    "SEMANTIC_CONDITION_TYPE_ERROR", "SEMANTIC_BREAK_OUTSIDE_LOOP", "SEMANTIC_CONTINUE_OUTSIDE_LOOP",
    "SEMANTIC_INVALID_CONTROL_FLOW",

    // General Errors
    "SEMANTIC_GENERAL_ERROR", "SEMANTIC_INVALID_DECLARATION", "SEMANTIC_INVALID_IDENTIFIER",
    "SEMANTIC_INVALID_TYPE", "SEMANTIC_INVALID_PARAMETER", "SEMANTIC_MALFORMED_FUNCTION",
    "SEMANTIC_INVALID_EXPRESSION", "SEMANTIC_INVALID_VARIABLE_ACCESS", "SEMANTIC_INVALID_OPERATOR",
    "SEMANTIC_INVALID_FUNCTION_CALL", "SEMANTIC_INVALID_ASSIGNMENT", "SEMANTIC_INVALID_ARRAY_ACCESS",
    "INTERNAL_ERROR"};

// Error codes for quick reference
static const char *error_codes[] = {
    // Lexical Errors
    "L001", "L002", "L003", "L004", "L005", "L006", "L007", "L008",

    // Syntax Errors
    "S001", "S002", "S003", "S004", "S005", "S006", "S007", "S008", "S009", "S010", "S011", "S012",

    // Semantic Errors - Declarations
    "E001", "E002", "E003", "E004", "E005", "E006",

    // Semantic Errors - Types
    "E007", "E008", "E009", "E010", "E011", "E012",

    // Semantic Errors - Functions
    "E013", "E014", "E015", "E016", "E017", "E018",

    // Semantic Errors - Classes and Inheritance
    "E019", "E020", "E021", "E022", "E023", "E024",

    // Semantic Errors - Scopes and Variables
    "E025", "E026", "E027", "E028", "E029",

    // Control Flow Errors
    "E030", "E031", "E032", "E033",

    // General Errors
    "E999", "E034", "E035", "E036", "E037", "E038", "E039", "E040", "E041", "E042", "E043", "E044", "I001"};

// Error Manager Functions
ErrorManager *create_error_manager(void)
{
    ErrorManager *manager = (ErrorManager *)malloc(sizeof(ErrorManager));
    if (!manager)
        return NULL;

    manager->syntax_errors = NULL;
    manager->semantic_errors = NULL;
    manager->lexical_errors = NULL;
    manager->syntax_error_count = 0;
    manager->semantic_error_count = 0;
    manager->lexical_error_count = 0;
    manager->warning_count = 0;
    manager->max_errors = 50; // Increased from 10
    manager->suppress_warnings = 0;
    manager->current_filename = NULL;

    return manager;
}

void destroy_error_manager(ErrorManager *manager)
{
    if (!manager)
        return;

    // Free all error lists
    ComprehensiveError *current;

    current = manager->syntax_errors;
    while (current)
    {
        ComprehensiveError *next = current->next;
        free(current->message);
        free(current->filename);
        free(current->expected);
        free(current->found);
        free(current->context);
        free(current->suggestion);
        free(current->error_code);
        free(current);
        current = next;
    }

    current = manager->semantic_errors;
    while (current)
    {
        ComprehensiveError *next = current->next;
        free(current->message);
        free(current->filename);
        free(current->context);
        free(current->suggestion);
        free(current->error_code);
        free(current);
        current = next;
    }

    current = manager->lexical_errors;
    while (current)
    {
        ComprehensiveError *next = current->next;
        free(current->message);
        free(current->filename);
        free(current->found);
        free(current->suggestion);
        free(current->error_code);
        free(current);
        current = next;
    }

    free(manager->current_filename);
    free(manager);
}

void reset_error_manager(ErrorManager *manager)
{
    if (!manager)
        return;

    // Don't destroy and recreate - just reset counters and clear lists
    // Implementation would clear the lists but keep the manager structure
    manager->syntax_error_count = 0;
    manager->semantic_error_count = 0;
    manager->lexical_error_count = 0;
    manager->warning_count = 0;
}

void set_current_filename(const char *filename)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }

    free(global_error_manager->current_filename);
    global_error_manager->current_filename = filename ? strdup(filename) : NULL;
}

// Helper function to create a comprehensive error
static ComprehensiveError *create_comprehensive_error(ErrorType type, ErrorSeverity severity,
                                                      int line, int col, const char *message)
{
    ComprehensiveError *error = (ComprehensiveError *)malloc(sizeof(ComprehensiveError));
    if (!error)
        return NULL;

    error->type = type;
    error->severity = severity;
    error->line = line;
    error->column = col;
    error->message = message ? strdup(message) : NULL;
    error->filename = (global_error_manager && global_error_manager->current_filename) ? strdup(global_error_manager->current_filename) : NULL;
    error->expected = NULL;
    error->found = NULL;
    error->context = NULL;
    error->suggestion = NULL;
    error->error_code = strdup(get_error_code(type));
    error->next = NULL;

    return error;
}

// Error Reporting Functions
void report_lexical_error(ErrorType type, int line, int col, const char *message,
                          const char *found, const char *suggestion)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }

    if (global_error_manager->lexical_error_count >= global_error_manager->max_errors)
    {
        fprintf(stderr, "Too many lexical errors. Stopping analysis.\n");
        exit(1);
    }

    ComprehensiveError *error = create_comprehensive_error(type, ERROR_ERROR, line, col, message);
    if (!error)
        return;

    error->found = found ? strdup(found) : NULL;
    error->suggestion = suggestion ? strdup(suggestion) : NULL;

    // Add to lexical error list
    error->next = global_error_manager->lexical_errors;
    global_error_manager->lexical_errors = error;
    global_error_manager->lexical_error_count++;

    // Print immediate feedback to both stderr and error log file
    fprintf(stderr, "❌ [%s] Lexical Error at line %d:%d: %s\n",
            error->error_code, line, col, message);
    if (global_error_log_file)
    {
        fprintf(global_error_log_file, "❌ [%s] Lexical Error at line %d:%d: %s\n",
                error->error_code, line, col, message);
    }
    if (found)
    {
        fprintf(stderr, "   ├─ Found: '%s'\n", found);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   ├─ Found: '%s'\n", found);
        }
    }
    if (suggestion)
    {
        fprintf(stderr, "   └─ 💡 Suggestion: %s\n", suggestion);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   └─ 💡 Suggestion: %s\n", suggestion);
        }
    }
}

void report_syntax_error_enhanced(ErrorType type, int line, int col, const char *message,
                                  const char *expected, const char *found, const char *suggestion)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }

    if (global_error_manager->syntax_error_count >= global_error_manager->max_errors)
    {
        fprintf(stderr, "Too many syntax errors. Stopping analysis.\n");
        exit(1);
    }

    ComprehensiveError *error = create_comprehensive_error(type, ERROR_ERROR, line, col, message);
    if (!error)
        return;

    error->expected = expected ? strdup(expected) : NULL;
    error->found = found ? strdup(found) : NULL;
    error->suggestion = suggestion ? strdup(suggestion) : NULL;

    // Add context if possible
    if (global_error_manager->current_filename)
    {
        error->context = get_detailed_context(global_error_manager->current_filename, line, col, 1);
    }

    // Add to syntax error list
    error->next = global_error_manager->syntax_errors;
    global_error_manager->syntax_errors = error;
    global_error_manager->syntax_error_count++;

    // Print immediate feedback to both stderr and error log file
    fprintf(stderr, "❌ [%s] Syntax Error at line %d:%d: %s\n",
            error->error_code, line, col, message);
    if (global_error_log_file)
    {
        fprintf(global_error_log_file, "❌ [%s] Syntax Error at line %d:%d: %s\n",
                error->error_code, line, col, message);
    }
    if (expected && found)
    {
        fprintf(stderr, "   ├─ Expected: %s\n", expected);
        fprintf(stderr, "   ├─ Found: %s\n", found);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   ├─ Expected: %s\n", expected);
            fprintf(global_error_log_file, "   ├─ Found: %s\n", found);
        }
    }
    if (suggestion)
    {
        fprintf(stderr, "   └─ 💡 Suggestion: %s\n", suggestion);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   └─ 💡 Suggestion: %s\n", suggestion);
        }
    }
}

void report_semantic_error_enhanced(ErrorType type, int line, int col, const char *message,
                                    const char *context, const char *suggestion)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }

    if (global_error_manager->semantic_error_count >= global_error_manager->max_errors)
    {
        fprintf(stderr, "Too many semantic errors. Stopping analysis.\n");
        return; // Don't exit for semantic errors, allow parsing to continue
    }

    ComprehensiveError *error = create_comprehensive_error(type, ERROR_ERROR, line, col, message);
    if (!error)
        return;

    error->context = context ? strdup(context) : NULL;
    error->suggestion = suggestion ? strdup(suggestion) : NULL;

    // Add to semantic error list
    error->next = global_error_manager->semantic_errors;
    global_error_manager->semantic_errors = error;
    global_error_manager->semantic_error_count++;

    // Print immediate feedback to both stderr and error log file
    fprintf(stderr, "❌ [%s] Semantic Error at line %d:%d: %s\n",
            error->error_code, line, col, message);
    if (global_error_log_file)
    {
        fprintf(global_error_log_file, "❌ [%s] Semantic Error at line %d:%d: %s\n",
                error->error_code, line, col, message);
    }
    if (context)
    {
        fprintf(stderr, "   ├─ Context: %s\n", context);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   ├─ Context: %s\n", context);
        }
    }
    if (suggestion)
    {
        fprintf(stderr, "   └─ 💡 Suggestion: %s\n", suggestion);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   └─ 💡 Suggestion: %s\n", suggestion);
        }
    }
}

void report_warning(ErrorType type, int line, int col, const char *message, const char *suggestion)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }

    if (global_error_manager->suppress_warnings)
    {
        return; // Warnings are suppressed
    }

    ComprehensiveError *error = create_comprehensive_error(type, ERROR_WARNING, line, col, message);
    if (!error)
        return;

    error->suggestion = suggestion ? strdup(suggestion) : NULL;

    // Add to appropriate warning list (we'll use semantic_errors for warnings too)
    // In a more complex system, you might want separate warning lists
    error->next = global_error_manager->semantic_errors;
    global_error_manager->semantic_errors = error;
    global_error_manager->warning_count++;

    // Print immediate feedback to both stderr and error log file
    fprintf(stderr, "⚠️ [%s] Warning at line %d:%d: %s\n",
            error->error_code, line, col, message);
    if (global_error_log_file)
    {
        fprintf(global_error_log_file, "⚠️ [%s] Warning at line %d:%d: %s\n",
                error->error_code, line, col, message);
    }
    if (suggestion)
    {
        fprintf(stderr, "   └─ 💡 Suggestion: %s\n", suggestion);
        if (global_error_log_file)
        {
            fprintf(global_error_log_file, "   └─ 💡 Suggestion: %s\n", suggestion);
        }
    }
}

// Specific Error Reporting Functions
void report_duplicate_declaration(int line, int col, const char *name, const char *type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Duplicate %s declaration: '%s'", type, name);
    snprintf(suggestion, sizeof(suggestion), "Remove the duplicate declaration or rename '%s'", name);

    report_semantic_error_enhanced(SEM_DUPLICATE_DECLARATION, line, col, message, NULL, suggestion);
}

void report_undefined_symbol(int line, int col, const char *name, const char *symbol_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Undefined %s: '%s'", symbol_type, name);
    snprintf(suggestion, sizeof(suggestion), "Declare '%s' before using it", name);

    ErrorType error_type = SEM_UNDEFINED_VARIABLE;
    if (strcmp(symbol_type, "function") == 0)
    {
        error_type = SEM_UNDEFINED_FUNCTION;
    }
    else if (strcmp(symbol_type, "class") == 0)
    {
        error_type = SEM_UNDEFINED_CLASS;
    }

    report_semantic_error_enhanced(error_type, line, col, message, NULL, suggestion);
}

void report_type_mismatch(int line, int col, DataType expected, DataType found, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Type mismatch: expected %s, but got %s",
             type_to_string(expected), type_to_string(found));

    if (context)
    {
        snprintf(suggestion, sizeof(suggestion), "Change the %s to match the expected type %s",
                 context, type_to_string(expected));
    }
    else
    {
        snprintf(suggestion, sizeof(suggestion), "Convert the value to %s or change the expected type",
                 type_to_string(expected));
    }

    report_semantic_error_enhanced(SEM_TYPE_MISMATCH, line, col, message, context, suggestion);
}

void report_function_call_error(int line, int col, const char *func_name, int expected_args, int found_args)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Function '%s' expects %d argument(s), but %d were provided",
             func_name, expected_args, found_args);

    if (found_args < expected_args)
    {
        snprintf(suggestion, sizeof(suggestion), "Add %d more argument(s) to the function call",
                 expected_args - found_args);
    }
    else
    {
        snprintf(suggestion, sizeof(suggestion), "Remove %d argument(s) from the function call",
                 found_args - expected_args);
    }

    report_semantic_error_enhanced(SEM_WRONG_ARGUMENT_COUNT, line, col, message, NULL, suggestion);
}

void report_argument_type_error(int line, int col, const char *func_name, int arg_num,
                                DataType expected, DataType found)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Function '%s' argument %d: expected %s, but got %s",
             func_name, arg_num, type_to_string(expected), type_to_string(found));

    snprintf(suggestion, sizeof(suggestion),
             "Convert argument %d to %s or change the function parameter type",
             arg_num, type_to_string(expected));

    report_semantic_error_enhanced(SEM_ARGUMENT_TYPE_MISMATCH, line, col, message, NULL, suggestion);
}

void report_inheritance_error(int line, int col, const char *child, const char *parent, const char *reason)
{
    char message[256];
    char suggestion[128];

    snprintf(message, sizeof(message), "Inheritance error: class '%s' cannot inherit from '%s' - %s",
             child, parent, reason);
    snprintf(suggestion, sizeof(suggestion), "Check the parent class declaration and inheritance hierarchy");

    report_semantic_error_enhanced(SEM_INHERITANCE_ERROR, line, col, message, NULL, suggestion);
}

void report_member_access_error(int line, int col, const char *member, const char *class_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Member '%s' not found in class '%s'", member, class_name);
    snprintf(suggestion, sizeof(suggestion), "Check the member name spelling or add the member to class '%s'", class_name);

    report_semantic_error_enhanced(SEM_MEMBER_ACCESS_ERROR, line, col, message, NULL, suggestion);
}

void report_visibility_violation(int line, int col, const char *member, const char *visibility)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Cannot access %s member '%s' from this context", visibility, member);
    snprintf(suggestion, sizeof(suggestion), "Change member visibility to public or access from within the class");

    report_semantic_error_enhanced(SEM_VISIBILITY_VIOLATION, line, col, message, NULL, suggestion);
}

// Additional Comprehensive Error Reporting Functions

void report_incompatible_assignment(int line, int col, DataType lhs_type, DataType rhs_type, const char *variable_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Incompatible assignment to '%s': cannot assign %s to %s",
             variable_name, type_to_string(rhs_type), type_to_string(lhs_type));

    snprintf(suggestion, sizeof(suggestion),
             "Cast the %s value to %s or change the variable type",
             type_to_string(rhs_type), type_to_string(lhs_type));

    report_semantic_error_enhanced(SEM_INCOMPATIBLE_ASSIGNMENT, line, col, message, NULL, suggestion);
}

void report_incompatible_operation(int line, int col, const char *operator, DataType left_type, DataType right_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Incompatible types for operator '%s': %s and %s",
             operator, type_to_string(left_type), type_to_string(right_type));

    snprintf(suggestion, sizeof(suggestion),
             "Ensure both operands are compatible types for operator '%s'", operator);

    report_semantic_error_enhanced(SEM_INCOMPATIBLE_OPERATION, line, col, message, NULL, suggestion);
}

void report_invalid_array_index(int line, int col, DataType index_type, const char *array_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid array index type %s for array '%s' - expected integer",
             type_to_string(index_type), array_name);

    snprintf(suggestion, sizeof(suggestion),
             "Convert the index to an integer type or use an integer expression");

    report_semantic_error_enhanced(SEM_INVALID_ARRAY_INDEX, line, col, message, NULL, suggestion);
}

void report_invalid_cast(int line, int col, DataType from_type, DataType to_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid cast from %s to %s",
             type_to_string(from_type), type_to_string(to_type));

    snprintf(suggestion, sizeof(suggestion),
             "Use a valid cast operation or change the target type");

    report_semantic_error_enhanced(SEM_INVALID_CAST, line, col, message, NULL, suggestion);
}

void report_return_type_mismatch(int line, int col, const char *function_name,
                                 DataType expected_type, DataType actual_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Function '%s' return type mismatch: expected %s, but got %s",
             function_name, type_to_string(expected_type), type_to_string(actual_type));

    snprintf(suggestion, sizeof(suggestion),
             "Change the return value to %s or modify the function return type",
             type_to_string(expected_type));

    report_semantic_error_enhanced(SEM_RETURN_TYPE_MISMATCH, line, col, message, NULL, suggestion);
}

void report_no_return_statement(int line, int col, const char *function_name, DataType return_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Function '%s' with return type %s must have a return statement",
             function_name, type_to_string(return_type));

    snprintf(suggestion, sizeof(suggestion),
             "Add a return statement with a %s value", type_to_string(return_type));

    report_semantic_error_enhanced(SEM_NO_RETURN_STATEMENT, line, col, message, NULL, suggestion);
}

void report_unreachable_code(int line, int col, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Unreachable code detected after %s", context);
    snprintf(suggestion, sizeof(suggestion), "Remove the unreachable code or fix the control flow");

    report_semantic_error_enhanced(SEM_UNREACHABLE_CODE, line, col, message, NULL, suggestion);
}

void report_recursive_definition(int line, int col, const char *name, const char *definition_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Recursive definition detected for %s '%s'", definition_type, name);

    snprintf(suggestion, sizeof(suggestion),
             "Break the circular dependency or use forward declarations");

    report_semantic_error_enhanced(SEM_RECURSIVE_DEFINITION, line, col, message, NULL, suggestion);
}

void report_circular_inheritance(int line, int col, const char *class1, const char *class2)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Circular inheritance detected between classes '%s' and '%s'", class1, class2);

    snprintf(suggestion, sizeof(suggestion),
             "Remove the circular inheritance by redesigning the class hierarchy");

    report_semantic_error_enhanced(SEM_CIRCULAR_INHERITANCE, line, col, message, NULL, suggestion);
}

void report_invalid_inheritance(int line, int col, const char *child, const char *parent, const char *reason)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid inheritance: class '%s' cannot inherit from '%s' - %s", child, parent, reason);

    snprintf(suggestion, sizeof(suggestion),
             "Choose a valid parent class or make the parent class inheritable");

    report_semantic_error_enhanced(SEM_INVALID_INHERITANCE, line, col, message, NULL, suggestion);
}

void report_scope_error(int line, int col, const char *identifier, const char *scope_context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Identifier '%s' is not accessible in %s scope", identifier, scope_context);

    snprintf(suggestion, sizeof(suggestion),
             "Check the variable scope or declare '%s' in the current scope", identifier);

    report_semantic_error_enhanced(SEM_SCOPE_ERROR, line, col, message, NULL, suggestion);
}

void report_variable_not_initialized(int line, int col, const char *variable_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Variable '%s' used without initialization", variable_name);

    snprintf(suggestion, sizeof(suggestion),
             "Initialize variable '%s' before using it", variable_name);

    report_semantic_error_enhanced(SEM_VARIABLE_NOT_INITIALIZED, line, col, message, NULL, suggestion);
}

void report_constant_assignment(int line, int col, const char *constant_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Cannot assign to constant '%s'", constant_name);

    snprintf(suggestion, sizeof(suggestion),
             "Constants cannot be modified after initialization");

    report_semantic_error_enhanced(SEM_CONSTANT_ASSIGNMENT, line, col, message, NULL, suggestion);
}

void report_array_bounds_error(int line, int col, const char *array_name, int index, int array_size)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Array bounds error: index %d out of range for array '%s' (size: %d)",
             index, array_name, array_size);

    snprintf(suggestion, sizeof(suggestion),
             "Use an index between 0 and %d for array '%s'", array_size - 1, array_name);

    report_semantic_error_enhanced(SEM_ARRAY_BOUNDS_ERROR, line, col, message, NULL, suggestion);
}

void report_memory_access_error(int line, int col, const char *operation, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid memory access during %s: %s", operation, context);

    snprintf(suggestion, sizeof(suggestion),
             "Check pointer validity and memory allocation");

    report_semantic_error_enhanced(SEM_MEMORY_ACCESS_ERROR, line, col, message, NULL, suggestion);
}

void report_condition_type_error(int line, int col, DataType condition_type, const char *statement_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid condition type %s in %s statement - expected boolean",
             type_to_string(condition_type), statement_type);

    snprintf(suggestion, sizeof(suggestion),
             "Use a boolean expression or comparison as the condition");

    report_semantic_error_enhanced(SEM_CONDITION_TYPE_ERROR, line, col, message, NULL, suggestion);
}

void report_break_outside_loop(int line, int col)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Break statement outside of loop");
    snprintf(suggestion, sizeof(suggestion), "Break statements can only be used inside loops or switch statements");

    report_semantic_error_enhanced(SEM_BREAK_OUTSIDE_LOOP, line, col, message, NULL, suggestion);
}

void report_continue_outside_loop(int line, int col)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Continue statement outside of loop");
    snprintf(suggestion, sizeof(suggestion), "Continue statements can only be used inside loops");

    report_semantic_error_enhanced(SEM_CONTINUE_OUTSIDE_LOOP, line, col, message, NULL, suggestion);
}

void report_invalid_control_flow(int line, int col, const char *statement, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Invalid control flow: %s statement in %s context", statement, context);

    snprintf(suggestion, sizeof(suggestion),
             "Review the control flow structure and statement placement");

    report_semantic_error_enhanced(SEM_INVALID_CONTROL_FLOW, line, col, message, NULL, suggestion);
}

void report_constructor_error(int line, int col, const char *class_name, const char *error_details)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Constructor error in class '%s': %s", class_name, error_details);

    snprintf(suggestion, sizeof(suggestion),
             "Check constructor parameters and initialization");

    report_semantic_error_enhanced(SEM_CONSTRUCTOR_ERROR, line, col, message, NULL, suggestion);
}

void report_redefinition_error(int line, int col, const char *name, const char *entity_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message),
             "Redefinition of %s '%s'", entity_type, name);

    snprintf(suggestion, sizeof(suggestion),
             "Remove the duplicate definition or use a different name");

    report_semantic_error_enhanced(SEM_REDEFINITION_ERROR, line, col, message, NULL, suggestion);
}

// Comprehensive Lexical Error Reporting Functions

void report_invalid_token(int line, int col, const char *token)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid token: '%s'", token);
    snprintf(suggestion, sizeof(suggestion), "Check token spelling and language syntax rules");

    report_lexical_error(LEX_INVALID_TOKEN, line, col, message, token, suggestion);
}

void report_invalid_number(int line, int col, const char *number_str)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid number format: '%s'", number_str);
    snprintf(suggestion, sizeof(suggestion), "Use valid integer format (no leading zeros except for '0')");

    report_lexical_error(LEX_INVALID_NUMBER, line, col, message, number_str, suggestion);
}

void report_invalid_float(int line, int col, const char *float_str)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid floating-point number: '%s'", float_str);
    snprintf(suggestion, sizeof(suggestion), "Use format like '123.45' or '0.123' (avoid trailing zeros after decimal)");

    report_lexical_error(LEX_INVALID_FLOAT, line, col, message, float_str, suggestion);
}

void report_invalid_identifier(int line, int col, const char *identifier)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid identifier: '%s'", identifier);
    snprintf(suggestion, sizeof(suggestion), "Identifiers must start with a letter and contain only letters, digits, and underscores");

    report_lexical_error(LEX_INVALID_IDENTIFIER, line, col, message, identifier, suggestion);
}

void report_unterminated_comment(int line, int col)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Unterminated comment starting at line %d", line);
    snprintf(suggestion, sizeof(suggestion), "Add closing '*/' to terminate the comment");

    report_lexical_error(LEX_UNTERMINATED_COMMENT, line, col, message, "/*", suggestion);
}

void report_unterminated_string(int line, int col, const char *string_start)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Unterminated string literal");
    snprintf(suggestion, sizeof(suggestion), "Add closing quote to terminate the string");

    report_lexical_error(LEX_UNTERMINATED_STRING, line, col, message, string_start, suggestion);
}

void report_invalid_character(int line, int col, char invalid_char)
{
    char message[256];
    char suggestion[256];
    char char_str[4];

    snprintf(char_str, sizeof(char_str), "%c", invalid_char);
    snprintf(message, sizeof(message), "Invalid character in source code: '%c' (ASCII %d)", invalid_char, (int)invalid_char);
    snprintf(suggestion, sizeof(suggestion), "Remove or replace the invalid character");

    report_lexical_error(LEX_INVALID_CHARACTER, line, col, message, char_str, suggestion);
}

void report_double_semicolon(int line, int col)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Double semicolon detected");
    snprintf(suggestion, sizeof(suggestion), "Remove the extra semicolon");

    report_lexical_error(LEX_DOUBLE_SEMICOLON, line, col, message, ";;", suggestion);
}

// Comprehensive Syntax Error Reporting Functions

void report_missing_token(int line, int col, const char *expected_token, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Missing %s in %s", expected_token, context);
    snprintf(suggestion, sizeof(suggestion), "Add the required %s", expected_token);

    report_syntax_error_enhanced(SYNTAX_MISSING_TOKEN, line, col, message, expected_token, "EOF or other token", suggestion);
}

void report_unexpected_token(int line, int col, const char *found_token, const char *expected_tokens)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Unexpected token");
    snprintf(suggestion, sizeof(suggestion), "Replace with one of the expected tokens or check syntax");

    report_syntax_error_enhanced(SYNTAX_UNEXPECTED_TOKEN, line, col, message, expected_tokens, found_token, suggestion);
}

void report_missing_semicolon(int line, int col, const char *statement_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Missing semicolon after %s", statement_type);
    snprintf(suggestion, sizeof(suggestion), "Add a semicolon (;) at the end of the statement");

    report_syntax_error_enhanced(SYNTAX_MISSING_SEMICOLON, line, col, message, ";", "newline or other", suggestion);
}

void report_missing_brace(int line, int col, const char *brace_type, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Missing %s brace in %s", brace_type, context);
    snprintf(suggestion, sizeof(suggestion), "Add the missing %s brace", brace_type);

    report_syntax_error_enhanced(SYNTAX_MISSING_BRACE, line, col, message, brace_type, "other token", suggestion);
}

void report_missing_parenthesis(int line, int col, const char *paren_type, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Missing %s parenthesis in %s", paren_type, context);
    snprintf(suggestion, sizeof(suggestion), "Add the missing %s parenthesis", paren_type);

    report_syntax_error_enhanced(SYNTAX_MISSING_PARENTHESIS, line, col, message, paren_type, "other token", suggestion);
}

void report_missing_bracket(int line, int col, const char *bracket_type, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Missing %s bracket in %s", bracket_type, context);
    snprintf(suggestion, sizeof(suggestion), "Add the missing %s bracket", bracket_type);

    report_syntax_error_enhanced(SYNTAX_MISSING_BRACKET, line, col, message, bracket_type, "other token", suggestion);
}

void report_invalid_expression(int line, int col, const char *expression_context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid expression in %s", expression_context);
    snprintf(suggestion, sizeof(suggestion), "Check expression syntax and operator precedence");

    report_syntax_error_enhanced(SYNTAX_INVALID_EXPRESSION, line, col, message, "valid expression", "invalid syntax", suggestion);
}

void report_invalid_statement(int line, int col, const char *statement_context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid statement: %s", statement_context);
    snprintf(suggestion, sizeof(suggestion), "Check statement syntax and structure");

    report_syntax_error_enhanced(SYNTAX_INVALID_STATEMENT, line, col, message, "valid statement", statement_context, suggestion);
}

void report_invalid_declaration(int line, int col, const char *declaration_type)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid %s declaration", declaration_type);
    snprintf(suggestion, sizeof(suggestion), "Check %s declaration syntax", declaration_type);

    report_syntax_error_enhanced(SYNTAX_INVALID_DECLARATION, line, col, message, "valid declaration", "invalid syntax", suggestion);
}

void report_malformed_function(int line, int col, const char *function_name, const char *issue)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Malformed function '%s': %s", function_name, issue);
    snprintf(suggestion, sizeof(suggestion), "Check function declaration syntax including parameters and return type");

    report_syntax_error_enhanced(SYNTAX_MALFORMED_FUNCTION, line, col, message, "valid function syntax", issue, suggestion);
}

void report_malformed_class(int line, int col, const char *class_name, const char *issue)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Malformed class '%s': %s", class_name, issue);
    snprintf(suggestion, sizeof(suggestion), "Check class declaration syntax including braces and members");

    report_syntax_error_enhanced(SYNTAX_MALFORMED_CLASS, line, col, message, "valid class syntax", issue, suggestion);
}

void report_invalid_operator(int line, int col, const char *operator_str, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Invalid operator '%s' in %s", operator_str, context);
    snprintf(suggestion, sizeof(suggestion), "Use a valid operator for this context");

    report_syntax_error_enhanced(SYNTAX_INVALID_OPERATOR, line, col, message, "valid operator", operator_str, suggestion);
}

// Utility Functions
char *get_error_type_string(ErrorType type)
{
    if (type >= 0 && type < sizeof(error_type_names) / sizeof(error_type_names[0]))
    {
        return (char *)error_type_names[type];
    }
    return "UNKNOWN_ERROR";
}

char *get_error_code(ErrorType type)
{
    if (type >= 0 && type < sizeof(error_codes) / sizeof(error_codes[0]))
    {
        return (char *)error_codes[type];
    }
    return "UNK";
}

const char *get_severity_string(ErrorSeverity severity)
{
    switch (severity)
    {
    case ERROR_INFO:
        return "Info";
    case ERROR_WARNING:
        return "Warning";
    case ERROR_ERROR:
        return "Error";
    case ERROR_FATAL:
        return "Fatal";
    default:
        return "Unknown";
    }
}

const char *get_severity_icon(ErrorSeverity severity)
{
    switch (severity)
    {
    case ERROR_INFO:
        return "[INFO]";
    case ERROR_WARNING:
        return "[WARNING]";
    case ERROR_ERROR:
        return "[ERROR]";
    case ERROR_FATAL:
        return "[FATAL]";
    default:
        return "[UNKNOWN]";
    }
}

// Error Analysis Functions
int has_fatal_errors(void)
{
    if (!global_error_manager)
        return 0;

    // Check for fatal syntax errors or too many errors
    return (global_error_manager->syntax_error_count > 0) ||
           (global_error_manager->semantic_error_count >= global_error_manager->max_errors);
}

int should_continue_analysis(void)
{
    if (!global_error_manager)
        return 1;

    return global_error_manager->semantic_error_count < global_error_manager->max_errors;
}

// Error Count Functions
int get_lexical_error_count(void)
{
    return global_error_manager ? global_error_manager->lexical_error_count : 0;
}

int get_syntax_error_count(void)
{
    return global_error_manager ? global_error_manager->syntax_error_count : 0;
}

int get_semantic_error_count(void)
{
    return global_error_manager ? global_error_manager->semantic_error_count : 0;
}

int get_total_error_count(void)
{
    if (!global_error_manager)
        return 0;

    return global_error_manager->lexical_error_count +
           global_error_manager->syntax_error_count +
           global_error_manager->semantic_error_count;
}

// Context extraction function
char *get_detailed_context(const char *filename, int line, int col, int context_lines)
{
    if (!filename)
        return NULL;

    // Mark unused parameter to avoid warnings
    (void)col;

    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;

    char *context = malloc(1024);
    if (!context)
    {
        fclose(file);
        return NULL;
    }

    context[0] = '\0';
    char line_buffer[256];
    int current_line = 1;
    int start_line = (line > context_lines) ? line - context_lines : 1;
    int end_line = line + context_lines;

    while (fgets(line_buffer, sizeof(line_buffer), file) && current_line <= end_line)
    {
        if (current_line >= start_line)
        {
            char formatted_line[300];
            if (current_line == line)
            {
                snprintf(formatted_line, sizeof(formatted_line), " >> %3d: %s", current_line, line_buffer);
            }
            else
            {
                snprintf(formatted_line, sizeof(formatted_line), "    %3d: %s", current_line, line_buffer);
            }
            strncat(context, formatted_line, 1024 - strlen(context) - 1);
        }
        current_line++;
    }

    fclose(file);
    return context;
}

// Error Output Functions
void print_error_summary(FILE *output)
{
    if (!global_error_manager || !output)
        return;

    fprintf(output, "\n");
    fprintf(output, "================================================================================\n");
    fprintf(output, "                           ERROR SUMMARY REPORT                                \n");
    fprintf(output, "================================================================================\n");

    fprintf(output, "┌─ Error Statistics ─────────────────────────────────────────────────────────┐\n");
    fprintf(output, "│ %-25s │ %5d │ %-40s │\n", "Lexical Errors",
            global_error_manager->lexical_error_count, "Invalid tokens, numbers, identifiers");
    fprintf(output, "│ %-25s │ %5d │ %-40s │\n", "Syntax Errors",
            global_error_manager->syntax_error_count, "Grammar violations, missing tokens");
    fprintf(output, "│ %-25s │ %5d │ %-40s │\n", "Semantic Errors",
            global_error_manager->semantic_error_count, "Type mismatches, undefined symbols");
    fprintf(output, "│ %-25s │ %5d │ %-40s │\n", "Warnings",
            global_error_manager->warning_count, "Non-critical issues");
    fprintf(output, "└────────────────────────────────────────────────────────────────────────────┘\n");

    int total_errors = global_error_manager->lexical_error_count +
                       global_error_manager->syntax_error_count +
                       global_error_manager->semantic_error_count;

    if (total_errors == 0)
    {
        fprintf(output, "\nSUCCESS: No errors found! Code analysis completed successfully.\n");
    }
    else
    {
        fprintf(output, "\nFAILED: Found %d error(s) that need to be fixed before compilation.\n", total_errors);
    }
}

void print_detailed_error_report(FILE *output)
{
    if (!global_error_manager || !output)
        return;

    fprintf(output, "\n");
    fprintf(output, "================================================================================\n");
    fprintf(output, "                        COMPREHENSIVE ERROR REPORT                             \n");
    fprintf(output, "================================================================================\n");

    int error_number = 1;

    // Print Lexical Errors
    if (global_error_manager->lexical_errors)
    {
        fprintf(output, "\nLEXICAL ERRORS:\n");
        fprintf(output, "────────────────────────────────────────────────────────────────────────────\n");

        ComprehensiveError *error = global_error_manager->lexical_errors;
        while (error)
        {
            fprintf(output, "%s LEXICAL ERROR #%d\n", get_severity_icon(error->severity), error_number++);
            fprintf(output, "   ├─ Code: %s\n", error->error_code);
            fprintf(output, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            if (error->filename)
            {
                fprintf(output, "   ├─ File: %s\n", error->filename);
            }
            fprintf(output, "   ├─ Description: %s\n", error->message);
            if (error->found)
            {
                fprintf(output, "   ├─ Found: '%s'\n", error->found);
            }
            if (error->suggestion)
            {
                fprintf(output, "   Suggestion: %s\n", error->suggestion);
            }
            fprintf(output, "\n");
            error = error->next;
        }
    }

    // Print Syntax Errors
    if (global_error_manager->syntax_errors)
    {
        fprintf(output, "\nSYNTAX ERRORS:\n");
        fprintf(output, "────────────────────────────────────────────────────────────────────────────\n");

        ComprehensiveError *error = global_error_manager->syntax_errors;
        while (error)
        {
            fprintf(output, "%s SYNTAX ERROR #%d\n", get_severity_icon(error->severity), error_number++);
            fprintf(output, "   ├─ Code: %s\n", error->error_code);
            fprintf(output, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            if (error->filename)
            {
                fprintf(output, "   ├─ File: %s\n", error->filename);
            }
            fprintf(output, "   ├─ Description: %s\n", error->message);
            if (error->expected && error->found)
            {
                fprintf(output, "   ├─ Expected: %s\n", error->expected);
                fprintf(output, "   ├─ Found: %s\n", error->found);
            }
            if (error->context)
            {
                fprintf(output, "   ├─ Source Context:\n%s", error->context);
            }
            if (error->suggestion)
            {
                fprintf(output, "   Suggestion: %s\n", error->suggestion);
            }
            fprintf(output, "\n");
            error = error->next;
        }
    }

    // Print Semantic Errors
    if (global_error_manager->semantic_errors)
    {
        fprintf(output, "\nSEMANTIC ERRORS:\n");
        fprintf(output, "────────────────────────────────────────────────────────────────────────────\n");

        ComprehensiveError *error = global_error_manager->semantic_errors;
        while (error)
        {
            fprintf(output, "%s SEMANTIC ERROR #%d\n", get_severity_icon(error->severity), error_number++);
            fprintf(output, "   ├─ Code: %s\n", error->error_code);
            fprintf(output, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            if (error->filename)
            {
                fprintf(output, "   ├─ File: %s\n", error->filename);
            }
            fprintf(output, "   ├─ Description: %s\n", error->message);
            if (error->context)
            {
                fprintf(output, "   ├─ Context: %s\n", error->context);
            }
            if (error->suggestion)
            {
                fprintf(output, "   Suggestion: %s\n", error->suggestion);
            }
            fprintf(output, "\n");
            error = error->next;
        }
    }
}

void write_comprehensive_error_report(FILE *file)
{
    if (!global_error_manager || !file)
        return;

    int syntax_errors = global_error_manager->syntax_error_count;
    int semantic_errors = global_error_manager->semantic_error_count;
    int warning_count = global_error_manager->warning_count;

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
            syntax_errors, syntax_errors > 0 ? "FAILED                       " : "PASSED                       ");
    fprintf(file, "│ Semantic Errors      │ %5d │   HIGH   │ %s │\n",
            semantic_errors, semantic_errors > 0 ? "FAILED                       " : "PASSED                       ");
    fprintf(file, "│ Semantic Warnings    │ %5d │   LOW    │ %s │\n",
            warning_count, warning_count > 0 ? "WARNINGS                     " : "CLEAN                        ");
    fprintf(file, "└─────────────────────────────────────────────────────────────────────────────┘\n\n");

    // Print syntax errors if any
    if (syntax_errors > 0 && global_error_manager->syntax_errors)
    {
        fprintf(file, "================================================================================\n");
        fprintf(file, "                              SYNTAX ERRORS                                    \n");
        fprintf(file, "================================================================================\n\n");

        ComprehensiveError *error = global_error_manager->syntax_errors;
        int error_num = 1;
        while (error)
        {
            fprintf(file, "[ERROR] SYNTAX ERROR #%d\n", error_num++);
            fprintf(file, "   ├─ Location: Line %d, Column %d\n", error->line, error->column);
            fprintf(file, "   ├─ Type: %s\n", error->error_code ? error->error_code : "SYNTAX_ERROR");
            fprintf(file, "   ├─ Description: %s\n", error->message);

            // Show source context
            if (error->filename && error->line > 0)
            {
                fprintf(file, "   ├─ Source Context:\n");
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
    if (semantic_errors > 0 && global_error_manager->semantic_errors)
    {
        fprintf(file, "================================================================================\n");
        fprintf(file, "                             SEMANTIC ERRORS                                   \n");
        fprintf(file, "================================================================================\n\n");

        ComprehensiveError *error = global_error_manager->semantic_errors;
        int error_num = 1;
        while (error)
        {
            fprintf(file, "[ERROR] SEMANTIC ERROR #%d\n", error_num++);
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
        fprintf(file, "SYNTAX ERROR RESOLUTION:\n");
        fprintf(file, "   1. Fix syntax errors first - they prevent proper parsing\n");
        fprintf(file, "   2. Check for missing semicolons, braces, or parentheses\n");
        fprintf(file, "   3. Verify proper token spelling and casing\n");
        fprintf(file, "   4. Ensure operators and literals follow language rules\n\n");
    }

    if (semantic_errors > 0)
    {
        fprintf(file, "SEMANTIC ERROR RESOLUTION:\n");
        fprintf(file, "   1. Verify variable declarations before use\n");
        fprintf(file, "   2. Check type compatibility in assignments\n");
        fprintf(file, "   3. Ensure function calls match signatures\n");
        fprintf(file, "   4. Validate scope and visibility rules\n\n");
    }

    fprintf(file, "ADDITIONAL RESOURCES:\n");
    fprintf(file, "   • Language Grammar: Check semantic.c for syntax rules and grammar implementation\n");
    fprintf(file, "   • Semantic Rules: Review TMA03 specification document\n");
    fprintf(file, "   • Test Cases: Examine valid*.txt files for examples\n\n");

    fprintf(file, "================================================================================\n");
}

void write_error_report_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Warning: Cannot create error report file '%s'\n", filename);
        return;
    }

    write_comprehensive_error_report(file);

    fclose(file);
}

// Validation Functions
int validate_identifier(const char *id)
{
    if (!id || !*id)
        return 0;

    // Must start with letter
    if (!((id[0] >= 'a' && id[0] <= 'z') || (id[0] >= 'A' && id[0] <= 'Z')))
    {
        return 0;
    }

    // Rest must be alphanumeric or underscore
    for (int i = 1; id[i]; i++)
    {
        if (!((id[i] >= 'a' && id[i] <= 'z') ||
              (id[i] >= 'A' && id[i] <= 'Z') ||
              (id[i] >= '0' && id[i] <= '9') ||
              id[i] == '_'))
        {
            return 0;
        }
    }

    return 1;
}

int validate_number_format(const char *number)
{
    if (!number || !*number)
        return 0;

    // Cannot start with 0 unless it's just "0"
    if (number[0] == '0' && number[1] != '\0')
    {
        return 0;
    }

    // Must be all digits
    for (int i = 0; number[i]; i++)
    {
        if (number[i] < '0' || number[i] > '9')
        {
            return 0;
        }
    }

    return 1;
}

int validate_float_format(const char *float_str)
{
    if (!float_str || !*float_str)
        return 0;

    // Simple validation - more complex rules can be added
    int has_decimal = 0;
    int digits_after_decimal = 0;

    for (int i = 0; float_str[i]; i++)
    {
        if (float_str[i] == '.')
        {
            if (has_decimal)
                return 0; // Multiple decimal points
            has_decimal = 1;
        }
        else if (float_str[i] >= '0' && float_str[i] <= '9')
        {
            if (has_decimal)
                digits_after_decimal++;
        }
        else
        {
            return 0; // Invalid character
        }
    }

    // Cannot end with .0 followed by more zeros
    if (has_decimal && digits_after_decimal > 1)
    {
        // Check for trailing zeros after decimal (except .0)
        const char *decimal_pos = strchr(float_str, '.');
        if (decimal_pos && strlen(decimal_pos) > 2)
        {
            // More complex validation can be added here
        }
    }

    return 1;
}

// Initialize the global error manager if not already created
void init_error_manager(void)
{
    if (!global_error_manager)
    {
        global_error_manager = create_error_manager();
    }
}

// Initialize global error log file for real-time error logging
void init_error_log_file(const char *input_file)
{
    if (global_error_log_file)
    {
        fclose(global_error_log_file);
    }

    char filename[512];
    char base_name[256];
    char *dot_pos = strrchr(input_file, '.');

    if (dot_pos)
    {
        strncpy(base_name, input_file, dot_pos - input_file);
        base_name[dot_pos - input_file] = '\0';
    }
    else
    {
        strcpy(base_name, input_file);
    }

    snprintf(filename, sizeof(filename), "%s_error_log.txt", base_name);
    global_error_log_file = fopen(filename, "w");

    if (global_error_log_file)
    {
        fprintf(global_error_log_file, "================================================================================\n");
        fprintf(global_error_log_file, "                     DESIGN PROJECT - COMPREHENSIVE ERROR LOG              \n");
        fprintf(global_error_log_file, "================================================================================\n");
        char timestamp[100];
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(timestamp, sizeof(timestamp), "%B %d, %Y at %H:%M:%S", timeinfo);
        fprintf(global_error_log_file, "Generated: %s\n", timestamp);
        fprintf(global_error_log_file, "Input File: %s\n\n", input_file);
        fprintf(global_error_log_file, "REAL-TIME COMPILATION LOG:\n");
        fprintf(global_error_log_file, "────────────────────────────────────────────────────────────────────────────────\n\n");
        fflush(global_error_log_file);
    }
}

// Close global error log file
void close_error_log_file(void)
{
    if (global_error_log_file)
    {
        // Add comprehensive error summary like terminal output
        fprintf(global_error_log_file, "\n\n");
        fprintf(global_error_log_file, "================================================================================\n");
        fprintf(global_error_log_file, "                           ERROR SUMMARY REPORT                                \n");
        fprintf(global_error_log_file, "================================================================================\n");
        fprintf(global_error_log_file, "┌─ Error Statistics ─────────────────────────────────────────────────────────┐\n");
        fprintf(global_error_log_file, "│ Lexical Errors            │ %5d │ Invalid tokens, numbers, identifiers     │\n", get_lexical_error_count());
        fprintf(global_error_log_file, "│ Syntax Errors             │ %5d │ Grammar violations, missing tokens       │\n", get_syntax_error_count());
        fprintf(global_error_log_file, "│ Semantic Errors           │ %5d │ Type mismatches, undefined symbols       │\n", get_semantic_error_count());
        fprintf(global_error_log_file, "│ Warnings                  │ %5d │ Non-critical issues                      │\n", get_warning_count());
        fprintf(global_error_log_file, "└────────────────────────────────────────────────────────────────────────────┘\n\n");

        int total_errors = get_total_error_count();
        if (total_errors == 0)
        {
            fprintf(global_error_log_file, "SUCCESS: No errors found! Code analysis completed successfully.\n\n");
        }
        else
        {
            fprintf(global_error_log_file, "FAILED: Found %d error(s) that need to be fixed before compilation.\n\n", total_errors);
        }

        // Add comprehensive error report like terminal
        if (global_error_manager && (global_error_manager->lexical_errors || global_error_manager->syntax_errors || global_error_manager->semantic_errors))
        {
            write_comprehensive_error_report(global_error_log_file);
        }

        fprintf(global_error_log_file, "\n================================================================================\n");
        fprintf(global_error_log_file, "                              END OF ERROR LOG                               \n");
        fprintf(global_error_log_file, "================================================================================\n");
        fclose(global_error_log_file);
        global_error_log_file = NULL;
    }
}

// Error Recovery and Analysis Functions

void suggest_error_fix(ErrorType type, const char *context, char *suggestion, size_t suggestion_size)
{
    if (!suggestion || suggestion_size == 0)
        return;

    switch (type)
    {
    case SEM_TYPE_MISMATCH:
        snprintf(suggestion, suggestion_size, "Check variable types and use appropriate type conversion");
        break;
    case SEM_UNDEFINED_VARIABLE:
        snprintf(suggestion, suggestion_size, "Declare the variable before using it");
        break;
    case SEM_UNDEFINED_FUNCTION:
        snprintf(suggestion, suggestion_size, "Define the function or check function name spelling");
        break;
    case SYNTAX_MISSING_SEMICOLON:
        snprintf(suggestion, suggestion_size, "Add a semicolon (;) at the end of the statement");
        break;
    case SYNTAX_MISSING_BRACE:
        snprintf(suggestion, suggestion_size, "Add the missing opening or closing brace");
        break;
    case LEX_INVALID_IDENTIFIER:
        snprintf(suggestion, suggestion_size, "Use a valid identifier (start with letter, use letters/digits/underscore)");
        break;
    default:
        if (context)
        {
            snprintf(suggestion, suggestion_size, "Review the %s for syntax and semantic correctness", context);
        }
        else
        {
            snprintf(suggestion, suggestion_size, "Check the code for syntax and semantic errors");
        }
        break;
    }
}

char *extract_source_line(const char *filename, int line_num)
{
    if (!filename || line_num <= 0)
        return NULL;

    FILE *file = fopen(filename, "r");
    if (!file)
        return NULL;

    char *line_buffer = malloc(512);
    if (!line_buffer)
    {
        fclose(file);
        return NULL;
    }

    int current_line = 1;
    while (fgets(line_buffer, 512, file))
    {
        if (current_line == line_num)
        {
            fclose(file);
            // Remove newline if present
            char *newline = strchr(line_buffer, '\n');
            if (newline)
                *newline = '\0';
            return line_buffer;
        }
        current_line++;
    }

    fclose(file);
    free(line_buffer);
    return NULL;
}

void attempt_error_recovery(ErrorType type)
{
    // Basic error recovery strategies
    switch (type)
    {
    case SYNTAX_MISSING_SEMICOLON:
        // Could insert a semicolon in the token stream
        fprintf(stderr, "Recovery: Assuming missing semicolon\n");
        break;
    case SYNTAX_MISSING_BRACE:
        // Could try to match braces
        fprintf(stderr, "Recovery: Attempting to balance braces\n");
        break;
    case SYNTAX_UNEXPECTED_TOKEN:
        // Could skip the unexpected token
        fprintf(stderr, "Recovery: Skipping unexpected token\n");
        break;
    default:
        fprintf(stderr, "Recovery: No specific recovery strategy available\n");
        break;
    }
}

void skip_to_synchronization_point(void)
{
    // This would be implemented by the parser to skip to known synchronization points
    // such as semicolons, braces, etc.
    fprintf(stderr, "Skipping to next synchronization point...\n");
}

int find_recovery_token(void)
{
    // This would be implemented by the parser to find appropriate recovery tokens
    // Return the token type that can be used for recovery
    return 0; // Placeholder
}

int compare_errors_by_line(const void *a, const void *b)
{
    const ComprehensiveError *error_a = (const ComprehensiveError *)a;
    const ComprehensiveError *error_b = (const ComprehensiveError *)b;

    if (error_a->line != error_b->line)
    {
        return error_a->line - error_b->line;
    }
    return error_a->column - error_b->column;
}

void sort_errors_by_location(void)
{
    // This would sort all errors by their line and column positions
    // Implementation would need to convert linked lists to arrays, sort, then convert back
    // For now, just a placeholder
    if (global_error_manager)
    {
        fprintf(stderr, "Sorting errors by location...\n");
        // Implementation would go here
    }
}

void highlight_error_position(char *line_text, int column)
{
    if (!line_text || column <= 0)
        return;

    printf("%s\n", line_text);

    // Print spaces up to the error column
    for (int i = 1; i < column; i++)
    {
        if (i < (int)strlen(line_text) && line_text[i - 1] == '\t')
        {
            printf("\t");
        }
        else
        {
            printf(" ");
        }
    }
    printf("^\n");
}

char *format_error_message(ComprehensiveError *error)
{
    if (!error)
        return NULL;

    char *formatted = malloc(1024);
    if (!formatted)
        return NULL;

    snprintf(formatted, 1024,
             "[%s] %s Error at line %d:%d: %s",
             error->error_code,
             get_severity_string(error->severity),
             error->line,
             error->column,
             error->message);

    return formatted;
}

int validate_operator_usage(const char *operator, DataType left, DataType right)
{
    if (!operator)
        return 0;

    // Basic validation - more sophisticated rules can be added
    if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0 ||
        strcmp(operator, "*") == 0 || strcmp(operator, "/") == 0)
    {
        // Arithmetic operators - both operands should be numeric
        return (left == TYPE_INTEGER || left == TYPE_FLOAT) &&
               (right == TYPE_INTEGER || right == TYPE_FLOAT);
    }
    else if (strcmp(operator, "==") == 0 || strcmp(operator, "!=") == 0)
    {
        // Equality operators - types should be compatible
        return left == right ||
               ((left == TYPE_INTEGER || left == TYPE_FLOAT) && (right == TYPE_INTEGER || right == TYPE_FLOAT));
    }
    else if (strcmp(operator, "<") == 0 || strcmp(operator, ">") == 0 ||
             strcmp(operator, "<=") == 0 || strcmp(operator, ">=") == 0)
    {
        // Comparison operators - both operands should be numeric
        return (left == TYPE_INTEGER || left == TYPE_FLOAT) &&
               (right == TYPE_INTEGER || right == TYPE_FLOAT);
    }
    else if (strcmp(operator, "&&") == 0 || strcmp(operator, "||") == 0)
    {
        // Logical operators - both operands should be compatible (no boolean type in this grammar)
        // For this language, we'll allow any non-void types in logical operations
        return left != TYPE_VOID && right != TYPE_VOID && left != TYPE_ERROR && right != TYPE_ERROR;
    }

    return 1; // Default to valid for unknown operators
}

// Enhanced Error Statistics and Reporting

void print_error_statistics(FILE *output)
{
    if (!global_error_manager || !output)
        return;

    fprintf(output, "\n");
    fprintf(output, "╔══════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(output, "║                            DETAILED ERROR STATISTICS                        ║\n");
    fprintf(output, "╠══════════════════════════════════════════════════════════════════════════════╣\n");

    int total_errors = global_error_manager->lexical_error_count +
                       global_error_manager->syntax_error_count +
                       global_error_manager->semantic_error_count;

    fprintf(output, "║ Error Category        │ Count │ Percentage │ Status                       ║\n");
    fprintf(output, "╠═══════════════════════╪═══════╪════════════╪══════════════════════════════╣\n");

    double lex_pct = total_errors > 0 ? (global_error_manager->lexical_error_count * 100.0 / total_errors) : 0.0;
    double syn_pct = total_errors > 0 ? (global_error_manager->syntax_error_count * 100.0 / total_errors) : 0.0;
    double sem_pct = total_errors > 0 ? (global_error_manager->semantic_error_count * 100.0 / total_errors) : 0.0;

    fprintf(output, "║ Lexical Errors        │ %5d │    %6.1f%% │ %-28s ║\n",
            global_error_manager->lexical_error_count, lex_pct,
            global_error_manager->lexical_error_count > 0 ? "❌ Issues Found" : "✅ Clean");

    fprintf(output, "║ Syntax Errors         │ %5d │    %6.1f%% │ %-28s ║\n",
            global_error_manager->syntax_error_count, syn_pct,
            global_error_manager->syntax_error_count > 0 ? "❌ Issues Found" : "✅ Clean");

    fprintf(output, "║ Semantic Errors       │ %5d │    %6.1f%% │ %-28s ║\n",
            global_error_manager->semantic_error_count, sem_pct,
            global_error_manager->semantic_error_count > 0 ? "❌ Issues Found" : "✅ Clean");

    fprintf(output, "║ Warnings              │ %5d │       N/A │ %-28s ║\n",
            global_error_manager->warning_count,
            global_error_manager->warning_count > 0 ? "⚠️  Warnings Present" : "✅ Clean");

    fprintf(output, "╠═══════════════════════╪═══════╪════════════╪══════════════════════════════╣\n");
    fprintf(output, "║ TOTAL ERRORS          │ %5d │    100.0%% │ %-28s ║\n",
            total_errors, total_errors > 0 ? "❌ Compilation Failed" : "✅ Success");
    fprintf(output, "╚═══════════════════════╧═══════╧════════════╧══════════════════════════════╝\n");

    if (total_errors > 0)
    {
        fprintf(output, "\n💡 RECOMMENDATION: Fix lexical errors first, then syntax errors, then semantic errors.\n");
        fprintf(output, "   This approach typically resolves cascading errors more efficiently.\n");
    }
}

// Wrapper functions for backward compatibility
void report_semantic_error(int line, int col, const char *message, const char *error_type)
{
    (void)error_type; // Suppress unused parameter warning
    report_semantic_error_enhanced(SEM_GENERAL_ERROR, line, col, message, NULL, NULL);
}

void report_semantic_error_detailed(int line, int col, const char *message, const char *error_type,
                                    const char *context, const char *suggestion)
{
    (void)error_type; // Suppress unused parameter warning
    report_semantic_error_enhanced(SEM_GENERAL_ERROR, line, col, message, context, suggestion);
}

// ============================================================================
// COMPREHENSIVE WARNING SYSTEM IMPLEMENTATION
// ============================================================================

// Global warning tracking
static int total_warnings_found = 0;

// Get warning count function
int get_warning_count(void)
{
    if (global_error_manager)
    {
        return global_error_manager->warning_count;
    }
    return total_warnings_found;
}

// Main warning analysis function
void perform_warning_analysis(ASTNode *root)
{
    if (!root)
        return;

    printf("🔍 Performing comprehensive warning analysis...\n");

    // Initialize warning count
    total_warnings_found = 0;

    // Check for various warning types
    check_unused_variables(global_scope);
    check_implicit_conversions(root);
    check_unreachable_code_warnings(root);
    check_uninitialized_variables(root, global_scope);
    check_shadowed_variables(global_scope);
    check_missing_returns(root);
    check_performance_issues(root);
    check_style_violations(root);

    // Update global warning count
    if (global_error_manager)
    {
        global_error_manager->warning_count = total_warnings_found;
    }

    printf("✅ Warning analysis complete. Found %d warnings.\n", total_warnings_found);
}

// Report all semantic warnings function
void report_semantic_warnings(void)
{
    printf("📋 Generating warning summary report...\n");

    if (total_warnings_found == 0)
    {
        printf("✨ No warnings found. Code is clean!\n");
    }
    else
    {
        printf("⚠️  Total warnings: %d\n", total_warnings_found);
        printf("   Recommendation: Review warnings to improve code quality.\n");
    }
}

// ============================================================================
// SPECIFIC WARNING DETECTION FUNCTIONS
// ============================================================================

// Check for unused variables
void check_unused_variables(SymbolTable *scope)
{
    if (!scope)
        return;

    Symbol *current = scope->symbols;
    while (current)
    {
        if (current->type == SYM_VAR && !current->is_used &&
            strcmp(current->name, "self") != 0)
        { // Skip 'self' parameter
            report_unused_variable_warning(current->line, current->column, current->name);
        }
        current = current->next;
    }

    // Recursively check child scopes
    SymbolTable *child = scope->children;
    while (child)
    {
        check_unused_variables(child);
        child = child->next_sibling;
    }
}

// Check for unused parameters
void check_unused_parameters(Symbol *function_symbol)
{
    if (!function_symbol || function_symbol->type != SYM_FUNC)
        return;

    // Check function parameters
    if (function_symbol->attributes.func.local_scope)
    {
        Symbol *param = function_symbol->attributes.func.local_scope->symbols;
        while (param)
        {
            if (param->type == SYM_PARAM && !param->is_used &&
                strcmp(param->name, "self") != 0)
            { // Skip 'self' parameter
                report_unused_parameter_warning(param->line, param->column,
                                                param->name, function_symbol->name);
            }
            param = param->next;
        }
    }
}

// Check for unused functions
void check_unused_functions(SymbolTable *scope)
{
    if (!scope)
        return;

    Symbol *current = scope->symbols;
    while (current)
    {
        if (current->type == SYM_FUNC && !current->is_used &&
            strcmp(current->name, "main") != 0)
        { // Skip main function
            report_unused_function_warning(current->line, current->column, current->name);
        }
        current = current->next;
    }

    // Recursively check child scopes
    SymbolTable *child = scope->children;
    while (child)
    {
        check_unused_functions(child);
        child = child->next_sibling;
    }
}

// Check for implicit conversions
void check_implicit_conversions(ASTNode *node)
{
    if (!node)
        return;

    if (node->type == AST_ASSIGN_STMT && node->child_count >= 2)
    {
        DataType lhs_type = get_expression_type(node->children[0]);
        DataType rhs_type = get_expression_type(node->children[1]);

        if (lhs_type != rhs_type && lhs_type != TYPE_ERROR && rhs_type != TYPE_ERROR)
        {
            // Check if it's a valid implicit conversion
            if ((lhs_type == TYPE_FLOAT && rhs_type == TYPE_INTEGER) ||
                (lhs_type == TYPE_INTEGER && rhs_type == TYPE_FLOAT))
            {
                report_implicit_conversion_warning(node->line, node->column, rhs_type, lhs_type);
            }
        }
    }

    // Check binary expressions for implicit conversions
    if (node->type == AST_BINARY_EXPR && node->child_count >= 2)
    {
        DataType left_type = get_expression_type(node->children[0]);
        DataType right_type = get_expression_type(node->children[1]);

        if (left_type != right_type && left_type != TYPE_ERROR && right_type != TYPE_ERROR)
        {
            if ((left_type == TYPE_FLOAT && right_type == TYPE_INTEGER) ||
                (left_type == TYPE_INTEGER && right_type == TYPE_FLOAT))
            {
                report_implicit_conversion_warning(node->line, node->column, right_type, left_type);
            }
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_implicit_conversions(node->children[i]);
    }
}

// Check for unreachable code
void check_unreachable_code_warnings(ASTNode *node)
{
    if (!node)
        return;

    // Check for statements after return statements
    if (node->type == AST_STAT_BLOCK || node->type == AST_FUNC_BODY)
    {
        int found_return = 0;
        for (int i = 0; i < node->child_count; i++)
        {
            if (found_return && node->children[i])
            {
                report_unreachable_code_warning(node->children[i]->line,
                                                node->children[i]->column,
                                                "Statement after return");
                break;
            }
            if (node->children[i] && node->children[i]->type == AST_RETURN_STMT)
            {
                found_return = 1;
            }
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_unreachable_code_warnings(node->children[i]);
    }
}

// Check for uninitialized variables
void check_uninitialized_variables(ASTNode *node, SymbolTable *scope)
{
    if (!node || !scope)
        return;

    // Check variable usage before initialization
    if (node->type == AST_VARIABLE && node->data.string_value)
    {
        Symbol *symbol = lookup_symbol_global(scope, node->data.string_value);
        if (symbol && symbol->type == SYM_VAR && !symbol->is_initialized)
        {
            report_uninitialized_variable_warning(node->line, node->column, symbol->name);
        }
    }

    // Mark variables as initialized in assignment statements
    if (node->type == AST_ASSIGN_STMT && node->child_count >= 1 &&
        node->children[0] && node->children[0]->type == AST_VARIABLE)
    {
        Symbol *symbol = lookup_symbol_global(scope, node->children[0]->data.string_value);
        if (symbol && symbol->type == SYM_VAR)
        {
            symbol->is_initialized = 1;
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_uninitialized_variables(node->children[i], scope);
    }
}

// Check for shadowed variables
void check_shadowed_variables(SymbolTable *scope)
{
    if (!scope || !scope->parent)
        return;

    Symbol *current = scope->symbols;
    while (current)
    {
        if (current->type == SYM_VAR || current->type == SYM_PARAM)
        {
            // Check if variable shadows one in outer scope
            Symbol *outer = lookup_symbol_global(scope->parent, current->name);
            if (outer && (outer->type == SYM_VAR || outer->type == SYM_PARAM))
            {
                report_shadowed_variable_warning(current->line, current->column,
                                                 current->name, scope->parent->scope_name);
            }
        }
        current = current->next;
    }

    // Recursively check child scopes
    SymbolTable *child = scope->children;
    while (child)
    {
        check_shadowed_variables(child);
        child = child->next_sibling;
    }
}

// Check for missing return statements
void check_missing_returns(ASTNode *node)
{
    if (!node)
        return;

    if (node->type == AST_FUNC_DEF && node->child_count >= 2)
    {
        // Get function header to check return type
        ASTNode *header = node->children[0];
        if (header && header->child_count >= 3)
        {
            ASTNode *return_type_node = header->children[2];
            if (return_type_node && return_type_node->data.string_value)
            {
                DataType return_type = string_to_data_type(return_type_node->data.string_value);
                if (return_type != TYPE_VOID)
                {
                    // Check if function body has return statement
                    ASTNode *body = node->children[1];
                    if (!has_return_statement(body))
                    {
                        Symbol *func_symbol = node->symbol;
                        const char *func_name = func_symbol ? func_symbol->name : "unknown";
                        report_missing_return_warning(node->line, node->column, func_name);
                    }
                }
            }
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_missing_returns(node->children[i]);
    }
}

// Check for performance issues
void check_performance_issues(ASTNode *node)
{
    if (!node)
        return;

    // Check for inefficient operations in loops
    if (node->type == AST_WHILE_STMT)
    {
        // Look for string concatenation operations in loop body
        if (node->child_count >= 2)
        {
            check_for_string_concat_in_loop(node->children[1], node->line, node->column);
        }
    }

    // Check for repeated function calls with same parameters
    check_for_repeated_calls(node);

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_performance_issues(node->children[i]);
    }
}

// Check for style violations
void check_style_violations(ASTNode *node)
{
    if (!node)
        return;

    // Check for empty statement blocks
    if ((node->type == AST_STAT_BLOCK || node->type == AST_FUNC_BODY) &&
        node->child_count == 0)
    {
        report_style_warning(node->line, node->column, "Empty statement block",
                             "Consider adding comments or removing unnecessary braces");
    }

    // Check for single-statement if blocks without braces (if applicable)
    if (node->type == AST_IF_STMT && node->child_count >= 2)
    {
        ASTNode *then_stmt = node->children[1];
        if (then_stmt && then_stmt->type != AST_STAT_BLOCK)
        {
            report_style_warning(node->line, node->column,
                                 "Single statement without braces",
                                 "Consider using braces for clarity");
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_style_violations(node->children[i]);
    }
}

// ============================================================================
// HELPER FUNCTIONS FOR WARNING DETECTION - FORWARD DECLARATIONS
// ============================================================================

// Forward declarations moved to prevent conflicts

// ============================================================================
// HELPER FUNCTIONS FOR WARNING DETECTION
// ============================================================================

// Check if function body has return statement
int has_return_statement(ASTNode *node)
{
    if (!node)
        return 0;

    if (node->type == AST_RETURN_STMT)
        return 1;

    // Check all children
    for (int i = 0; i < node->child_count; i++)
    {
        if (has_return_statement(node->children[i]))
        {
            return 1;
        }
    }
    return 0;
}

// Check for string concatenation in loops
void check_for_string_concat_in_loop(ASTNode *node, int loop_line, int loop_col)
{
    if (!node)
        return;

    // Look for string concatenation operations (simplified check)
    if (node->type == AST_BINARY_EXPR && node->data.operator &&
        strcmp(node->data.operator, "+") == 0)
    {
        // Check if operands are being concatenated (simplified heuristic)
        DataType left_type = get_expression_type(node->children[0]);
        DataType right_type = get_expression_type(node->children[1]);

        // For now, skip string concatenation check since TYPE_STRING is not defined
        // This could be enhanced when string types are added to the language
        if (left_type != TYPE_ERROR && right_type != TYPE_ERROR)
        {
            report_performance_warning(loop_line, loop_col,
                                       "Potential inefficient operation in loop",
                                       "Consider optimizing loop operations");
        }
    }

    // Recursively check children
    for (int i = 0; i < node->child_count; i++)
    {
        check_for_string_concat_in_loop(node->children[i], loop_line, loop_col);
    }
}

// Check for repeated function calls
void check_for_repeated_calls(ASTNode *node)
{
    // This is a simplified implementation
    // A full implementation would track function calls and compare parameters
    (void)node; // Suppress unused parameter warning for now
}

// ============================================================================
// WARNING REPORTING FUNCTIONS
// ============================================================================

void report_unused_variable_warning(int line, int col, const char *var_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Variable '%s' declared but never used", var_name);
    snprintf(suggestion, sizeof(suggestion), "Remove unused variable or add usage");

    report_warning(WARN_UNUSED_VARIABLE, line, col, message, suggestion);
    total_warnings_found++;
}

void report_unused_parameter_warning(int line, int col, const char *param_name, const char *func_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Parameter '%s' in function '%s' is never used",
             param_name, func_name);
    snprintf(suggestion, sizeof(suggestion), "Remove unused parameter or use it in function body");

    report_warning(WARN_UNUSED_PARAMETER, line, col, message, suggestion);
    total_warnings_found++;
}

void report_unused_function_warning(int line, int col, const char *func_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Function '%s' declared but never called", func_name);
    snprintf(suggestion, sizeof(suggestion), "Remove unused function or add function calls");

    report_warning(WARN_UNUSED_FUNCTION, line, col, message, suggestion);
    total_warnings_found++;
}

void report_implicit_conversion_warning(int line, int col, DataType from, DataType to)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Implicit conversion from %s to %s",
             type_to_string(from), type_to_string(to));
    snprintf(suggestion, sizeof(suggestion), "Use explicit cast for clarity: (%s)value",
             type_to_string(to));

    report_warning(WARN_IMPLICIT_CONVERSION, line, col, message, suggestion);
    total_warnings_found++;
}

void report_unreachable_code_warning(int line, int col, const char *context)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Unreachable code detected: %s", context);
    snprintf(suggestion, sizeof(suggestion), "Remove unreachable statements");

    report_warning(WARN_UNREACHABLE_CODE, line, col, message, suggestion);
    total_warnings_found++;
}

void report_uninitialized_variable_warning(int line, int col, const char *var_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Variable '%s' used before initialization", var_name);
    snprintf(suggestion, sizeof(suggestion), "Initialize variable before use");

    report_warning(WARN_UNINITIALIZED_VAR, line, col, message, suggestion);
    total_warnings_found++;
}

void report_shadowed_variable_warning(int line, int col, const char *var_name, const char *outer_scope)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Variable '%s' shadows variable in outer scope '%s'",
             var_name, outer_scope);
    snprintf(suggestion, sizeof(suggestion), "Rename variable to avoid shadowing");

    report_warning(WARN_SHADOWED_VARIABLE, line, col, message, suggestion);
    total_warnings_found++;
}

void report_missing_return_warning(int line, int col, const char *func_name)
{
    char message[256];
    char suggestion[256];

    snprintf(message, sizeof(message), "Non-void function '%s' may not return a value", func_name);
    snprintf(suggestion, sizeof(suggestion), "Add return statement or change function to void");

    report_warning(WARN_MISSING_RETURN, line, col, message, suggestion);
    total_warnings_found++;
}

void report_performance_warning(int line, int col, const char *issue, const char *suggestion)
{
    char message[256];

    snprintf(message, sizeof(message), "Performance issue: %s", issue);

    report_warning(WARN_PERFORMANCE_ISSUE, line, col, message, suggestion);
    total_warnings_found++;
}

void report_style_warning(int line, int col, const char *issue, const char *suggestion)
{
    char message[256];

    snprintf(message, sizeof(message), "Style issue: %s", issue);

    report_warning(WARN_STYLE_VIOLATION, line, col, message, suggestion);
    total_warnings_found++;
}