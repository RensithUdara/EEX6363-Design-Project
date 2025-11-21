%{
#include <stdio.h>
#include <stdlib.h>

// External reference to Flex
int yylex();
void yyerror(const char *s);
extern void print_symbol_table();
extern int line, column;
extern FILE *yyin;  // For file input
%}

/* Token Declarations */
%token IF ELSE WHILE THEN READ WRITE RETURN
%token INTEGER_KW FLOAT_KW VOID
%token EQ ASSIGN EQUALS LE GE LT GT NE PLUS MINUS MULT DIV
%token AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET SEMI COMMA DOT SCOPE COLON
%token CLASS FUNC IMPLEMENT ISA PRIVATE PUBLIC LOCAL ATTRIBUTE
%token ID INT FLOAT STRING ERROR

%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MULT DIV
%right NOT
%right UMINUS /* For unary minus */

%start program

%%

program: top_level_list { printf("Derivation: program -> top_level_list\n"); }

top_level_list: top_level_list top_level_element { printf("Derivation: top_level_list -> top_level_list top_level_element\n"); }
 | top_level_element { printf("Derivation: top_level_list -> top_level_element\n"); }
 ;

top_level_element: class_decl { printf("Derivation: top_level_element -> class_decl\n"); }
 | impl_def { printf("Derivation: top_level_element -> impl_def\n"); }
 | method_decl { printf("Derivation: top_level_element -> method_decl\n"); }
 ;

class_decl: CLASS ID opt_isa LBRACE member_list RBRACE { printf("Derivation: class_decl -> CLASS ID opt_isa LBRACE member_list RBRACE\n"); }

opt_isa: ISA ID { printf("Derivation: opt_isa -> ISA ID\n"); }
 | /* empty */ { printf("Derivation: opt_isa -> e\n"); }
 ;

member_list: member member_list' { printf("Derivation: member_list -> member member_list'\n"); }
 ;

member_list': member member_list' { printf("Derivation: member_list' -> member member_list'\n"); }
 | /* empty */ { printf("Derivation: member_list' -> e\n"); }
 ;

member: visibility field_decl { printf("Derivation: member -> visibility field_decl\n"); }
 | visibility method_decl { printf("Derivation: member -> visibility method_decl\n"); }
 | field_decl { printf("Derivation: member -> field_decl\n"); }
 | method_decl { printf("Derivation: member -> method_decl\n"); }
 ;

visibility: PUBLIC { printf("Derivation: visibility -> PUBLIC\n"); }
 | PRIVATE { printf("Derivation: visibility -> PRIVATE\n"); }
 ;

field_decl: ATTRIBUTE type ID SEMI { printf("Derivation: field_decl -> ATTRIBUTE type ID SEMI\n"); }
 | ATTRIBUTE type ID LBRACKET INT RBRACKET SEMI { printf("Derivation: field_decl -> ATTRIBUTE type ID LBRACKET INT RBRACKET SEMI\n"); }
 | ATTRIBUTE type ID LBRACKET ID RBRACKET LBRACKET INT RBRACKET SEMI { printf("Derivation: field_decl -> ATTRIBUTE type ID LBRACKET ID RBRACKET LBRACKET INT RBRACKET SEMI\n"); }
 ;

method_decl: FUNC ID LPAREN opt_param_list RPAREN opt_colon_type LBRACE stmt_list RBRACE { printf("Derivation: method_decl -> FUNC ID LPAREN opt_param_list RPAREN opt_colon_type LBRACE stmt_list RBRACE\n"); }
 ;

opt_param_list: param_list { printf("Derivation: opt_param_list -> param_list\n"); }
 | /* empty */ { printf("Derivation: opt_param_list -> e\n"); }
 ;

opt_colon_type: COLON type { printf("Derivation: opt_colon_type -> COLON type\n"); }
 | /* empty */ { printf("Derivation: opt_colon_type -> e\n"); }
 ;

param_list: param param_list' { printf("Derivation: param_list -> param param_list'\n"); }
 ;

param_list': COMMA param param_list' { printf("Derivation: param_list' -> COMMA param param_list'\n"); }
 | /* empty */ { printf("Derivation: param_list' -> e\n"); }
 ;

param: type ID { printf("Derivation: param -> type ID\n"); }
 | type ID LBRACKET RBRACKET { printf("Derivation: param -> type ID LBRACKET RBRACKET\n"); }
 ;

