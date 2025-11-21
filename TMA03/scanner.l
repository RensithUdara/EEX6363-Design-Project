%{
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ast.h"
#include "error_handler.h"

// Token definitions (must match parser_topdown.c)
#define IF 3
#define ELSE 4
#define WHILE 5
#define THEN 6
#define READ 7
#define WRITE 8
#define RETURN 9
#define INTEGER_KW 10
#define FLOAT_KW 11
#define VOID 12
#define SELF 13
#define CONSTRUCT 14
#define CONSTRUCTOR 15
#define ASSIGN 16
#define ARROW 17
#define EQ 18
#define NE 19
#define LE 20
#define GE 21
#define LT 22
#define GT 23
#define PLUS 24
#define MINUS 25
#define MULT 26
#define DIV 27
#define AND 28
#define OR 29
#define NOT 30
#define LPAREN 31
#define RPAREN 32
#define LBRACE 33
#define RBRACE 34
#define LBRACKET 35
#define RBRACKET 36
#define SEMI 37
#define COMMA 38
#define DOT 39
#define COLON 40
#define CLASS 41
#define FUNC 42
#define IMPLEMENT 43
#define ISA 44
#define PRIVATE 45
#define PUBLIC 46
#define LOCAL 47
#define ATTRIBUTE 48
#define ID 49
#define INTLIT 50
#define FLOATLIT 51
#define ERROR 52

// Declare fileno to avoid implicit declaration warning
int fileno(FILE *stream);

// Provide strdup if not available
#ifndef strdup
char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}
#endif

int line = 1, column = 1;

// Error handling globals
extern int error_count;
extern int warning_count;
extern int max_errors;

typedef struct {
    char* lexeme;
    char* token_type;
    int line_no;
    int column_no;
} Token;

Token symbol_table[1000];
int sym_index = 0;

void add_token(const char* lexeme, const char* type) {
    if (sym_index < 1000) {
        symbol_table[sym_index].lexeme = strdup(lexeme);
        symbol_table[sym_index].token_type = strdup(type);
        symbol_table[sym_index].line_no = line;
        symbol_table[sym_index].column_no = column;
        sym_index++;
    }
    column += yyleng;  // Update column after adding token
}

void print_token_table() {
    int i;
    char location_str[20];
    printf("\n=== Token Table ===\n");
    printf("+---------------+---------------+---------------+\n");
    printf("| %-13s | %-13s | %-13s |\n", "Lexeme", "Token Type", "Location");
    printf("+---------------+---------------+---------------+\n");
    for (i = 0; i < sym_index; i++) {
        sprintf(location_str, "(%d, %d)", symbol_table[i].line_no, symbol_table[i].column_no);
        printf("| %-13s | %-13s | %-13s |\n",
               symbol_table[i].lexeme,
               symbol_table[i].token_type,
               location_str);
    }
    printf("+---------------+---------------+---------------+\n");
    printf("Total tokens: %d\n", sym_index);
}

// Enhanced error handling for lexical errors
void handle_lexical_error(const char* msg) {
    if (error_count >= max_errors) {
        fprintf(stderr, "Too many errors (%d). Stopping compilation.\n", max_errors);
        exit(1);
    }
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, msg);
    
    // Report to global error manager if available
    report_lexical_error(LEX_INVALID_TOKEN, line, column, msg, yytext, "Check token format");
    
    error_count++;
}

void handle_invalid_number(const char* number_str) {
    char error_msg[200];
    snprintf(error_msg, sizeof(error_msg), 
             "Invalid octal number '%s' - leading zeros are not allowed in decimal numbers", 
             number_str);
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, error_msg);
    report_lexical_error(LEX_INVALID_NUMBER, line, column, error_msg, 
                        number_str, "Remove leading zeros or use '0' for zero value");
    error_count++;
}

void handle_invalid_float(const char* float_str) {
    char error_msg[200];
    snprintf(error_msg, sizeof(error_msg), 
             "Invalid float literal '%s' - contains multiple decimal points", 
             float_str);
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, error_msg);
    report_lexical_error(LEX_INVALID_FLOAT, line, column, error_msg, 
                        float_str, "Use correct float format with only one decimal point (e.g., '3.14')");
    error_count++;
}

