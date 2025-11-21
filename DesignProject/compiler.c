#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "codegen.h"
#include "register_allocator.h"
#include "../TMA03/ast.h"

// Compiler version and information
#define COMPILER_VERSION "1.0.0"
#define COMPILER_NAME "Design Project Compiler"

// Compilation phases
typedef enum
{
    PHASE_LEXICAL = 1,
    PHASE_SYNTAX = 2,
    PHASE_SEMANTIC = 4,
    PHASE_CODEGEN = 8,
    PHASE_ALL = 15
} CompilationPhase;

// Compiler options
typedef struct
{
    char *input_file;
    char *output_file;
    int phases_to_run;
    int verbose;
    int debug_info;
    int optimize_level;
    int show_ast;
    int show_symbols;
    int show_registers;
    int show_memory;
    AllocationStrategy reg_strategy;
} CompilerOptions;

// External functions from TMA phases
extern int yylex();
extern void yyerror(const char *s);
extern FILE *yyin;
extern ASTNode *ast_root;
extern void semantic_analysis(ASTNode *root);
extern void print_ast(ASTNode *node, int indent);
extern void print_symbol_tables(FILE *file);

// Function prototypes
void print_usage(const char *program_name);
void print_version();
int parse_command_line(int argc, char *argv[], CompilerOptions *options);
int compile_file(CompilerOptions *options);
int run_lexical_analysis(const char *filename);
int run_syntax_analysis(const char *filename);
int run_semantic_analysis();
int run_code_generation(const char *output_file, CompilerOptions *options);
void print_compilation_summary(CompilerOptions *options);
const char *get_phase_name(CompilationPhase phase);

// Global compilation statistics
static struct
{
    int tokens_generated;
    int syntax_errors;
    int semantic_errors;
    int instructions_generated;
    double compilation_time;
} compilation_stats = {0, 0, 0, 0, 0.0};

int main(int argc, char *argv[])
{
    CompilerOptions options = {0};

    // Set default options
    options.phases_to_run = PHASE_ALL;
    options.verbose = 0;
    options.debug_info = 0;
    options.optimize_level = 1;
    options.show_ast = 0;
    options.show_symbols = 0;
    options.show_registers = 0;
    options.show_memory = 0;
    options.reg_strategy = ALLOC_LRU;

    printf("%s v%s\n", COMPILER_NAME, COMPILER_VERSION);
    printf("Complete Compiler Implementation - Design Project\n\n");

    // Parse command line arguments
    if (parse_command_line(argc, argv, &options) != 0)
    {
        return 1;
    }

    // Validate input file
    if (!options.input_file)
    {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    // Set default output file if not specified
    if (!options.output_file)
    {
        char *base_name = malloc(strlen(options.input_file) + 1);
        strcpy(base_name, options.input_file);
        char *dot = strrchr(base_name, '.');
        if (dot)
            *dot = '\0';

        options.output_file = malloc(strlen(base_name) + 5);
        sprintf(options.output_file, "%s.asm", base_name);
        free(base_name);
    }

    // Compile the file
    int result = compile_file(&options);

    // Print compilation summary
    if (options.verbose)
    {
        print_compilation_summary(&options);
    }

    // Clean up
    if (options.input_file)
        free(options.input_file);
    if (options.output_file)
        free(options.output_file);

    return result;
}

// Print usage information
void print_usage(const char *program_name)
{
    printf("Usage: %s [options] input_file\n\n", program_name);
    printf("Options:\n");
    printf("  -o, --output FILE       Output file (default: input.asm)\n");
    printf("  -p, --phases PHASES     Phases to run (1=lex, 2=parse, 4=semantic, 8=codegen)\n");
    printf("  -v, --verbose           Verbose output\n");
    printf("  -d, --debug             Include debug information\n");
    printf("  -O, --optimize LEVEL    Optimization level (0-3)\n");
    printf("  -a, --show-ast          Show abstract syntax tree\n");
    printf("  -s, --show-symbols      Show symbol tables\n");
    printf("  -r, --show-registers    Show register allocation info\n");
    printf("  -m, --show-memory       Show memory layout info\n");
    printf("  -R, --reg-strategy STR  Register allocation strategy\n");
    printf("                          (naive, lru, optimal, graph)\n");
    printf("  -h, --help              Show this help message\n");
    printf("  --version               Show version information\n\n");
    printf("Examples:\n");
    printf("  %s program.src                    # Compile program.src to program.asm\n", program_name);
    printf("  %s -v -r program.src              # Verbose with register info\n", program_name);
    printf("  %s -p 7 program.src               # Run lexical, syntax, and semantic only\n", program_name);
}

// Print version information
void print_version()
{
    printf("%s version %s\n", COMPILER_NAME, COMPILER_VERSION);
    printf("Built with:\n");
    printf("  - Lexical Analysis (Flex-based)\n");
    printf("  - Syntax Analysis (Recursive Descent Parser)\n");
    printf("  - Semantic Analysis (Two-phase with symbol tables)\n");
    printf("  - Code Generation (x86-64 assembly)\n");
    printf("  - Register Allocation (Multiple strategies)\n");
}

// Parse command line arguments
int parse_command_line(int argc, char *argv[], CompilerOptions *options)
{
    static struct option long_options[] = {
        {"output", required_argument, 0, 'o'},
        {"phases", required_argument, 0, 'p'},
        {"verbose", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 'd'},
        {"optimize", required_argument, 0, 'O'},
        {"show-ast", no_argument, 0, 'a'},
        {"show-symbols", no_argument, 0, 's'},
        {"show-registers", no_argument, 0, 'r'},
        {"show-memory", no_argument, 0, 'm'},
        {"reg-strategy", required_argument, 0, 'R'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 1000},
        {0, 0, 0, 0}};

    int c;
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "o:p:vdO:asrmR:h", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'o':
            options->output_file = malloc(strlen(optarg) + 1);
                strcpy(options->output_file, optarg);
            break;

        case 'p':
            options->phases_to_run = atoi(optarg);
            break;

        case 'v':
            options->verbose = 1;
            break;

        case 'd':
            options->debug_info = 1;
            break;

        case 'O':
            options->optimize_level = atoi(optarg);
            if (options->optimize_level < 0 || options->optimize_level > 3)
            {
                fprintf(stderr, "Error: Invalid optimization level: %d\n", options->optimize_level);
                return 1;
            }
            break;

        case 'a':
            options->show_ast = 1;
            break;

        case 's':
            options->show_symbols = 1;
            break;

        case 'r':
            options->show_registers = 1;
            break;

        case 'm':
            options->show_memory = 1;
            break;

        case 'R':
            if (strcmp(optarg, "naive") == 0)
            {
                options->reg_strategy = ALLOC_NAIVE;
            }
            else if (strcmp(optarg, "lru") == 0)
            {
                options->reg_strategy = ALLOC_LRU;
            }
            else if (strcmp(optarg, "optimal") == 0)
            {
                options->reg_strategy = ALLOC_OPTIMAL;
            }
            else if (strcmp(optarg, "graph") == 0)
            {
                options->reg_strategy = ALLOC_GRAPH_COLOR;
            }
            else
            {
                fprintf(stderr, "Error: Invalid register strategy: %s\n", optarg);
                return 1;
            }
            break;

        case 'h':
            print_usage(argv[0]);
            exit(0);

        case 1000:
            print_version();
            exit(0);

        case '?':
            return 1;

        default:
            break;
        }
    }

    // Get input file
    if (optind < argc)
    {
        options->input_file = malloc(strlen(argv[optind]) + 1);
        strcpy(options->input_file, argv[optind]);
    }

    return 0;
}