stmt_list: stmt stmt_list' { printf("Derivation: stmt_list -> stmt stmt_list'\n"); }
 | /* empty */ { printf("Derivation: stmt_list -> e\n"); }
 ;

stmt_list': stmt stmt_list' { printf("Derivation: stmt_list' -> stmt stmt_list'\n"); }
 | /* empty */ { printf("Derivation: stmt_list' -> e\n"); }
 ;

stmt: expr_stmt { printf("Derivation: stmt -> expr_stmt\n"); }
 | if_stmt { printf("Derivation: stmt -> if_stmt\n"); }
 | while_stmt { printf("Derivation: stmt -> while_stmt\n"); }
 | io_stmt { printf("Derivation: stmt -> io_stmt\n"); }
 | decl_stmt { printf("Derivation: stmt -> decl_stmt\n"); }
 | block_stmt { printf("Derivation: stmt -> block_stmt\n"); }
 | return_stmt { printf("Derivation: stmt -> return_stmt\n"); }
 | assignment_stmt { printf("Derivation: stmt -> assignment_stmt\n"); }
 ;

assignment_stmt: ID DOT ID EQUALS expr SEMI { printf("Derivation: assignment_stmt -> ID DOT ID EQUALS expr SEMI\n"); }
 | ID LBRACKET expr RBRACKET EQUALS expr SEMI { printf("Derivation: assignment_stmt -> ID LBRACKET expr RBRACKET EQUALS expr SEMI\n"); }
 | ID LBRACKET expr RBRACKET LBRACKET expr RBRACKET EQUALS expr SEMI { printf("Derivation: assignment_stmt -> ID LBRACKET expr RBRACKET LBRACKET expr RBRACKET EQUALS expr SEMI\n"); }
 ;

return_stmt: RETURN expr SEMI { printf("Derivation: return_stmt -> RETURN expr SEMI\n"); }
 | RETURN SEMI { printf("Derivation: return_stmt -> RETURN SEMI\n"); }
 ;

block_stmt: LBRACE stmt_list RBRACE { printf("Derivation: block_stmt -> LBRACE stmt_list RBRACE\n"); }
 ;

decl_stmt: LOCAL type ID ASSIGN expr SEMI { printf("Derivation: decl_stmt -> LOCAL type ID ASSIGN expr SEMI\n"); }
 | LOCAL type ID SEMI { printf("Derivation: decl_stmt -> LOCAL type ID SEMI\n"); }
 | type ID ASSIGN expr SEMI { printf("Derivation: decl_stmt -> type ID ASSIGN expr SEMI\n"); }
 | type ID SEMI { printf("Derivation: decl_stmt -> type ID SEMI\n"); }
 ;

type: INTEGER_KW { printf("Derivation: type -> INTEGER_KW\n"); }
 | FLOAT_KW { printf("Derivation: type -> FLOAT_KW\n"); }
 | VOID { printf("Derivation: type -> VOID\n"); }
 | ID { printf("Derivation: type -> ID\n"); }
 ;

expr_stmt: ID ASSIGN expr SEMI { printf("Derivation: expr_stmt -> ID ASSIGN expr SEMI\n"); }
 | ID EQUALS expr SEMI { printf("Derivation: expr_stmt -> ID EQUALS expr SEMI\n"); }
 ;

if_stmt: IF LPAREN expr RPAREN THEN stmt ELSE stmt { printf("Derivation: if_stmt -> IF LPAREN expr RPAREN THEN stmt ELSE stmt\n"); }
 | IF LPAREN expr RPAREN THEN stmt { printf("Derivation: if_stmt -> IF LPAREN expr RPAREN THEN stmt\n"); }
 | IF LPAREN expr RPAREN stmt ELSE stmt { printf("Derivation: if_stmt -> IF LPAREN expr RPAREN stmt ELSE stmt\n"); }
 | IF LPAREN expr RPAREN stmt { printf("Derivation: if_stmt -> IF LPAREN expr RPAREN stmt\n"); }
 ;

while_stmt: WHILE LPAREN expr RPAREN stmt { printf("Derivation: while_stmt -> WHILE LPAREN expr RPAREN stmt\n"); }
 ;