void handle_invalid_identifier(const char* id_str) {
    char error_msg[200];
    snprintf(error_msg, sizeof(error_msg), 
             "Invalid identifier '%s' - contains illegal character '.' (dot)", 
             id_str);
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, error_msg);
    report_lexical_error(LEX_INVALID_IDENTIFIER, line, column, error_msg, 
                        id_str, "Remove dots from identifier name or use separate identifiers");
    error_count++;
}

void handle_double_semicolon(const char* id_str) {
    char error_msg[200];
    snprintf(error_msg, sizeof(error_msg), 
             "Unexpected double semicolon '%s'", 
             id_str);
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, error_msg);
    report_lexical_error(LEX_DOUBLE_SEMICOLON, line, column, error_msg, 
                        id_str, "Remove or replace ';;' with valid syntax");
    error_count++;
}

void handle_invalid_arrow(const char* arrow_str) {
    char error_msg[200];
    snprintf(error_msg, sizeof(error_msg), 
             "Invalid arrow operator '%s' - use '=>' for function return types", 
             arrow_str);
    
    fprintf(stderr, "Lexical Error at line %d, column %d: %s\n", line, column, error_msg);
    report_lexical_error(LEX_INVALID_TOKEN, line, column, error_msg, 
                        arrow_str, "Replace '<=>' with correct arrow syntax '=>'");
    error_count++;
}

%}

/* Flex options */
%option noinput
%option nounput

/* Regular expression definitions */
LETTER      [a-zA-Z]
DIGIT       [0-9]
NONZERO     [1-9]
ALPHANUM    {LETTER}|{DIGIT}|_
ID          {LETTER}{ALPHANUM}*
INTEGER     {NONZERO}{DIGIT}*|0
FRACTION    \.{DIGIT}*{NONZERO}|\.0
FLOAT       {INTEGER}{FRACTION}([eE][+-]?{INTEGER})?
WHITESPACE  [ \t\r]+
NEWLINE     \n

/* Invalid patterns for better error messages */
INVALID_NUM     0{DIGIT}+
INVALID_FLOAT   {INTEGER}\.{DIGIT}*\.{DIGIT}*|{INTEGER}\.{DIGIT}*0{DIGIT}+
INVALID_ID      {DIGIT}+{LETTER}{ALPHANUM}*
UNTERMINATED_STR \"[^"]*$
DOUBLE_SEMI ;;
INVALID_ARROW <=>

/* Start conditions for comments */
%x BLOCK_COMMENT

%%

"//".*                            { /* Inline comment - skip */ column += yyleng; }

"/*"                              { BEGIN(BLOCK_COMMENT); column += 2; }
<BLOCK_COMMENT>"*/"               { BEGIN(INITIAL); column += 2; }
<BLOCK_COMMENT>\n                 { line++; column = 1; }
<BLOCK_COMMENT><<EOF>>            { 
    handle_lexical_error("Unterminated block comment starting at this location"); 
    return 0; 
}
<BLOCK_COMMENT>.                  { column++; }

{WHITESPACE}                      { column += yyleng; }
{NEWLINE}                         { line++; column = 1; }

"attribute"                       { add_token(yytext, "ATTRIBUTE"); return ATTRIBUTE; }
"class"                           { add_token(yytext, "CLASS"); return CLASS; }
"construct"                       { add_token(yytext, "CONSTRUCT"); return CONSTRUCT; }
"constructor"                     { add_token(yytext, "CONSTRUCTOR"); return CONSTRUCTOR; }
"else"                            { add_token(yytext, "ELSE"); return ELSE; }
"float"                           { add_token(yytext, "FLOAT_KW"); return FLOAT_KW; }
"func"                            { add_token(yytext, "FUNC"); return FUNC; }
"if"                              { add_token(yytext, "IF"); return IF; }
"implement"                       { add_token(yytext, "IMPLEMENT"); return IMPLEMENT; }
"integer"                         { add_token(yytext, "INTEGER_KW"); return INTEGER_KW; }
"isa"                             { add_token(yytext, "ISA"); return ISA; }
"local"                           { add_token(yytext, "LOCAL"); return LOCAL; }
"private"                         { add_token(yytext, "PRIVATE"); return PRIVATE; }
"public"                          { add_token(yytext, "PUBLIC"); return PUBLIC; }
"read"                            { add_token(yytext, "READ"); return READ; }
"return"                          { add_token(yytext, "RETURN"); return RETURN; }
"self"                            { add_token(yytext, "SELF"); return SELF; }
"then"                            { add_token(yytext, "THEN"); return THEN; }
"void"                            { add_token(yytext, "VOID"); return VOID; }
"while"                           { add_token(yytext, "WHILE"); return WHILE; }
"write"                           { add_token(yytext, "WRITE"); return WRITE; }
"and"                             { add_token(yytext, "AND"); return AND; }
"or"                              { add_token(yytext, "OR"); return OR; }
"not"                             { add_token(yytext, "NOT"); return NOT; }