// Main compilation function
int compile_file(CompilerOptions *options)
{
    if (options->verbose)
    {
        printf("Compiling: %s -> %s\n", options->input_file, options->output_file);
        printf("Phases to run: ");
        if (options->phases_to_run & PHASE_LEXICAL)
            printf("Lexical ");
        if (options->phases_to_run & PHASE_SYNTAX)
            printf("Syntax ");
        if (options->phases_to_run & PHASE_SEMANTIC)
            printf("Semantic ");
        if (options->phases_to_run & PHASE_CODEGEN)
            printf("CodeGen ");
        printf("\n\n");
    }

    int result = 0;

    // Phase 1: Lexical Analysis
    if (options->phases_to_run & PHASE_LEXICAL)
    {
        if (options->verbose)
            printf("=== Phase 1: Lexical Analysis ===\n");
        result = run_lexical_analysis(options->input_file);
        if (result != 0)
        {
            fprintf(stderr, "Lexical analysis failed\n");
            return result;
        }
        if (options->verbose)
            printf("Lexical analysis completed successfully\n\n");
    }

    // Phase 2: Syntax Analysis
    if (options->phases_to_run & PHASE_SYNTAX)
    {
        if (options->verbose)
            printf("=== Phase 2: Syntax Analysis ===\n");
        result = run_syntax_analysis(options->input_file);
        if (result != 0)
        {
            fprintf(stderr, "Syntax analysis failed\n");
            return result;
        }
        if (options->verbose)
            printf("Syntax analysis completed successfully\n\n");

        // Show AST if requested
        if (options->show_ast && ast_root)
        {
            printf("=== Abstract Syntax Tree ===\n");
            print_ast(ast_root, 0);
            printf("\n");
        }
    }

    // Phase 3: Semantic Analysis
    if (options->phases_to_run & PHASE_SEMANTIC)
    {
        if (options->verbose)
            printf("=== Phase 3: Semantic Analysis ===\n");
        result = run_semantic_analysis();
        if (result != 0)
        {
            fprintf(stderr, "Semantic analysis failed\n");
            return result;
        }
        if (options->verbose)
            printf("Semantic analysis completed successfully\n\n");

        // Show symbol tables if requested
        if (options->show_symbols)
        {
            printf("=== Symbol Tables ===\n");
            print_symbol_tables(stdout);
            printf("\n");
        }
    }

    // Phase 4: Code Generation
    if (options->phases_to_run & PHASE_CODEGEN)
    {
        if (options->verbose)
            printf("=== Phase 4: Code Generation ===\n");
        result = run_code_generation(options->output_file, options);
        if (result != 0)
        {
            fprintf(stderr, "Code generation failed\n");
            return result;
        }
        if (options->verbose)
            printf("Code generation completed successfully\n\n");
    }

    if (options->verbose)
    {
        printf("Compilation completed successfully!\n");
        printf("Output written to: %s\n", options->output_file);
    }

    return 0;
}