io_stmt: READ LPAREN ID RPAREN SEMI { printf("Derivation: io_stmt -> READ LPAREN ID RPAREN SEMI\n"); }
 | READ LPAREN ID DOT ID RPAREN SEMI { printf("Derivation: io_stmt -> READ LPAREN ID DOT ID RPAREN SEMI\n"); }
 | READ LPAREN ID LBRACKET expr RBRACKET RPAREN SEMI { printf("Derivation: io_stmt -> READ LPAREN ID LBRACKET expr RBRACKET RPAREN SEMI\n"); }
 | WRITE LPAREN expr RPAREN SEMI { printf("Derivation: io_stmt -> WRITE LPAREN expr RPAREN SEMI\n"); }
 ;

expr: expr OR expr { printf("Derivation: expr -> expr OR expr\n"); }
 | expr AND expr { printf("Derivation: expr -> expr AND expr\n"); }
 | expr EQ expr { printf("Derivation: expr -> expr EQ expr\n"); }
 | expr NE expr { printf("Derivation: expr -> expr NE expr\n"); }
 | expr LT expr { printf("Derivation: expr -> expr LT expr\n"); }
 | expr GT expr { printf("Derivation: expr -> expr GT expr\n"); }
 | expr LE expr { printf("Derivation: expr -> expr LE expr\n"); }
 | expr GE expr { printf("Derivation: expr -> expr GE expr\n"); }
 | expr PLUS expr { printf("Derivation: expr -> expr PLUS expr\n"); }
 | expr MINUS expr { printf("Derivation: expr -> expr MINUS expr\n"); }
 | expr MULT expr { printf("Derivation: expr -> expr MULT expr\n"); }
 | expr DIV expr { printf("Derivation: expr -> expr DIV expr\n"); }
 | NOT expr { printf("Derivation: expr -> NOT expr\n"); }
 | MINUS expr %prec UMINUS { printf("Derivation: expr -> MINUS expr\n"); }
 | LPAREN expr RPAREN { printf("Derivation: expr -> LPAREN expr RPAREN\n"); }
 | ID { printf("Derivation: expr -> ID\n"); }
 | ID DOT ID { printf("Derivation: expr -> ID DOT ID\n"); }
 | ID LBRACKET expr RBRACKET { printf("Derivation: expr -> ID LBRACKET expr RBRACKET\n"); }
 | ID LBRACKET expr RBRACKET LBRACKET expr RBRACKET { printf("Derivation: expr -> ID LBRACKET expr RBRACKET LBRACKET expr RBRACKET\n"); }
 | ID LPAREN arg_list RPAREN { printf("Derivation: expr -> ID LPAREN arg_list RPAREN\n"); }
 | ID LPAREN RPAREN { printf("Derivation: expr -> ID LPAREN RPAREN\n"); }
 | ID DOT ID LPAREN arg_list RPAREN { printf("Derivation: expr -> ID DOT ID LPAREN arg_list RPAREN\n"); }
 | ID DOT ID LPAREN RPAREN { printf("Derivation: expr -> ID DOT ID LPAREN RPAREN\n"); }
 | ID SCOPE ID LPAREN arg_list RPAREN { printf("Derivation: expr -> ID SCOPE ID LPAREN arg_list RPAREN\n"); }
 | ID SCOPE ID LPAREN RPAREN { printf("Derivation: expr -> ID SCOPE ID LPAREN RPAREN\n"); }
 | INT { printf("Derivation: expr -> INT\n"); }
 | FLOAT { printf("Derivation: expr -> FLOAT\n"); }
 | STRING { printf("Derivation: expr -> STRING\n"); }
 ;

arg_list: expr arg_list' { printf("Derivation: arg_list -> expr arg_list'\n"); }
 ;

arg_list': COMMA expr arg_list' { printf("Derivation: arg_list' -> COMMA expr arg_list'\n"); }
 | /* empty */ { printf("Derivation: arg_list' -> e\n"); }
 ;

impl_def: IMPLEMENT ID LBRACE method_list RBRACE { printf("Derivation: impl_def -> IMPLEMENT ID LBRACE method_list RBRACE\n"); }
 ;

method_list: method_decl method_list { printf("Derivation: method_list -> method_decl method_list\n"); }
 | /* empty */ { printf("Derivation: method_list -> e\n"); }
 ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d, column %d: %s\n", line, column, s);
}

int main(int argc, char *argv[]) {
    FILE *input_file;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    
    // Set flex to read from file instead of stdin
    yyin = input_file;
    
    printf("Begin parsing file: %s\n", argv[1]);
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");
    } else {
        printf("Parsing failed.\n");
    }
    
    fclose(input_file);
    print_symbol_table();
    return 0;
}