":="                              { add_token(yytext, "ASSIGN"); return ASSIGN; }
"=>"                              { add_token(yytext, "ARROW"); return ARROW; }
"=="                              { add_token(yytext, "EQ"); return EQ; }
"<>"                              { add_token(yytext, "NE"); return NE; }
"<="                              { add_token(yytext, "LE"); return LE; }
">="                              { add_token(yytext, "GE"); return GE; }

"<"                               { add_token(yytext, "LT"); return LT; }
">"                               { add_token(yytext, "GT"); return GT; }
"+"                               { add_token(yytext, "PLUS"); return PLUS; }
"-"                               { add_token(yytext, "MINUS"); return MINUS; }
"*"                               { add_token(yytext, "MULT"); return MULT; }
"/"                               { add_token(yytext, "DIV"); return DIV; }
"("                               { add_token(yytext, "LPAREN"); return LPAREN; }
")"                               { add_token(yytext, "RPAREN"); return RPAREN; }
"{"                               { add_token(yytext, "LBRACE"); return LBRACE; }
"}"                               { add_token(yytext, "RBRACE"); return RBRACE; }
"["                               { add_token(yytext, "LBRACKET"); return LBRACKET; }
"]"                               { add_token(yytext, "RBRACKET"); return RBRACKET; }
";"                               { add_token(yytext, "SEMI"); return SEMI; }
","                               { add_token(yytext, "COMMA"); return COMMA; }
"."                               { add_token(yytext, "DOT"); return DOT; }
":"                               { add_token(yytext, "COLON"); return COLON; }

{FLOAT}                           { add_token(yytext, "FLOATLIT"); return FLOATLIT; }
{INTEGER}                         { add_token(yytext, "INTLIT"); return INTLIT; }

{INVALID_NUM}                     { 
    handle_invalid_number(yytext); 
    column += yyleng; 
    // Return INTLIT with corrected value to allow parsing to continue
    // Convert "010" to "10" - just return INTLIT token
    return INTLIT; 
}

{INVALID_FLOAT}                   { 
    handle_invalid_float(yytext); 
    column += yyleng; 
    // Return FLOATLIT with best-effort parsing to allow parsing to continue
    return FLOATLIT; 
}

{ID}                              { add_token(yytext, "ID"); return ID; }

{INVALID_ID}                      { 
    handle_invalid_identifier(yytext);
    column += yyleng;
    // Return ID token to allow parsing to continue
    return ID;
}

{DOUBLE_SEMI}                      { 
    handle_double_semicolon(yytext);
    column += yyleng; 
    return ERROR; 
}

{UNTERMINATED_STR}                {
    handle_lexical_error("Unterminated string literal");
    column += yyleng;
    return ERROR;
}

{INVALID_ARROW}                   {
    handle_invalid_arrow(yytext);
    column += yyleng;
    return ERROR;
}

.                                 { 
    char error_msg[100];
    if (yytext[0] >= 32 && yytext[0] <= 126) {
        snprintf(error_msg, sizeof(error_msg), "Unexpected character '%c'", yytext[0]);
    } else {
        snprintf(error_msg, sizeof(error_msg), "Unexpected character (ASCII %d)", yytext[0]);
    }
    handle_lexical_error(error_msg);
    column += yyleng; 
    return ERROR; 
}

%%

int yywrap() {
    return 1;
}