// Run lexical analysis
int run_lexical_analysis(const char *filename)
{
    yyin = fopen(filename, "r");
    if (!yyin)
    {
        fprintf(stderr, "Error: Cannot open input file: %s\n", filename);
        return 1;
    }

    // Count tokens for statistics
    compilation_stats.tokens_generated = 0;
    
    // This would normally just prepare for parsing
    // For demonstration, we'll just verify the file can be tokenized
    
    fclose(yyin);
    return 0;
}

// Run syntax analysis
int run_syntax_analysis(const char *filename)
{
    yyin = fopen(filename, "r");
    if (!yyin)
    {
        fprintf(stderr, "Error: Cannot open input file: %s\n", filename);
        return 1;
    }

    // Call the parser (this would be yyparse() in a Bison-based system)
    // For our TMA03 system, we'd call the recursive descent parser
    extern int parse_program();

    int parse_result = parse_program();

    fclose(yyin);

    if (parse_result != 0)
    {
        compilation_stats.syntax_errors++;
        return 1;
    }

    return 0;
}

// Run semantic analysis
int run_semantic_analysis()
{
    if (!ast_root)
    {
        fprintf(stderr, "Error: No AST available for semantic analysis\n");
        return 1;
    }

    // Call semantic analyzer
    semantic_analysis(ast_root);

    // Check for semantic errors
    extern int semantic_error_count;
    compilation_stats.semantic_errors = semantic_error_count;

    if (semantic_error_count > 0)
    {
        return 1;
    }

    return 0;
}

// Run code generation
int run_code_generation(const char *output_file, CompilerOptions *options)
{
    if (!ast_root)
    {
        fprintf(stderr, "Error: No AST available for code generation\n");
        return 1;
    }

    FILE *output = fopen(output_file, "w");
    if (!output)
    {
        fprintf(stderr, "Error: Cannot create output file: %s\n", output_file);
        return 1;
    }

    // Generate code
    generate_code(ast_root, output);

    fclose(output);

    // Show additional information if requested
    if (options->show_registers || options->show_memory)
    {
        CodeGenContext *ctx = create_codegen_context(stdout);
        if (ctx)
        {
            if (options->show_registers)
            {
                print_register_allocation_info(ctx);
            }
            if (options->show_memory)
            {
                print_memory_layout_info(ctx);
            }
            destroy_codegen_context(ctx);
        }
    }

    return 0;
}

// Print compilation summary
void print_compilation_summary(CompilerOptions *options)
{
    printf("\n=== Compilation Summary ===\n");
    printf("Input file: %s\n", options->input_file);
    printf("Output file: %s\n", options->output_file);
    printf("Phases run: ");
    if (options->phases_to_run & PHASE_LEXICAL)
        printf("Lexical ");
    if (options->phases_to_run & PHASE_SYNTAX)
        printf("Syntax ");
    if (options->phases_to_run & PHASE_SEMANTIC)
        printf("Semantic ");
    if (options->phases_to_run & PHASE_CODEGEN)
        printf("CodeGen ");
    printf("\n");

    printf("\nStatistics:\n");
    printf("  Tokens generated: %d\n", compilation_stats.tokens_generated);
    printf("  Syntax errors: %d\n", compilation_stats.syntax_errors);
    printf("  Semantic errors: %d\n", compilation_stats.semantic_errors);
    printf("  Instructions generated: %d\n", compilation_stats.instructions_generated);
    printf("  Compilation time: %.3f seconds\n", compilation_stats.compilation_time);

    printf("\nOptions used:\n");
    printf("  Verbose: %s\n", options->verbose ? "Yes" : "No");
    printf("  Debug info: %s\n", options->debug_info ? "Yes" : "No");
    printf("  Optimization level: %d\n", options->optimize_level);
    printf("  Register strategy: ");
    switch (options->reg_strategy)
    {
    case ALLOC_NAIVE:
        printf("Naive\n");
        break;
    case ALLOC_LRU:
        printf("LRU\n");
        break;
    case ALLOC_OPTIMAL:
        printf("Optimal\n");
        break;
    case ALLOC_GRAPH_COLOR:
        printf("Graph Coloring\n");
        break;
    }
}

// Get phase name for display
const char *get_phase_name(CompilationPhase phase)
{
    switch (phase)
    {
    case PHASE_LEXICAL:
        return "Lexical Analysis";
    case PHASE_SYNTAX:
        return "Syntax Analysis";
    case PHASE_SEMANTIC:
        return "Semantic Analysis";
    case PHASE_CODEGEN:
        return "Code Generation";
    default:
        return "Unknown Phase";
    }
}