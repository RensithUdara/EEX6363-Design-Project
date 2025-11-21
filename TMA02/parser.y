%{
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ast.h"

// External reference to Flex
int yylex();
void yyerror(const char *s);
extern int line, column;
extern FILE *yyin;
extern char* yytext;

// Error handling globals
int error_count = 0;
int warning_count = 0;
int max_errors = 10;

// Derivation file for output
FILE *derivation_file = NULL;

// AST root for semantic analysis
ASTNode* ast_root = NULL;

// Current visibility for member declarations
Visibility current_visibility = VIS_PUBLIC;

// Function to write derivations
void write_derivation(const char *production) {
    if (derivation_file) {
        fprintf(derivation_file, "%s\n", production);
        fflush(derivation_file);
    }
}

// Error handling functions
void report_syntax_error(int line, int col, const char* expected, const char* found) {
    if (error_count >= max_errors) {
        fprintf(stderr, "Too many errors (%d). Stopping compilation.\n", max_errors);
        exit(1);
    }
    
    fprintf(stderr, "Syntax Error at line %d, column %d: ", line, col);
    if (expected && found) {
        fprintf(stderr, "Expected %s but found '%s'\n", expected, found);
    } else if (expected) {
        fprintf(stderr, "Expected %s\n", expected);
    } else {
        fprintf(stderr, "Unexpected token '%s'\n", found ? found : "end of input");
    }
    error_count++;
}

// Simple panic mode recovery using token numbers
void panic_mode_recovery(void) {
    extern int yychar;
    extern int yylex(void);
    
    // Skip tokens until we find a synchronization point
    while (yychar != 0) {  // 0 is EOF
        // Check for synchronization tokens using their token numbers
        if (yychar == 259 ||    // SEMI
            yychar == 266 ||    // RBRACE
            yychar == 265 ||    // LBRACE
            yychar == 275 ||    // CLASS
            yychar == 276 ||    // FUNC
            yychar == 277 ||    // IMPLEMENT
            yychar == 252) {    // CONSTRUCTOR
            return;  // Found sync point, resume parsing
        }
        yychar = yylex();  // Get next token
    }
}

%}

/* Token Declarations */
%token IF ELSE WHILE THEN READ WRITE RETURN
%token INTEGER_KW FLOAT_KW VOID SELF CONSTRUCT CONSTRUCTOR
%token ASSIGN ARROW EQ NE LE GE LT GT
%token PLUS MINUS MULT DIV
%token AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMI COMMA DOT COLON
%token CLASS FUNC IMPLEMENT ISA PRIVATE PUBLIC LOCAL ATTRIBUTE
%token ID INTLIT FLOATLIT
%token ERROR

/* Union declaration for semantic values */
%union {
    char* str_val;
    int int_val;
    float float_val;
    ASTNode* ast_node;
    DataType data_type;
    Visibility visibility_val;
}

/* Type declarations */
%type <ast_node> prog classOrImplOrFuncList classOrImplOrFunc
%type <ast_node> classDecl implDef funcDef funcHead funcBody
%type <ast_node> inheritanceOpt idList visibilityMemberList memberDecl
%type <ast_node> funcDecl attributeDecl localVarDecl varDecl
%type <ast_node> arraySizeList arraySize varDeclOrStmtList localVarDeclOrStmt
%type <ast_node> statement simpleStmt idStmt idStmtTail idArrayTail idMemberTail
%type <ast_node> selfStmt selfMemberTail ifStmt elseOpt whileStmt
%type <ast_node> statBlock statementList variable variableTail
%type <ast_node> expr relExpr relExprPrime arithExpr arithExprPrime
%type <ast_node> term termPrime factor factorTail factorMemberTail factorSelfTail
%type <ast_node> type returnType fParams fParamsTail aParams aParamsTail
%type <ast_node> relOp addOp multOp sign funcDefList

%type <str_val> ID
%type <int_val> INTLIT
%type <float_val> FLOATLIT
%type <visibility_val> visibility
%type <data_type> INTEGER_KW FLOAT_KW VOID

%start prog

%%

prog:
    classOrImplOrFuncList
    { 
        write_derivation("prog → classOrImplOrFuncList");
        $$ = create_ast_node(AST_PROGRAM, line, column);
        if ($1) add_child($$, $1);
        ast_root = $$;
    }
    ;

