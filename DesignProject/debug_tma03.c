/*
 * DEBUG VERSION - Test TMA03 Integration
 */

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "error_handler.h"

// TMA03 external functions
extern int parse_program(ASTNode **result_ast);
extern void semantic_analysis(ASTNode *root);
extern ASTNode *ast_root;
extern int error_count;
extern int semantic_error_count;
extern FILE *yyin;

int main(int argc, char *argv[])
{
    printf("=== DEBUG TMA03 INTEGRATION TEST ===\n");

    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    printf("Opening file: %s\n", argv[1]);
    yyin = fopen(argv[1], "r");
    if (!yyin)
    {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    printf("Calling parse_program...\n");
    int result = parse_program(&ast_root);

    printf("Parse result: %d\n", result);
    printf("Error count: %d\n", error_count);
    printf("Semantic error count: %d\n", semantic_error_count);

    if (ast_root)
    {
        printf("AST created successfully\n");
    }
    else
    {
        printf("AST is NULL\n");
    }

    fclose(yyin);
    return 0;
}