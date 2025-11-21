/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    ELSE = 259,                    /* ELSE  */
    WHILE = 260,                   /* WHILE  */
    THEN = 261,                    /* THEN  */
    READ = 262,                    /* READ  */
    WRITE = 263,                   /* WRITE  */
    RETURN = 264,                  /* RETURN  */
    INTEGER_KW = 265,              /* INTEGER_KW  */
    FLOAT_KW = 266,                /* FLOAT_KW  */
    VOID = 267,                    /* VOID  */
    SELF = 268,                    /* SELF  */
    CONSTRUCT = 269,               /* CONSTRUCT  */
    CONSTRUCTOR = 270,             /* CONSTRUCTOR  */
    ASSIGN = 271,                  /* ASSIGN  */
    ARROW = 272,                   /* ARROW  */
    EQ = 273,                      /* EQ  */
    NE = 274,                      /* NE  */
    LE = 275,                      /* LE  */
    GE = 276,                      /* GE  */
    LT = 277,                      /* LT  */
    GT = 278,                      /* GT  */
    PLUS = 279,                    /* PLUS  */
    MINUS = 280,                   /* MINUS  */
    MULT = 281,                    /* MULT  */
    DIV = 282,                     /* DIV  */
    AND = 283,                     /* AND  */
    OR = 284,                      /* OR  */
    NOT = 285,                     /* NOT  */
    LPAREN = 286,                  /* LPAREN  */
    RPAREN = 287,                  /* RPAREN  */
    LBRACE = 288,                  /* LBRACE  */
    RBRACE = 289,                  /* RBRACE  */
    LBRACKET = 290,                /* LBRACKET  */
    RBRACKET = 291,                /* RBRACKET  */
    SEMI = 292,                    /* SEMI  */
    COMMA = 293,                   /* COMMA  */
    DOT = 294,                     /* DOT  */
    COLON = 295,                   /* COLON  */
    CLASS = 296,                   /* CLASS  */
    FUNC = 297,                    /* FUNC  */
    IMPLEMENT = 298,               /* IMPLEMENT  */
    ISA = 299,                     /* ISA  */
    PRIVATE = 300,                 /* PRIVATE  */
    PUBLIC = 301,                  /* PUBLIC  */
    LOCAL = 302,                   /* LOCAL  */
    ATTRIBUTE = 303,               /* ATTRIBUTE  */
    ID = 304,                      /* ID  */
    INTLIT = 305,                  /* INTLIT  */
    FLOATLIT = 306,                /* FLOATLIT  */
    ERROR = 307                    /* ERROR  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 92 "parser.y"

    char* str_val;
    int int_val;
    float float_val;
    ASTNode* ast_node;
    DataType data_type;
    Visibility visibility_val;

#line 125 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