classOrImplOrFuncList:
    classOrImplOrFunc classOrImplOrFuncList
    { 
        write_derivation("classOrImplOrFuncList → classOrImplOrFunc classOrImplOrFuncList");
        $$ = $1;
        if ($1 && $2) add_sibling($1, $2);
    }
    | /* empty */
    { 
        write_derivation("classOrImplOrFuncList → ε");
        $$ = NULL;
    }
    ;

classOrImplOrFunc:
    classDecl
    { 
        write_derivation("classOrImplOrFunc → classDecl");
        $$ = $1;
    }
    | implDef
    { 
        write_derivation("classOrImplOrFunc → implDef");
        $$ = $1;
    }
    | funcDef
    { 
        write_derivation("classOrImplOrFunc → funcDef");
        $$ = $1;
    }
    ;

/* Class declaration */
classDecl:
    CLASS ID inheritanceOpt LBRACE visibilityMemberList RBRACE SEMI
    { 
        write_derivation("classDecl → class id inheritanceOpt { visibilityMemberList } ;");
        $$ = create_ast_node(AST_CLASS_DECL, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($3) add_child($$, $3);
        if ($5) add_child($$, $5);
    }
    ;

inheritanceOpt:
    ISA ID idList
    { 
        write_derivation("inheritanceOpt → isa id idList");
        $$ = create_ast_node(AST_INHERIT_LIST, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($3) add_child($$, $3);
    }
    | /* empty */
    { 
        write_derivation("inheritanceOpt → ε");
        $$ = NULL;
    }
    ;

idList:
    COMMA ID idList
    { 
        write_derivation("idList → , id idList");
        $$ = create_string_node(AST_ID, $2, line, column);
        if ($3) add_sibling($$, $3);
    }
    | /* empty */
    { 
        write_derivation("idList → ε");
        $$ = NULL;
    }
    ;

/* Implementation definition */
implDef:
    IMPLEMENT ID LBRACE funcDefList RBRACE
    { 
        write_derivation("implDef → implement id { funcDefList }");
        $$ = create_ast_node(AST_IMPL_DEF, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($4) add_child($$, $4);
    }
    ;

funcDefList:
    funcDef funcDefList
    { 
        write_derivation("funcDefList → funcDef funcDefList");
        $$ = $1;
        if ($1 && $2) add_sibling($1, $2);
    }
    | /* empty */
    { 
        write_derivation("funcDefList → ε");
        $$ = NULL;
    }
    ;

/* Function definition */
funcDef:
    funcHead funcBody
    { 
        write_derivation("funcDef → funcHead funcBody");
        $$ = create_ast_node(AST_FUNC_DEF, line, column);
        if ($1) add_child($$, $1);
        if ($2) add_child($$, $2);
    }
    ;

funcHead:
    FUNC ID LPAREN fParams RPAREN ARROW returnType
    { 
        write_derivation("funcHead → func id ( fParams ) => returnType");
        $$ = create_ast_node(AST_FUNC_HEAD, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($4) add_child($$, $4);
        if ($7) add_child($$, $7);
    }
    | CONSTRUCTOR LPAREN fParams RPAREN
    { 
        write_derivation("funcHead → constructor ( fParams )");
        $$ = create_ast_node(AST_FUNC_HEAD, line, column);
        add_child($$, create_string_node(AST_ID, "constructor", line, column));
        if ($3) add_child($$, $3);
        add_child($$, create_type_node(TYPE_VOID, line, column));
    }
    ;

funcBody:
    LBRACE varDeclOrStmtList RBRACE
    { 
        write_derivation("funcBody → { varDeclOrStmtList }");
        $$ = create_ast_node(AST_FUNC_BODY, line, column);
        if ($2) add_child($$, $2);
    }
    ;

/* Visibility and members */
visibility:
    PUBLIC
    { 
        write_derivation("visibility → public");
        $$ = VIS_PUBLIC;
        current_visibility = VIS_PUBLIC;
    }
    | PRIVATE
    { 
        write_derivation("visibility → private");
        $$ = VIS_PRIVATE;
        current_visibility = VIS_PRIVATE;
    }
    ;

visibilityMemberList:
    visibility memberDecl visibilityMemberList
    { 
        write_derivation("visibilityMemberList → visibility memberDecl visibilityMemberList");
        $$ = $2;
        if ($2) {
            // Set visibility for member
            ASTNode* vis_node = create_ast_node(AST_VISIBILITY, line, column);
            vis_node->data.visibility = $1;
            add_child($2, vis_node);
        }
        if ($2 && $3) add_sibling($2, $3);
    }
    | /* empty */
    { 
        write_derivation("visibilityMemberList → ε");
        $$ = NULL;
    }
    ;

memberDecl:
    funcDecl
    { 
        write_derivation("memberDecl → funcDecl");
        $$ = $1;
    }
    | attributeDecl
    { 
        write_derivation("memberDecl → attributeDecl");
        $$ = $1;
    }
    ;

funcDecl:
    funcHead SEMI
    { 
        write_derivation("funcDecl → funcHead ;");
        $$ = create_ast_node(AST_FUNC_DECL, line, column);
        if ($1) add_child($$, $1);
    }
    ;

attributeDecl:
    ATTRIBUTE varDecl
    { 
        write_derivation("attributeDecl → attribute varDecl");
        $$ = create_ast_node(AST_ATTR_DECL, line, column);
        if ($2) add_child($$, $2);
    }
    ;

/* Variable declarations */
localVarDecl:
    LOCAL varDecl
    { 
        write_derivation("localVarDecl → local varDecl");
        $$ = create_ast_node(AST_LOCAL_VAR_DECL, line, column);
        if ($2) add_child($$, $2);
    }
    ;

varDecl:
    ID COLON type arraySizeList SEMI
    { 
        write_derivation("varDecl → id : type arraySizeList ;");
        $$ = create_ast_node(AST_VAR_DECL, line, column);
        add_child($$, create_string_node(AST_ID, $1, line, column));
        if ($3) add_child($$, $3);
        if ($4) add_child($$, $4);
    }
    ;

arraySizeList:
    arraySize arraySizeList
    { 
        write_derivation("arraySizeList → arraySize arraySizeList");
        $$ = create_ast_node(AST_ARRAY_SIZE_LIST, line, column);
        if ($1) add_child($$, $1);
        if ($2) add_child($$, $2);
    }
    | /* empty */
    { 
        write_derivation("arraySizeList → ε");
        $$ = NULL;
    }
    ;

arraySize:
    LBRACKET INTLIT RBRACKET
    { 
        write_derivation("arraySize → [ intLit ]");
        $$ = create_ast_node(AST_ARRAY_SIZE, line, column);
        add_child($$, create_int_node($2, line, column));
    }
    | LBRACKET RBRACKET
    { 
        write_derivation("arraySize → [ ]");
        $$ = create_ast_node(AST_ARRAY_SIZE, line, column);
        add_child($$, create_int_node(-1, line, column)); // -1 indicates dynamic size
    }
    ;

/* Statements and declarations list */
varDeclOrStmtList:
    localVarDeclOrStmt varDeclOrStmtList
    { 
        write_derivation("varDeclOrStmtList → localVarDeclOrStmt varDeclOrStmtList");
        $$ = create_ast_node(AST_STMT_LIST, line, column);
        if ($1) add_child($$, $1);
        if ($2) add_child($$, $2);
    }
    | /* empty */
    { 
        write_derivation("varDeclOrStmtList → ε");
        $$ = NULL;
    }
    ;

localVarDeclOrStmt:
    localVarDecl
    { 
        write_derivation("localVarDeclOrStmt → localVarDecl");
        $$ = $1;
    }
    | statement
    { 
        write_derivation("localVarDeclOrStmt → statement");
        $$ = $1;
    }
    ;

/* Statements */
statement:
    simpleStmt SEMI
    { 
        write_derivation("statement → simpleStmt ;");
        $$ = $1;
    }
    | ifStmt SEMI
    { 
        write_derivation("statement → ifStmt ;");
        $$ = $1;
    }
    | whileStmt SEMI
    { 
        write_derivation("statement → whileStmt ;");
        $$ = $1;
    }
    ;

simpleStmt:
    idStmt
    { 
        write_derivation("simpleStmt → idStmt");
        $$ = $1;
    }
    | selfStmt
    { 
        write_derivation("simpleStmt → selfStmt");
        $$ = $1;
    }
    | READ LPAREN variable RPAREN
    { 
        write_derivation("simpleStmt → read ( variable )");
        $$ = create_ast_node(AST_READ_STMT, line, column);
        if ($3) add_child($$, $3);
    }
    | WRITE LPAREN expr RPAREN
    { 
        write_derivation("simpleStmt → write ( expr )");
        $$ = create_ast_node(AST_WRITE_STMT, line, column);
        if ($3) add_child($$, $3);
    }
    | RETURN LPAREN expr RPAREN
    { 
        write_derivation("simpleStmt → return ( expr )");
        $$ = create_ast_node(AST_RETURN_STMT, line, column);
        if ($3) add_child($$, $3);
    }
    ;

/* ID-based statements - disambiguated */
idStmt:
    ID idStmtTail
    { 
        write_derivation("idStmt → id idStmtTail");
        $$ = $2;
        if ($$) {
            ASTNode* id_node = create_string_node(AST_ID, $1, line, column);
            add_child($$, id_node);
        }
    }
    ;

idStmtTail:
    ASSIGN expr
    { 
        write_derivation("idStmtTail → := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET idArrayTail
    { 
        write_derivation("idStmtTail → [ expr ] idArrayTail");
        $$ = $4;
        if ($$) {
            ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
            if ($2) add_child(array_access, $2);
            add_child($$, array_access);
        }
    }
    | LPAREN aParams RPAREN
    { 
        write_derivation("idStmtTail → ( aParams )");
        $$ = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    | DOT ID idMemberTail
    { 
        write_derivation("idStmtTail → . id idMemberTail");
        $$ = $3;
        if ($$) {
            ASTNode* member_access = create_ast_node(AST_MEMBER_ACCESS, line, column);
            add_child(member_access, create_string_node(AST_ID, $2, line, column));
            add_child($$, member_access);
        }
    }
    ;

idArrayTail:
    ASSIGN expr
    { 
        write_derivation("idArrayTail → := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET ASSIGN expr
    { 
        write_derivation("idArrayTail → [ expr ] := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child(array_access, $2);
        add_child($$, array_access);
        if ($5) add_child($$, $5);
    }
    ;

idMemberTail:
    ASSIGN expr
    { 
        write_derivation("idMemberTail → := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET ASSIGN expr
    { 
        write_derivation("idMemberTail → [ expr ] := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child(array_access, $2);
        add_child($$, array_access);
        if ($5) add_child($$, $5);
    }
    | LPAREN aParams RPAREN
    { 
        write_derivation("idMemberTail → ( aParams )");
        $$ = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    ;

/* SELF-based statements */
selfStmt:
    SELF DOT ID selfMemberTail
    { 
        write_derivation("selfStmt → self . id selfMemberTail");
        $$ = $4;
        if ($$) {
            ASTNode* self_member = create_ast_node(AST_MEMBER_ACCESS, line, column);
            add_child(self_member, create_string_node(AST_ID, "self", line, column));
            add_child(self_member, create_string_node(AST_ID, $3, line, column));
            add_child($$, self_member);
        }
    }
    ;

selfMemberTail:
    ASSIGN expr
    { 
        write_derivation("selfMemberTail → := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET ASSIGN expr
    { 
        write_derivation("selfMemberTail → [ expr ] := expr");
        $$ = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child(array_access, $2);
        add_child($$, array_access);
        if ($5) add_child($$, $5);
    }
    | LPAREN aParams RPAREN
    { 
        write_derivation("selfMemberTail → ( aParams )");
        $$ = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ($2) add_child($$, $2);
    }
    ;

/* Control structures */
ifStmt:
    IF LPAREN expr RPAREN THEN statBlock elseOpt
    { 
        write_derivation("ifStmt → if ( expr ) then statBlock elseOpt");
        $$ = create_ast_node(AST_IF_STMT, line, column);
        if ($3) add_child($$, $3);  // condition
        if ($6) add_child($$, $6);  // then block
        if ($7) add_child($$, $7);  // else block
    }
    ;

elseOpt:
    ELSE statBlock
    { 
        write_derivation("elseOpt → else statBlock");
        $$ = $2;
    }
    | /* empty */
    { 
        write_derivation("elseOpt → ε");
        $$ = NULL;
    }
    ;

whileStmt:
    WHILE LPAREN expr RPAREN statBlock
    { 
        write_derivation("whileStmt → while ( expr ) statBlock");
        $$ = create_ast_node(AST_WHILE_STMT, line, column);
        if ($3) add_child($$, $3);  // condition
        if ($5) add_child($$, $5);  // body
    }
    ;

statBlock:
    LBRACE statementList RBRACE
    { 
        write_derivation("statBlock → { statementList }");
        $$ = create_ast_node(AST_STAT_BLOCK, line, column);
        if ($2) add_child($$, $2);
    }
    | statement
    { 
        write_derivation("statBlock → statement");
        $$ = $1;
    }
    | /* empty */
    { 
        write_derivation("statBlock → ε");
        $$ = NULL;
    }
    ;

statementList:
    statement statementList
    { 
        write_derivation("statementList → statement statementList");
        $$ = create_ast_node(AST_STMT_LIST, line, column);
        if ($1) add_child($$, $1);
        if ($2) add_child($$, $2);
    }
    | /* empty */
    { 
        write_derivation("statementList → ε");
        $$ = NULL;
    }
    ;

/* Variables */
variable:
    ID variableTail
    { 
        write_derivation("variable → id variableTail");
        if ($2) {
            $$ = $2;
            ASTNode* id_node = create_string_node(AST_ID, $1, line, column);
            add_child($$, id_node);
        } else {
            $$ = create_ast_node(AST_VARIABLE, line, column);
            add_child($$, create_string_node(AST_ID, $1, line, column));
        }
    }
    | SELF DOT ID variableTail
    { 
        write_derivation("variable → self . id variableTail");
        if ($4) {
            $$ = $4;
        } else {
            $$ = create_ast_node(AST_MEMBER_ACCESS, line, column);
        }
        ASTNode* self_node = create_string_node(AST_ID, "self", line, column);
        ASTNode* member_node = create_string_node(AST_ID, $3, line, column);
        add_child($$, self_node);
        add_child($$, member_node);
    }
    ;

variableTail:
    LBRACKET expr RBRACKET variableTail
    { 
        write_derivation("variableTail → [ expr ] variableTail");
        $$ = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child($$, $2);
        if ($4) add_child($$, $4);
    }
    | DOT ID variableTail
    { 
        write_derivation("variableTail → . id variableTail");
        $$ = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($3) add_child($$, $3);
    }
    | /* empty */
    { 
        write_derivation("variableTail → ε");
        $$ = NULL;
    }
    ;

/* Expressions - LL(1) with left recursion eliminated */
expr:
    relExpr
    { 
        write_derivation("expr → relExpr");
        $$ = $1;
    }
    ;

relExpr:
    arithExpr relExprPrime
    { 
        write_derivation("relExpr → arithExpr relExprPrime");
        if ($2) {
            $$ = $2;
            add_child($$, $1);
        } else {
            $$ = $1;
        }
    }
    ;

relExprPrime:
    relOp arithExpr
    { 
        write_derivation("relExprPrime → relOp arithExpr");
        $$ = $1;
        if ($2) add_child($$, $2);
    }
    | /* empty */
    { 
        write_derivation("relExprPrime → ε");
        $$ = NULL;
    }
    ;

relOp:
    EQ
    { 
        write_derivation("relOp → ==");
        $$ = create_binary_expr("==", NULL, NULL, line, column);
    }
    | NE
    { 
        write_derivation("relOp → <>");
        $$ = create_binary_expr("<>", NULL, NULL, line, column);
    }
    | LT
    { 
        write_derivation("relOp → <");
        $$ = create_binary_expr("<", NULL, NULL, line, column);
    }
    | GT
    { 
        write_derivation("relOp → >");
        $$ = create_binary_expr(">", NULL, NULL, line, column);
    }
    | LE
    { 
        write_derivation("relOp → <=");
        $$ = create_binary_expr("<=", NULL, NULL, line, column);
    }
    | GE
    { 
        write_derivation("relOp → >=");
        $$ = create_binary_expr(">=", NULL, NULL, line, column);
    }
    ;

arithExpr:
    term arithExprPrime
    { 
        write_derivation("arithExpr → term arithExprPrime");
        if ($2) {
            $$ = $2;
            add_child($$, $1);
        } else {
            $$ = $1;
        }
    }
    ;

arithExprPrime:
    addOp term arithExprPrime
    { 
        write_derivation("arithExprPrime → addOp term arithExprPrime");
        $$ = $1;
        if ($2) add_child($$, $2);
        if ($3) add_child($$, $3);
    }
    | /* empty */
    { 
        write_derivation("arithExprPrime → ε");
        $$ = NULL;
    }
    ;

addOp:
    PLUS
    { 
        write_derivation("addOp → +");
        $$ = create_binary_expr("+", NULL, NULL, line, column);
    }
    | MINUS
    { 
        write_derivation("addOp → -");
        $$ = create_binary_expr("-", NULL, NULL, line, column);
    }
    | OR
    { 
        write_derivation("addOp → or");
        $$ = create_binary_expr("or", NULL, NULL, line, column);
    }
    ;

term:
    factor termPrime
    { 
        write_derivation("term → factor termPrime");
        if ($2) {
            $$ = $2;
            add_child($$, $1);
        } else {
            $$ = $1;
        }
    }
    ;

termPrime:
    multOp factor termPrime
    { 
        write_derivation("termPrime → multOp factor termPrime");
        $$ = $1;
        if ($2) add_child($$, $2);
        if ($3) add_child($$, $3);
    }
    | /* empty */
    { 
        write_derivation("termPrime → ε");
        $$ = NULL;
    }
    ;

multOp:
    MULT
    { 
        write_derivation("multOp → *");
        $$ = create_binary_expr("*", NULL, NULL, line, column);
    }
    | DIV
    { 
        write_derivation("multOp → /");
        $$ = create_binary_expr("/", NULL, NULL, line, column);
    }
    | AND
    { 
        write_derivation("multOp → and");
        $$ = create_binary_expr("and", NULL, NULL, line, column);
    }
    ;

factor:
    INTLIT
    { 
        write_derivation("factor → intLit");
        $$ = create_int_node($1, line, column);
    }
    | FLOATLIT
    { 
        write_derivation("factor → floatLit");
        $$ = create_float_node($1, line, column);
    }
    | LPAREN expr RPAREN
    { 
        write_derivation("factor → ( expr )");
        $$ = $2;
    }
    | NOT factor
    { 
        write_derivation("factor → not factor");
        $$ = create_unary_expr("not", $2, line, column);
    }
    | sign factor
    { 
        write_derivation("factor → sign factor");
        $$ = $1;
        if ($2) add_child($$, $2);
    }
    | ID factorTail
    { 
        write_derivation("factor → id factorTail");
        ASTNode* id_node = create_string_node(AST_ID, $1, line, column);
        if ($2) {
            $$ = $2;
            add_child($$, id_node);
        } else {
            $$ = create_ast_node(AST_VARIABLE, line, column);
            add_child($$, id_node);
        }
    }
    | SELF factorSelfTail
    { 
        write_derivation("factor → self factorSelfTail");
        ASTNode* self_node = create_string_node(AST_ID, "self", line, column);
        $$ = $2;
        add_child($$, self_node);
    }
    ;

factorTail:
    LPAREN aParams RPAREN
    { 
        write_derivation("factorTail → ( aParams )");
        $$ = create_ast_node(AST_FUNC_CALL_EXPR, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET
    { 
        write_derivation("factorTail → [ expr ]");
        $$ = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child($$, $2);
    }
    | DOT ID factorMemberTail
    { 
        write_derivation("factorTail → . id factorMemberTail");
        $$ = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($3) add_child($$, $3);
    }
    | /* empty */
    { 
        write_derivation("factorTail → ε");
        $$ = NULL;
    }
    ;

factorMemberTail:
    LPAREN aParams RPAREN
    { 
        write_derivation("factorMemberTail → ( aParams )");
        $$ = create_ast_node(AST_FUNC_CALL_EXPR, line, column);
        if ($2) add_child($$, $2);
    }
    | LBRACKET expr RBRACKET
    { 
        write_derivation("factorMemberTail → [ expr ]");
        $$ = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ($2) add_child($$, $2);
    }
    | /* empty */
    { 
        write_derivation("factorMemberTail → ε");
        $$ = NULL;
    }
    ;

factorSelfTail:
    DOT ID factorMemberTail
    { 
        write_derivation("factorSelfTail → . id factorMemberTail");
        $$ = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($3) add_child($$, $3);
    }
    ;

sign:
    PLUS
    { 
        write_derivation("sign → +");
        $$ = create_unary_expr("+", NULL, line, column);
    }
    | MINUS
    { 
        write_derivation("sign → -");
        $$ = create_unary_expr("-", NULL, line, column);
    }
    ;

/* Types */
type:
    INTEGER_KW
    { 
        write_derivation("type → integer");
        $$ = create_type_node(TYPE_INTEGER, line, column);
    }
    | FLOAT_KW
    { 
        write_derivation("type → float");
        $$ = create_type_node(TYPE_FLOAT, line, column);
    }
    | ID
    { 
        write_derivation("type → id");
        $$ = create_ast_node(AST_TYPE, line, column);
        add_child($$, create_string_node(AST_ID, $1, line, column));
    }
    ;

returnType:
    type
    { 
        write_derivation("returnType → type");
        $$ = $1;
    }
    | VOID
    { 
        write_derivation("returnType → void");
        $$ = create_type_node(TYPE_VOID, line, column);
    }
    ;

/* Parameters */
fParams:
    ID COLON type arraySizeList fParamsTail
    { 
        write_derivation("fParams → id : type arraySizeList fParamsTail");
        $$ = create_ast_node(AST_PARAM_DECL, line, column);
        add_child($$, create_string_node(AST_ID, $1, line, column));
        if ($3) add_child($$, $3);
        if ($4) add_child($$, $4);
        if ($5) add_sibling($$, $5);
    }
    | /* empty */
    { 
        write_derivation("fParams → ε");
        $$ = NULL;
    }
    ;

fParamsTail:
    COMMA ID COLON type arraySizeList fParamsTail
    { 
        write_derivation("fParamsTail → , id : type arraySizeList fParamsTail");
        $$ = create_ast_node(AST_PARAM_DECL, line, column);
        add_child($$, create_string_node(AST_ID, $2, line, column));
        if ($4) add_child($$, $4);
        if ($5) add_child($$, $5);
        if ($6) add_sibling($$, $6);
    }
    | /* empty */
    { 
        write_derivation("fParamsTail → ε");
        $$ = NULL;
    }
    ;

aParams:
    expr aParamsTail
    { 
        write_derivation("aParams → expr aParamsTail");
        $$ = create_ast_node(AST_ARG_LIST, line, column);
        if ($1) add_child($$, $1);
        if ($2) add_sibling($$, $2);
    }
    | /* empty */
    { 
        write_derivation("aParams → ε");
        $$ = NULL;
    }
    ;

aParamsTail:
    COMMA expr aParamsTail
    { 
        write_derivation("aParamsTail → , expr aParamsTail");
        $$ = create_ast_node(AST_ARG_LIST, line, column);
        if ($2) add_child($$, $2);
        if ($3) add_sibling($$, $3);
    }
    | /* empty */
    { 
        write_derivation("aParamsTail → ε");
        $$ = NULL;
    }
    ;

%%

// Enhanced yyerror function with panic mode recovery
void yyerror(const char *s) {
    (void)s; // Mark unused parameter
    report_syntax_error(line, column, NULL, yytext);
    panic_mode_recovery();
}

// Enhanced main function with AST generation and semantic analysis
int main(int argc, char *argv[]) {
    FILE *input_file;
    char derivation_filename[256];
    char symbol_table_filename[256];
    char semantic_error_filename[256];
    
    // Initialize error counters
    error_count = 0;
    warning_count = 0;
    semantic_error_count = 0;
    semantic_warning_count = 0;
    
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> [derivation_file]\n", argv[0]);
        return 1;
    }
    
    input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    // Open output files
    if (argc == 3) {
        strcpy(derivation_filename, argv[2]);
    } else {
        sprintf(derivation_filename, "%s.derivation", argv[1]);
    }
    
    sprintf(symbol_table_filename, "%s.symboltable", argv[1]);
    sprintf(semantic_error_filename, "%s.semanticerrors", argv[1]);
    
    derivation_file = fopen(derivation_filename, "w");
    if (!derivation_file) {
        fprintf(stderr, "Warning: Cannot create derivation file '%s'\n", derivation_filename);
    } else {
        printf("Writing derivation to: %s\n", derivation_filename);
    }
    
    symbol_table_file = fopen(symbol_table_filename, "w");
    if (!symbol_table_file) {
        fprintf(stderr, "Warning: Cannot create symbol table file '%s'\n", symbol_table_filename);
    } else {
        printf("Writing symbol table to: %s\n", symbol_table_filename);
    }
    
    semantic_error_file = fopen(semantic_error_filename, "w");
    if (!semantic_error_file) {
        fprintf(stderr, "Warning: Cannot create semantic error file '%s'\n", semantic_error_filename);
    } else {
        printf("Writing semantic errors to: %s\n", semantic_error_filename);
    }
    
    // Set flex to read from file
    yyin = input_file;
    
    printf("\n=== Begin parsing file: %s ===\n", argv[1]);
    
    int result = yyparse();
    (void)result; // Mark unused variable
    
    // Print parsing summary
    printf("\n=== Parsing Summary ===\n");
    if (error_count == 0) {
        printf("✓ Parsing completed successfully with no syntax errors\n");
        if (derivation_file) {
            printf("✓ Derivation written to: %s\n", derivation_filename);
        }
        
        // Perform semantic analysis if parsing succeeded and AST was built
        if (ast_root) {
            printf("\n=== AST Structure ===\n");
            print_ast(ast_root, 0);
            
            printf("\n=== Semantic Analysis ===\n");
            semantic_analysis(ast_root);
            
            // Write symbol table
            if (symbol_table_file && global_scope) {
                print_symbol_table(global_scope, symbol_table_file, 0);
                printf("✓ Symbol table written to: %s\n", symbol_table_filename);
            }
            
            // Write semantic errors
            if (semantic_error_file) {
                print_semantic_errors(semantic_error_file);
                printf("✓ Semantic error report written to: %s\n", semantic_error_filename);
            }
            
            printf("\n=== Final Summary ===\n");
            if (semantic_error_count == 0) {
                printf("✓ Compilation completed successfully!\n");
                printf("  - No syntax errors\n");
                printf("  - No semantic errors\n");
                if (semantic_warning_count > 0) {
                    printf("  - %d semantic warning(s)\n", semantic_warning_count);
                }
            } else {
                printf("✗ Compilation failed due to semantic errors\n");
                printf("  - %d semantic error(s)\n", semantic_error_count);
                if (semantic_warning_count > 0) {
                    printf("  - %d semantic warning(s)\n", semantic_warning_count);
                }
            }
        } else {
            printf("✗ No AST generated - cannot perform semantic analysis\n");
        }
        
    } else {
        printf("✗ Parsing failed with %d syntax error(s)\n", error_count);
        if (warning_count > 0) {
            printf("⚠ %d warning(s) generated\n", warning_count);
        }
        
        // Provide helpful hints based on error patterns
        if (error_count >= max_errors) {
            printf("Note: Compilation stopped due to too many errors. Fix the first few and retry.\n");
        }
        printf("Tip: Check for missing semicolons, braces, or parentheses near reported locations.\n");
    }
    
    // Cleanup
    fclose(input_file);
    if (derivation_file) {
        fclose(derivation_file);
    }
    if (symbol_table_file) {
        fclose(symbol_table_file);
    }
    if (semantic_error_file) {
        fclose(semantic_error_file);
    }
    
    print_symbol_table(global_scope, stdout, 0);
    
    // Cleanup AST and semantic analyzer
    if (ast_root) {
        free_ast(ast_root);
    }
    cleanup_semantic_analyzer();
    
    return (error_count > 0 || semantic_error_count > 0) ? 1 : 0;
}