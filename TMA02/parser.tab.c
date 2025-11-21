/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

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


#line 149 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IF = 3,                         /* IF  */
  YYSYMBOL_ELSE = 4,                       /* ELSE  */
  YYSYMBOL_WHILE = 5,                      /* WHILE  */
  YYSYMBOL_THEN = 6,                       /* THEN  */
  YYSYMBOL_READ = 7,                       /* READ  */
  YYSYMBOL_WRITE = 8,                      /* WRITE  */
  YYSYMBOL_RETURN = 9,                     /* RETURN  */
  YYSYMBOL_INTEGER_KW = 10,                /* INTEGER_KW  */
  YYSYMBOL_FLOAT_KW = 11,                  /* FLOAT_KW  */
  YYSYMBOL_VOID = 12,                      /* VOID  */
  YYSYMBOL_SELF = 13,                      /* SELF  */
  YYSYMBOL_CONSTRUCT = 14,                 /* CONSTRUCT  */
  YYSYMBOL_CONSTRUCTOR = 15,               /* CONSTRUCTOR  */
  YYSYMBOL_ASSIGN = 16,                    /* ASSIGN  */
  YYSYMBOL_ARROW = 17,                     /* ARROW  */
  YYSYMBOL_EQ = 18,                        /* EQ  */
  YYSYMBOL_NE = 19,                        /* NE  */
  YYSYMBOL_LE = 20,                        /* LE  */
  YYSYMBOL_GE = 21,                        /* GE  */
  YYSYMBOL_LT = 22,                        /* LT  */
  YYSYMBOL_GT = 23,                        /* GT  */
  YYSYMBOL_PLUS = 24,                      /* PLUS  */
  YYSYMBOL_MINUS = 25,                     /* MINUS  */
  YYSYMBOL_MULT = 26,                      /* MULT  */
  YYSYMBOL_DIV = 27,                       /* DIV  */
  YYSYMBOL_AND = 28,                       /* AND  */
  YYSYMBOL_OR = 29,                        /* OR  */
  YYSYMBOL_NOT = 30,                       /* NOT  */
  YYSYMBOL_LPAREN = 31,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 32,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 33,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 34,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 35,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 36,                  /* RBRACKET  */
  YYSYMBOL_SEMI = 37,                      /* SEMI  */
  YYSYMBOL_COMMA = 38,                     /* COMMA  */
  YYSYMBOL_DOT = 39,                       /* DOT  */
  YYSYMBOL_COLON = 40,                     /* COLON  */
  YYSYMBOL_CLASS = 41,                     /* CLASS  */
  YYSYMBOL_FUNC = 42,                      /* FUNC  */
  YYSYMBOL_IMPLEMENT = 43,                 /* IMPLEMENT  */
  YYSYMBOL_ISA = 44,                       /* ISA  */
  YYSYMBOL_PRIVATE = 45,                   /* PRIVATE  */
  YYSYMBOL_PUBLIC = 46,                    /* PUBLIC  */
  YYSYMBOL_LOCAL = 47,                     /* LOCAL  */
  YYSYMBOL_ATTRIBUTE = 48,                 /* ATTRIBUTE  */
  YYSYMBOL_ID = 49,                        /* ID  */
  YYSYMBOL_INTLIT = 50,                    /* INTLIT  */
  YYSYMBOL_FLOATLIT = 51,                  /* FLOATLIT  */
  YYSYMBOL_ERROR = 52,                     /* ERROR  */
  YYSYMBOL_YYACCEPT = 53,                  /* $accept  */
  YYSYMBOL_prog = 54,                      /* prog  */
  YYSYMBOL_classOrImplOrFuncList = 55,     /* classOrImplOrFuncList  */
  YYSYMBOL_classOrImplOrFunc = 56,         /* classOrImplOrFunc  */
  YYSYMBOL_classDecl = 57,                 /* classDecl  */
  YYSYMBOL_inheritanceOpt = 58,            /* inheritanceOpt  */
  YYSYMBOL_idList = 59,                    /* idList  */
  YYSYMBOL_implDef = 60,                   /* implDef  */
  YYSYMBOL_funcDefList = 61,               /* funcDefList  */
  YYSYMBOL_funcDef = 62,                   /* funcDef  */
  YYSYMBOL_funcHead = 63,                  /* funcHead  */
  YYSYMBOL_funcBody = 64,                  /* funcBody  */
  YYSYMBOL_visibility = 65,                /* visibility  */
  YYSYMBOL_visibilityMemberList = 66,      /* visibilityMemberList  */
  YYSYMBOL_memberDecl = 67,                /* memberDecl  */
  YYSYMBOL_funcDecl = 68,                  /* funcDecl  */
  YYSYMBOL_attributeDecl = 69,             /* attributeDecl  */
  YYSYMBOL_localVarDecl = 70,              /* localVarDecl  */
  YYSYMBOL_varDecl = 71,                   /* varDecl  */
  YYSYMBOL_arraySizeList = 72,             /* arraySizeList  */
  YYSYMBOL_arraySize = 73,                 /* arraySize  */
  YYSYMBOL_varDeclOrStmtList = 74,         /* varDeclOrStmtList  */
  YYSYMBOL_localVarDeclOrStmt = 75,        /* localVarDeclOrStmt  */
  YYSYMBOL_statement = 76,                 /* statement  */
  YYSYMBOL_simpleStmt = 77,                /* simpleStmt  */
  YYSYMBOL_idStmt = 78,                    /* idStmt  */
  YYSYMBOL_idStmtTail = 79,                /* idStmtTail  */
  YYSYMBOL_idArrayTail = 80,               /* idArrayTail  */
  YYSYMBOL_idMemberTail = 81,              /* idMemberTail  */
  YYSYMBOL_selfStmt = 82,                  /* selfStmt  */
  YYSYMBOL_selfMemberTail = 83,            /* selfMemberTail  */
  YYSYMBOL_ifStmt = 84,                    /* ifStmt  */
  YYSYMBOL_elseOpt = 85,                   /* elseOpt  */
  YYSYMBOL_whileStmt = 86,                 /* whileStmt  */
  YYSYMBOL_statBlock = 87,                 /* statBlock  */
  YYSYMBOL_statementList = 88,             /* statementList  */
  YYSYMBOL_variable = 89,                  /* variable  */
  YYSYMBOL_variableTail = 90,              /* variableTail  */
  YYSYMBOL_expr = 91,                      /* expr  */
  YYSYMBOL_relExpr = 92,                   /* relExpr  */
  YYSYMBOL_relExprPrime = 93,              /* relExprPrime  */
  YYSYMBOL_relOp = 94,                     /* relOp  */
  YYSYMBOL_arithExpr = 95,                 /* arithExpr  */
  YYSYMBOL_arithExprPrime = 96,            /* arithExprPrime  */
  YYSYMBOL_addOp = 97,                     /* addOp  */
  YYSYMBOL_term = 98,                      /* term  */
  YYSYMBOL_termPrime = 99,                 /* termPrime  */
  YYSYMBOL_multOp = 100,                   /* multOp  */
  YYSYMBOL_factor = 101,                   /* factor  */
  YYSYMBOL_factorTail = 102,               /* factorTail  */
  YYSYMBOL_factorMemberTail = 103,         /* factorMemberTail  */
  YYSYMBOL_factorSelfTail = 104,           /* factorSelfTail  */
  YYSYMBOL_sign = 105,                     /* sign  */
  YYSYMBOL_type = 106,                     /* type  */
  YYSYMBOL_returnType = 107,               /* returnType  */
  YYSYMBOL_fParams = 108,                  /* fParams  */
  YYSYMBOL_fParamsTail = 109,              /* fParamsTail  */
  YYSYMBOL_aParams = 110,                  /* aParams  */
  YYSYMBOL_aParamsTail = 111               /* aParamsTail  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  125
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  252

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   126,   126,   136,   143,   150,   155,   160,   169,   180,
     188,   195,   202,   210,   220,   227,   235,   245,   253,   264,
     274,   280,   289,   302,   309,   314,   322,   331,   341,   350,
     361,   369,   376,   382,   392,   400,   407,   412,   421,   426,
     431,   439,   444,   449,   455,   461,   471,   483,   489,   499,
     505,   518,   524,   536,   542,   551,   561,   575,   581,   590,
     600,   611,   617,   624,   634,   640,   646,   653,   661,   669,
     681,   697,   704,   712,   720,   728,   741,   748,   755,   760,
     765,   770,   775,   780,   788,   801,   809,   816,   821,   826,
     834,   847,   855,   862,   867,   872,   880,   885,   890,   895,
     900,   906,   918,   928,   934,   940,   948,   955,   961,   968,
     975,   985,   990,   999,  1004,  1009,  1018,  1023,  1032,  1042,
    1049,  1059,  1066,  1074,  1081,  1089
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IF", "ELSE", "WHILE",
  "THEN", "READ", "WRITE", "RETURN", "INTEGER_KW", "FLOAT_KW", "VOID",
  "SELF", "CONSTRUCT", "CONSTRUCTOR", "ASSIGN", "ARROW", "EQ", "NE", "LE",
  "GE", "LT", "GT", "PLUS", "MINUS", "MULT", "DIV", "AND", "OR", "NOT",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "SEMI",
  "COMMA", "DOT", "COLON", "CLASS", "FUNC", "IMPLEMENT", "ISA", "PRIVATE",
  "PUBLIC", "LOCAL", "ATTRIBUTE", "ID", "INTLIT", "FLOATLIT", "ERROR",
  "$accept", "prog", "classOrImplOrFuncList", "classOrImplOrFunc",
  "classDecl", "inheritanceOpt", "idList", "implDef", "funcDefList",
  "funcDef", "funcHead", "funcBody", "visibility", "visibilityMemberList",
  "memberDecl", "funcDecl", "attributeDecl", "localVarDecl", "varDecl",
  "arraySizeList", "arraySize", "varDeclOrStmtList", "localVarDeclOrStmt",
  "statement", "simpleStmt", "idStmt", "idStmtTail", "idArrayTail",
  "idMemberTail", "selfStmt", "selfMemberTail", "ifStmt", "elseOpt",
  "whileStmt", "statBlock", "statementList", "variable", "variableTail",
  "expr", "relExpr", "relExprPrime", "relOp", "arithExpr",
  "arithExprPrime", "addOp", "term", "termPrime", "multOp", "factor",
  "factorTail", "factorMemberTail", "factorSelfTail", "sign", "type",
  "returnType", "fParams", "fParamsTail", "aParams", "aParamsTail", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-179)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      47,   -19,   -32,   -24,     9,    61,  -179,    47,  -179,  -179,
    -179,    54,    31,    60,    62,    58,  -179,  -179,    19,  -179,
      56,    66,    72,    77,    31,     1,    74,    91,   103,   106,
     107,   100,    92,    81,  -179,   108,    19,  -179,   109,  -179,
    -179,   111,   112,    20,  -179,   102,    39,   113,   116,     1,
      52,    52,     8,    52,    52,    94,   104,  -179,    52,    52,
      52,   105,  -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,
    -179,   120,   114,  -179,  -179,  -179,    -1,   117,   130,  -179,
    -179,   118,  -179,  -179,    52,    52,    88,  -179,  -179,   124,
    -179,   110,    49,    98,    52,   126,   121,    25,   127,   132,
     133,    78,    20,  -179,   128,   135,   134,    83,   -21,   131,
     120,   102,    92,   125,    39,  -179,  -179,   136,    23,   119,
    -179,  -179,   139,    52,    52,   129,  -179,   168,  -179,  -179,
    -179,  -179,  -179,  -179,  -179,    52,  -179,  -179,  -179,  -179,
      52,  -179,  -179,  -179,  -179,    52,  -179,    37,   137,    52,
     138,  -179,  -179,  -179,  -179,    52,    52,    52,  -179,   120,
      52,  -179,  -179,     3,    52,    52,    52,  -179,  -179,   140,
     142,  -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,  -179,
    -179,    32,  -179,   143,   141,    32,    37,  -179,    49,    98,
      46,  -179,  -179,    25,   144,    25,  -179,   147,   145,   146,
     128,    52,    52,  -179,  -179,   150,   148,  -179,   149,    52,
      52,  -179,  -179,  -179,  -179,   181,  -179,  -179,    46,   154,
    -179,    25,  -179,  -179,   177,  -179,  -179,  -179,   158,  -179,
     179,    20,   164,   161,    37,  -179,  -179,  -179,  -179,    52,
     183,    52,   120,  -179,  -179,  -179,  -179,    52,  -179,   131,
    -179,  -179
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,     0,     0,     0,     0,     2,     4,     5,     6,
       7,     0,   119,    10,     0,     0,     1,     3,    35,    16,
       0,     0,     0,     0,   119,    15,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,    35,    37,     0,    41,
      42,     0,     0,     0,    18,    12,    23,     0,     0,    15,
       0,     0,     0,     0,     0,     0,     0,    28,     0,   123,
       0,     0,    46,    19,    34,    38,    39,    40,   113,   114,
     115,    31,     0,     9,    21,    20,     0,     0,     0,    13,
      14,     0,   111,   112,     0,     0,   106,    96,    97,     0,
      74,    77,    86,    92,     0,     0,     0,    73,     0,     0,
       0,     0,     0,    47,   125,     0,     0,     0,     0,   121,
      31,    12,     0,     0,    23,    24,    25,     0,     0,     0,
     102,    99,     0,   123,     0,     0,   101,     0,    78,    79,
      82,    83,    80,    81,    75,     0,    87,    88,    89,    84,
       0,    93,    94,    95,    90,     0,   100,    66,     0,     0,
       0,    69,    43,    44,    45,     0,   123,     0,    56,    31,
       0,   122,    49,     0,     0,   123,     0,    50,    33,     0,
       0,   118,    30,    11,    27,    26,    22,     8,   117,   116,
      17,   109,    98,     0,     0,   109,    66,    76,    86,    92,
      68,    65,    63,    73,     0,    73,    57,     0,     0,     0,
     125,     0,     0,    48,    53,     0,     0,    32,     0,   123,
       0,   110,   103,   104,   105,    62,    85,    91,    68,     0,
      70,    73,    72,    59,     0,    29,   124,    51,     0,    55,
       0,     0,     0,     0,    66,    60,    67,    64,    71,     0,
       0,     0,    31,   107,   108,    61,    58,     0,    54,   121,
      52,   120
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -179,  -179,   194,  -179,  -179,  -179,    93,  -179,   153,   -12,
     151,  -179,  -179,    89,  -179,  -179,  -179,  -179,    95,  -107,
    -179,   169,  -179,   -18,  -179,  -179,  -179,  -179,  -179,  -179,
    -179,  -179,  -179,  -179,  -178,   -10,  -179,  -114,   -49,  -179,
    -179,  -179,    71,    21,  -179,    70,    22,  -179,   -74,  -179,
      27,  -179,  -179,   -95,  -179,   189,   -35,  -117,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,     6,     7,     8,    23,    73,     9,    48,    10,
      11,    19,    76,    77,   114,   115,   116,    34,    57,   109,
     110,    35,    36,   191,    38,    39,    62,   203,   167,    40,
     158,    41,   235,    42,   192,   219,    98,   151,   104,    90,
     134,   135,    91,   139,   140,    92,   144,   145,    93,   126,
     211,   120,    94,    71,   180,    21,   171,   105,   161
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      37,    89,    95,   172,    99,   100,   183,   159,   215,   103,
     121,   106,    12,    49,     1,   168,     1,    13,    37,   201,
     146,    96,    26,   179,    27,    14,    28,    29,    30,   169,
      68,    69,    31,    68,    69,   178,   122,    49,   202,   197,
      26,     3,    27,     3,    28,    29,    30,   112,   205,    26,
      31,    27,   199,    28,    29,    30,   245,    97,    15,    31,
     149,    16,     1,   209,   150,    81,    32,   210,    33,    70,
     190,   189,    70,   136,   137,   184,    82,    83,   138,   220,
      20,   222,    84,    85,    74,    75,    33,    18,     2,     3,
       4,    25,   232,    24,   155,    33,    43,    58,    44,   164,
     194,    86,    87,    88,    22,    50,   196,   238,   198,   156,
      46,   200,    59,   157,   165,   204,    60,   206,   166,   123,
      61,    45,    51,   124,   141,   142,   143,   125,   128,   129,
     130,   131,   132,   133,    52,   249,   242,    53,    54,    55,
      72,    56,    63,   101,   102,    78,    65,   118,    66,    67,
      79,   117,   227,   228,   107,   108,   127,   119,   147,   152,
     148,   233,   175,   111,   153,   154,   160,   162,   181,   170,
     163,   182,   218,   177,   186,   212,   207,   213,   185,   223,
     221,   224,   229,   225,   230,   234,   193,   195,   237,   231,
     246,   208,   248,   239,   240,   241,   243,   244,   250,   247,
     218,    17,    80,   176,   173,    64,   187,   174,   236,   216,
     188,   217,   214,    47,   251,   226,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   113
};

static const yytype_int16 yycheck[] =
{
      18,    50,    51,   110,    53,    54,   123,   102,   186,    58,
      84,    60,    31,    25,    15,    36,    15,    49,    36,    16,
      94,    13,     3,   118,     5,    49,     7,     8,     9,    50,
      10,    11,    13,    10,    11,    12,    85,    49,    35,   156,
       3,    42,     5,    42,     7,     8,     9,    48,   165,     3,
      13,     5,   159,     7,     8,     9,   234,    49,    49,    13,
      35,     0,    15,    31,    39,    13,    47,    35,    49,    49,
      33,   145,    49,    24,    25,   124,    24,    25,    29,   193,
      49,   195,    30,    31,    45,    46,    49,    33,    41,    42,
      43,    33,   209,    31,    16,    49,    40,    16,    32,    16,
     149,    49,    50,    51,    44,    31,   155,   221,   157,    31,
      33,   160,    31,    35,    31,   164,    35,   166,    35,    31,
      39,    49,    31,    35,    26,    27,    28,    39,    18,    19,
      20,    21,    22,    23,    31,   242,   231,    31,    31,    39,
      38,    49,    34,    49,    40,    32,    37,    17,    37,    37,
      34,    34,   201,   202,    49,    35,    32,    39,    32,    32,
      39,   210,    37,    49,    32,    32,    38,    32,    49,    38,
      36,    32,   190,    37,     6,    32,    36,    36,    49,    32,
      36,    36,    32,    37,    36,     4,    49,    49,    34,    40,
     239,    49,   241,    16,    36,    16,    32,    36,   247,    16,
     218,     7,    49,   114,   111,    36,   135,   112,   218,   188,
     140,   189,   185,    24,   249,   200,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    15,    41,    42,    43,    54,    55,    56,    57,    60,
      62,    63,    31,    49,    49,    49,     0,    55,    33,    64,
      49,   108,    44,    58,    31,    33,     3,     5,     7,     8,
       9,    13,    47,    49,    70,    74,    75,    76,    77,    78,
      82,    84,    86,    40,    32,    49,    33,   108,    61,    62,
      31,    31,    31,    31,    31,    39,    49,    71,    16,    31,
      35,    39,    79,    34,    74,    37,    37,    37,    10,    11,
      49,   106,    38,    59,    45,    46,    65,    66,    32,    34,
      61,    13,    24,    25,    30,    31,    49,    50,    51,    91,
      92,    95,    98,   101,   105,    91,    13,    49,    89,    91,
      91,    49,    40,    91,    91,   110,    91,    49,    35,    72,
      73,    49,    48,    63,    67,    68,    69,    34,    17,    39,
     104,   101,    91,    31,    35,    39,   102,    32,    18,    19,
      20,    21,    22,    23,    93,    94,    24,    25,    29,    96,
      97,    26,    27,    28,    99,   100,   101,    32,    39,    35,
      39,    90,    32,    32,    32,    16,    31,    35,    83,   106,
      38,   111,    32,    36,    16,    31,    35,    81,    36,    50,
      38,   109,    72,    59,    71,    37,    66,    37,    12,   106,
     107,    49,    32,   110,    91,    49,     6,    95,    98,   101,
      33,    76,    87,    49,    91,    49,    91,   110,    91,    72,
      91,    16,    35,    80,    91,   110,    91,    36,    49,    31,
      35,   103,    32,    36,   103,    87,    96,    99,    76,    88,
      90,    36,    90,    32,    36,    37,   111,    91,    91,    32,
      36,    40,   110,    91,     4,    85,    88,    34,    90,    16,
      36,    16,   106,    32,    36,    87,    91,    16,    91,    72,
      91,   109
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    53,    54,    55,    55,    56,    56,    56,    57,    58,
      58,    59,    59,    60,    61,    61,    62,    63,    63,    64,
      65,    65,    66,    66,    67,    67,    68,    69,    70,    71,
      72,    72,    73,    73,    74,    74,    75,    75,    76,    76,
      76,    77,    77,    77,    77,    77,    78,    79,    79,    79,
      79,    80,    80,    81,    81,    81,    82,    83,    83,    83,
      84,    85,    85,    86,    87,    87,    87,    88,    88,    89,
      89,    90,    90,    90,    91,    92,    93,    93,    94,    94,
      94,    94,    94,    94,    95,    96,    96,    97,    97,    97,
      98,    99,    99,   100,   100,   100,   101,   101,   101,   101,
     101,   101,   101,   102,   102,   102,   102,   103,   103,   103,
     104,   105,   105,   106,   106,   106,   107,   107,   108,   108,
     109,   109,   110,   110,   111,   111
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     7,     3,
       0,     3,     0,     5,     2,     0,     2,     7,     4,     3,
       1,     1,     3,     0,     1,     1,     2,     2,     2,     5,
       2,     0,     3,     2,     2,     0,     1,     1,     2,     2,
       2,     1,     1,     4,     4,     4,     2,     2,     4,     3,
       3,     2,     5,     2,     5,     3,     4,     2,     5,     3,
       7,     2,     0,     5,     3,     1,     0,     2,     0,     2,
       4,     4,     3,     0,     1,     2,     2,     0,     1,     1,
       1,     1,     1,     1,     2,     3,     0,     1,     1,     1,
       2,     3,     0,     1,     1,     1,     1,     1,     3,     2,
       2,     2,     2,     3,     3,     3,     0,     3,     3,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     5,     0,
       6,     0,     2,     0,     3,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* prog: classOrImplOrFuncList  */
#line 127 "parser.y"
    { 
        write_derivation("prog → classOrImplOrFuncList");
        (yyval.ast_node) = create_ast_node(AST_PROGRAM, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
        ast_root = (yyval.ast_node);
    }
#line 1425 "parser.tab.c"
    break;

  case 3: /* classOrImplOrFuncList: classOrImplOrFunc classOrImplOrFuncList  */
#line 137 "parser.y"
    { 
        write_derivation("classOrImplOrFuncList → classOrImplOrFunc classOrImplOrFuncList");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
        if ((yyvsp[-1].ast_node) && (yyvsp[0].ast_node)) add_sibling((yyvsp[-1].ast_node), (yyvsp[0].ast_node));
    }
#line 1435 "parser.tab.c"
    break;

  case 4: /* classOrImplOrFuncList: %empty  */
#line 143 "parser.y"
    { 
        write_derivation("classOrImplOrFuncList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1444 "parser.tab.c"
    break;

  case 5: /* classOrImplOrFunc: classDecl  */
#line 151 "parser.y"
    { 
        write_derivation("classOrImplOrFunc → classDecl");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1453 "parser.tab.c"
    break;

  case 6: /* classOrImplOrFunc: implDef  */
#line 156 "parser.y"
    { 
        write_derivation("classOrImplOrFunc → implDef");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1462 "parser.tab.c"
    break;

  case 7: /* classOrImplOrFunc: funcDef  */
#line 161 "parser.y"
    { 
        write_derivation("classOrImplOrFunc → funcDef");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1471 "parser.tab.c"
    break;

  case 8: /* classDecl: CLASS ID inheritanceOpt LBRACE visibilityMemberList RBRACE SEMI  */
#line 170 "parser.y"
    { 
        write_derivation("classDecl → class id inheritanceOpt { visibilityMemberList } ;");
        (yyval.ast_node) = create_ast_node(AST_CLASS_DECL, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-5].str_val), line, column));
        if ((yyvsp[-4].ast_node)) add_child((yyval.ast_node), (yyvsp[-4].ast_node));
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));
    }
#line 1483 "parser.tab.c"
    break;

  case 9: /* inheritanceOpt: ISA ID idList  */
#line 181 "parser.y"
    { 
        write_derivation("inheritanceOpt → isa id idList");
        (yyval.ast_node) = create_ast_node(AST_INHERIT_LIST, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1494 "parser.tab.c"
    break;

  case 10: /* inheritanceOpt: %empty  */
#line 188 "parser.y"
    { 
        write_derivation("inheritanceOpt → ε");
        (yyval.ast_node) = NULL;
    }
#line 1503 "parser.tab.c"
    break;

  case 11: /* idList: COMMA ID idList  */
#line 196 "parser.y"
    { 
        write_derivation("idList → , id idList");
        (yyval.ast_node) = create_string_node(AST_ID, (yyvsp[-1].str_val), line, column);
        if ((yyvsp[0].ast_node)) add_sibling((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1513 "parser.tab.c"
    break;

  case 12: /* idList: %empty  */
#line 202 "parser.y"
    { 
        write_derivation("idList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1522 "parser.tab.c"
    break;

  case 13: /* implDef: IMPLEMENT ID LBRACE funcDefList RBRACE  */
#line 211 "parser.y"
    { 
        write_derivation("implDef → implement id { funcDefList }");
        (yyval.ast_node) = create_ast_node(AST_IMPL_DEF, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-3].str_val), line, column));
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1533 "parser.tab.c"
    break;

  case 14: /* funcDefList: funcDef funcDefList  */
#line 221 "parser.y"
    { 
        write_derivation("funcDefList → funcDef funcDefList");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
        if ((yyvsp[-1].ast_node) && (yyvsp[0].ast_node)) add_sibling((yyvsp[-1].ast_node), (yyvsp[0].ast_node));
    }
#line 1543 "parser.tab.c"
    break;

  case 15: /* funcDefList: %empty  */
#line 227 "parser.y"
    { 
        write_derivation("funcDefList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1552 "parser.tab.c"
    break;

  case 16: /* funcDef: funcHead funcBody  */
#line 236 "parser.y"
    { 
        write_derivation("funcDef → funcHead funcBody");
        (yyval.ast_node) = create_ast_node(AST_FUNC_DEF, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1563 "parser.tab.c"
    break;

  case 17: /* funcHead: FUNC ID LPAREN fParams RPAREN ARROW returnType  */
#line 246 "parser.y"
    { 
        write_derivation("funcHead → func id ( fParams ) => returnType");
        (yyval.ast_node) = create_ast_node(AST_FUNC_HEAD, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-5].str_val), line, column));
        if ((yyvsp[-3].ast_node)) add_child((yyval.ast_node), (yyvsp[-3].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1575 "parser.tab.c"
    break;

  case 18: /* funcHead: CONSTRUCTOR LPAREN fParams RPAREN  */
#line 254 "parser.y"
    { 
        write_derivation("funcHead → constructor ( fParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_HEAD, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, "constructor", line, column));
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        add_child((yyval.ast_node), create_type_node(TYPE_VOID, line, column));
    }
#line 1587 "parser.tab.c"
    break;

  case 19: /* funcBody: LBRACE varDeclOrStmtList RBRACE  */
#line 265 "parser.y"
    { 
        write_derivation("funcBody → { varDeclOrStmtList }");
        (yyval.ast_node) = create_ast_node(AST_FUNC_BODY, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1597 "parser.tab.c"
    break;

  case 20: /* visibility: PUBLIC  */
#line 275 "parser.y"
    { 
        write_derivation("visibility → public");
        (yyval.visibility_val) = VIS_PUBLIC;
        current_visibility = VIS_PUBLIC;
    }
#line 1607 "parser.tab.c"
    break;

  case 21: /* visibility: PRIVATE  */
#line 281 "parser.y"
    { 
        write_derivation("visibility → private");
        (yyval.visibility_val) = VIS_PRIVATE;
        current_visibility = VIS_PRIVATE;
    }
#line 1617 "parser.tab.c"
    break;

  case 22: /* visibilityMemberList: visibility memberDecl visibilityMemberList  */
#line 290 "parser.y"
    { 
        write_derivation("visibilityMemberList → visibility memberDecl visibilityMemberList");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
        if ((yyvsp[-1].ast_node)) {
            // Set visibility for member
            ASTNode* vis_node = create_ast_node(AST_VISIBILITY, line, column);
            vis_node->data.visibility = (yyvsp[-2].visibility_val);
            add_child((yyvsp[-1].ast_node), vis_node);
        }
        if ((yyvsp[-1].ast_node) && (yyvsp[0].ast_node)) add_sibling((yyvsp[-1].ast_node), (yyvsp[0].ast_node));
    }
#line 1633 "parser.tab.c"
    break;

  case 23: /* visibilityMemberList: %empty  */
#line 302 "parser.y"
    { 
        write_derivation("visibilityMemberList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1642 "parser.tab.c"
    break;

  case 24: /* memberDecl: funcDecl  */
#line 310 "parser.y"
    { 
        write_derivation("memberDecl → funcDecl");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1651 "parser.tab.c"
    break;

  case 25: /* memberDecl: attributeDecl  */
#line 315 "parser.y"
    { 
        write_derivation("memberDecl → attributeDecl");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1660 "parser.tab.c"
    break;

  case 26: /* funcDecl: funcHead SEMI  */
#line 323 "parser.y"
    { 
        write_derivation("funcDecl → funcHead ;");
        (yyval.ast_node) = create_ast_node(AST_FUNC_DECL, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1670 "parser.tab.c"
    break;

  case 27: /* attributeDecl: ATTRIBUTE varDecl  */
#line 332 "parser.y"
    { 
        write_derivation("attributeDecl → attribute varDecl");
        (yyval.ast_node) = create_ast_node(AST_ATTR_DECL, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1680 "parser.tab.c"
    break;

  case 28: /* localVarDecl: LOCAL varDecl  */
#line 342 "parser.y"
    { 
        write_derivation("localVarDecl → local varDecl");
        (yyval.ast_node) = create_ast_node(AST_LOCAL_VAR_DECL, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1690 "parser.tab.c"
    break;

  case 29: /* varDecl: ID COLON type arraySizeList SEMI  */
#line 351 "parser.y"
    { 
        write_derivation("varDecl → id : type arraySizeList ;");
        (yyval.ast_node) = create_ast_node(AST_VAR_DECL, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-4].str_val), line, column));
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1702 "parser.tab.c"
    break;

  case 30: /* arraySizeList: arraySize arraySizeList  */
#line 362 "parser.y"
    { 
        write_derivation("arraySizeList → arraySize arraySizeList");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_SIZE_LIST, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1713 "parser.tab.c"
    break;

  case 31: /* arraySizeList: %empty  */
#line 369 "parser.y"
    { 
        write_derivation("arraySizeList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1722 "parser.tab.c"
    break;

  case 32: /* arraySize: LBRACKET INTLIT RBRACKET  */
#line 377 "parser.y"
    { 
        write_derivation("arraySize → [ intLit ]");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_SIZE, line, column);
        add_child((yyval.ast_node), create_int_node((yyvsp[-1].int_val), line, column));
    }
#line 1732 "parser.tab.c"
    break;

  case 33: /* arraySize: LBRACKET RBRACKET  */
#line 383 "parser.y"
    { 
        write_derivation("arraySize → [ ]");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_SIZE, line, column);
        add_child((yyval.ast_node), create_int_node(-1, line, column)); // -1 indicates dynamic size
    }
#line 1742 "parser.tab.c"
    break;

  case 34: /* varDeclOrStmtList: localVarDeclOrStmt varDeclOrStmtList  */
#line 393 "parser.y"
    { 
        write_derivation("varDeclOrStmtList → localVarDeclOrStmt varDeclOrStmtList");
        (yyval.ast_node) = create_ast_node(AST_STMT_LIST, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1753 "parser.tab.c"
    break;

  case 35: /* varDeclOrStmtList: %empty  */
#line 400 "parser.y"
    { 
        write_derivation("varDeclOrStmtList → ε");
        (yyval.ast_node) = NULL;
    }
#line 1762 "parser.tab.c"
    break;

  case 36: /* localVarDeclOrStmt: localVarDecl  */
#line 408 "parser.y"
    { 
        write_derivation("localVarDeclOrStmt → localVarDecl");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1771 "parser.tab.c"
    break;

  case 37: /* localVarDeclOrStmt: statement  */
#line 413 "parser.y"
    { 
        write_derivation("localVarDeclOrStmt → statement");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1780 "parser.tab.c"
    break;

  case 38: /* statement: simpleStmt SEMI  */
#line 422 "parser.y"
    { 
        write_derivation("statement → simpleStmt ;");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
    }
#line 1789 "parser.tab.c"
    break;

  case 39: /* statement: ifStmt SEMI  */
#line 427 "parser.y"
    { 
        write_derivation("statement → ifStmt ;");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
    }
#line 1798 "parser.tab.c"
    break;

  case 40: /* statement: whileStmt SEMI  */
#line 432 "parser.y"
    { 
        write_derivation("statement → whileStmt ;");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
    }
#line 1807 "parser.tab.c"
    break;

  case 41: /* simpleStmt: idStmt  */
#line 440 "parser.y"
    { 
        write_derivation("simpleStmt → idStmt");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1816 "parser.tab.c"
    break;

  case 42: /* simpleStmt: selfStmt  */
#line 445 "parser.y"
    { 
        write_derivation("simpleStmt → selfStmt");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 1825 "parser.tab.c"
    break;

  case 43: /* simpleStmt: READ LPAREN variable RPAREN  */
#line 450 "parser.y"
    { 
        write_derivation("simpleStmt → read ( variable )");
        (yyval.ast_node) = create_ast_node(AST_READ_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1835 "parser.tab.c"
    break;

  case 44: /* simpleStmt: WRITE LPAREN expr RPAREN  */
#line 456 "parser.y"
    { 
        write_derivation("simpleStmt → write ( expr )");
        (yyval.ast_node) = create_ast_node(AST_WRITE_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1845 "parser.tab.c"
    break;

  case 45: /* simpleStmt: RETURN LPAREN expr RPAREN  */
#line 462 "parser.y"
    { 
        write_derivation("simpleStmt → return ( expr )");
        (yyval.ast_node) = create_ast_node(AST_RETURN_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1855 "parser.tab.c"
    break;

  case 46: /* idStmt: ID idStmtTail  */
#line 472 "parser.y"
    { 
        write_derivation("idStmt → id idStmtTail");
        (yyval.ast_node) = (yyvsp[0].ast_node);
        if ((yyval.ast_node)) {
            ASTNode* id_node = create_string_node(AST_ID, (yyvsp[-1].str_val), line, column);
            add_child((yyval.ast_node), id_node);
        }
    }
#line 1868 "parser.tab.c"
    break;

  case 47: /* idStmtTail: ASSIGN expr  */
#line 484 "parser.y"
    { 
        write_derivation("idStmtTail → := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1878 "parser.tab.c"
    break;

  case 48: /* idStmtTail: LBRACKET expr RBRACKET idArrayTail  */
#line 490 "parser.y"
    { 
        write_derivation("idStmtTail → [ expr ] idArrayTail");
        (yyval.ast_node) = (yyvsp[0].ast_node);
        if ((yyval.ast_node)) {
            ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
            if ((yyvsp[-2].ast_node)) add_child(array_access, (yyvsp[-2].ast_node));
            add_child((yyval.ast_node), array_access);
        }
    }
#line 1892 "parser.tab.c"
    break;

  case 49: /* idStmtTail: LPAREN aParams RPAREN  */
#line 500 "parser.y"
    { 
        write_derivation("idStmtTail → ( aParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1902 "parser.tab.c"
    break;

  case 50: /* idStmtTail: DOT ID idMemberTail  */
#line 506 "parser.y"
    { 
        write_derivation("idStmtTail → . id idMemberTail");
        (yyval.ast_node) = (yyvsp[0].ast_node);
        if ((yyval.ast_node)) {
            ASTNode* member_access = create_ast_node(AST_MEMBER_ACCESS, line, column);
            add_child(member_access, create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
            add_child((yyval.ast_node), member_access);
        }
    }
#line 1916 "parser.tab.c"
    break;

  case 51: /* idArrayTail: ASSIGN expr  */
#line 519 "parser.y"
    { 
        write_derivation("idArrayTail → := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1926 "parser.tab.c"
    break;

  case 52: /* idArrayTail: LBRACKET expr RBRACKET ASSIGN expr  */
#line 525 "parser.y"
    { 
        write_derivation("idArrayTail → [ expr ] := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-3].ast_node)) add_child(array_access, (yyvsp[-3].ast_node));
        add_child((yyval.ast_node), array_access);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1939 "parser.tab.c"
    break;

  case 53: /* idMemberTail: ASSIGN expr  */
#line 537 "parser.y"
    { 
        write_derivation("idMemberTail → := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1949 "parser.tab.c"
    break;

  case 54: /* idMemberTail: LBRACKET expr RBRACKET ASSIGN expr  */
#line 543 "parser.y"
    { 
        write_derivation("idMemberTail → [ expr ] := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-3].ast_node)) add_child(array_access, (yyvsp[-3].ast_node));
        add_child((yyval.ast_node), array_access);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1962 "parser.tab.c"
    break;

  case 55: /* idMemberTail: LPAREN aParams RPAREN  */
#line 552 "parser.y"
    { 
        write_derivation("idMemberTail → ( aParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 1972 "parser.tab.c"
    break;

  case 56: /* selfStmt: SELF DOT ID selfMemberTail  */
#line 562 "parser.y"
    { 
        write_derivation("selfStmt → self . id selfMemberTail");
        (yyval.ast_node) = (yyvsp[0].ast_node);
        if ((yyval.ast_node)) {
            ASTNode* self_member = create_ast_node(AST_MEMBER_ACCESS, line, column);
            add_child(self_member, create_string_node(AST_ID, "self", line, column));
            add_child(self_member, create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
            add_child((yyval.ast_node), self_member);
        }
    }
#line 1987 "parser.tab.c"
    break;

  case 57: /* selfMemberTail: ASSIGN expr  */
#line 576 "parser.y"
    { 
        write_derivation("selfMemberTail → := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 1997 "parser.tab.c"
    break;

  case 58: /* selfMemberTail: LBRACKET expr RBRACKET ASSIGN expr  */
#line 582 "parser.y"
    { 
        write_derivation("selfMemberTail → [ expr ] := expr");
        (yyval.ast_node) = create_ast_node(AST_ASSIGN_STMT, line, column);
        ASTNode* array_access = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-3].ast_node)) add_child(array_access, (yyvsp[-3].ast_node));
        add_child((yyval.ast_node), array_access);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2010 "parser.tab.c"
    break;

  case 59: /* selfMemberTail: LPAREN aParams RPAREN  */
#line 591 "parser.y"
    { 
        write_derivation("selfMemberTail → ( aParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_CALL_STMT, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2020 "parser.tab.c"
    break;

  case 60: /* ifStmt: IF LPAREN expr RPAREN THEN statBlock elseOpt  */
#line 601 "parser.y"
    { 
        write_derivation("ifStmt → if ( expr ) then statBlock elseOpt");
        (yyval.ast_node) = create_ast_node(AST_IF_STMT, line, column);
        if ((yyvsp[-4].ast_node)) add_child((yyval.ast_node), (yyvsp[-4].ast_node));  // condition
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));  // then block
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));  // else block
    }
#line 2032 "parser.tab.c"
    break;

  case 61: /* elseOpt: ELSE statBlock  */
#line 612 "parser.y"
    { 
        write_derivation("elseOpt → else statBlock");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 2041 "parser.tab.c"
    break;

  case 62: /* elseOpt: %empty  */
#line 617 "parser.y"
    { 
        write_derivation("elseOpt → ε");
        (yyval.ast_node) = NULL;
    }
#line 2050 "parser.tab.c"
    break;

  case 63: /* whileStmt: WHILE LPAREN expr RPAREN statBlock  */
#line 625 "parser.y"
    { 
        write_derivation("whileStmt → while ( expr ) statBlock");
        (yyval.ast_node) = create_ast_node(AST_WHILE_STMT, line, column);
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));  // condition
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));  // body
    }
#line 2061 "parser.tab.c"
    break;

  case 64: /* statBlock: LBRACE statementList RBRACE  */
#line 635 "parser.y"
    { 
        write_derivation("statBlock → { statementList }");
        (yyval.ast_node) = create_ast_node(AST_STAT_BLOCK, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2071 "parser.tab.c"
    break;

  case 65: /* statBlock: statement  */
#line 641 "parser.y"
    { 
        write_derivation("statBlock → statement");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 2080 "parser.tab.c"
    break;

  case 66: /* statBlock: %empty  */
#line 646 "parser.y"
    { 
        write_derivation("statBlock → ε");
        (yyval.ast_node) = NULL;
    }
#line 2089 "parser.tab.c"
    break;

  case 67: /* statementList: statement statementList  */
#line 654 "parser.y"
    { 
        write_derivation("statementList → statement statementList");
        (yyval.ast_node) = create_ast_node(AST_STMT_LIST, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2100 "parser.tab.c"
    break;

  case 68: /* statementList: %empty  */
#line 661 "parser.y"
    { 
        write_derivation("statementList → ε");
        (yyval.ast_node) = NULL;
    }
#line 2109 "parser.tab.c"
    break;

  case 69: /* variable: ID variableTail  */
#line 670 "parser.y"
    { 
        write_derivation("variable → id variableTail");
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
            ASTNode* id_node = create_string_node(AST_ID, (yyvsp[-1].str_val), line, column);
            add_child((yyval.ast_node), id_node);
        } else {
            (yyval.ast_node) = create_ast_node(AST_VARIABLE, line, column);
            add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
        }
    }
#line 2125 "parser.tab.c"
    break;

  case 70: /* variable: SELF DOT ID variableTail  */
#line 682 "parser.y"
    { 
        write_derivation("variable → self . id variableTail");
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
        } else {
            (yyval.ast_node) = create_ast_node(AST_MEMBER_ACCESS, line, column);
        }
        ASTNode* self_node = create_string_node(AST_ID, "self", line, column);
        ASTNode* member_node = create_string_node(AST_ID, (yyvsp[-1].str_val), line, column);
        add_child((yyval.ast_node), self_node);
        add_child((yyval.ast_node), member_node);
    }
#line 2142 "parser.tab.c"
    break;

  case 71: /* variableTail: LBRACKET expr RBRACKET variableTail  */
#line 698 "parser.y"
    { 
        write_derivation("variableTail → [ expr ] variableTail");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2153 "parser.tab.c"
    break;

  case 72: /* variableTail: DOT ID variableTail  */
#line 705 "parser.y"
    { 
        write_derivation("variableTail → . id variableTail");
        (yyval.ast_node) = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2164 "parser.tab.c"
    break;

  case 73: /* variableTail: %empty  */
#line 712 "parser.y"
    { 
        write_derivation("variableTail → ε");
        (yyval.ast_node) = NULL;
    }
#line 2173 "parser.tab.c"
    break;

  case 74: /* expr: relExpr  */
#line 721 "parser.y"
    { 
        write_derivation("expr → relExpr");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 2182 "parser.tab.c"
    break;

  case 75: /* relExpr: arithExpr relExprPrime  */
#line 729 "parser.y"
    { 
        write_derivation("relExpr → arithExpr relExprPrime");
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
            add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        } else {
            (yyval.ast_node) = (yyvsp[-1].ast_node);
        }
    }
#line 2196 "parser.tab.c"
    break;

  case 76: /* relExprPrime: relOp arithExpr  */
#line 742 "parser.y"
    { 
        write_derivation("relExprPrime → relOp arithExpr");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2206 "parser.tab.c"
    break;

  case 77: /* relExprPrime: %empty  */
#line 748 "parser.y"
    { 
        write_derivation("relExprPrime → ε");
        (yyval.ast_node) = NULL;
    }
#line 2215 "parser.tab.c"
    break;

  case 78: /* relOp: EQ  */
#line 756 "parser.y"
    { 
        write_derivation("relOp → ==");
        (yyval.ast_node) = create_binary_expr("==", NULL, NULL, line, column);
    }
#line 2224 "parser.tab.c"
    break;

  case 79: /* relOp: NE  */
#line 761 "parser.y"
    { 
        write_derivation("relOp → <>");
        (yyval.ast_node) = create_binary_expr("<>", NULL, NULL, line, column);
    }
#line 2233 "parser.tab.c"
    break;

  case 80: /* relOp: LT  */
#line 766 "parser.y"
    { 
        write_derivation("relOp → <");
        (yyval.ast_node) = create_binary_expr("<", NULL, NULL, line, column);
    }
#line 2242 "parser.tab.c"
    break;

  case 81: /* relOp: GT  */
#line 771 "parser.y"
    { 
        write_derivation("relOp → >");
        (yyval.ast_node) = create_binary_expr(">", NULL, NULL, line, column);
    }
#line 2251 "parser.tab.c"
    break;

  case 82: /* relOp: LE  */
#line 776 "parser.y"
    { 
        write_derivation("relOp → <=");
        (yyval.ast_node) = create_binary_expr("<=", NULL, NULL, line, column);
    }
#line 2260 "parser.tab.c"
    break;

  case 83: /* relOp: GE  */
#line 781 "parser.y"
    { 
        write_derivation("relOp → >=");
        (yyval.ast_node) = create_binary_expr(">=", NULL, NULL, line, column);
    }
#line 2269 "parser.tab.c"
    break;

  case 84: /* arithExpr: term arithExprPrime  */
#line 789 "parser.y"
    { 
        write_derivation("arithExpr → term arithExprPrime");
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
            add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        } else {
            (yyval.ast_node) = (yyvsp[-1].ast_node);
        }
    }
#line 2283 "parser.tab.c"
    break;

  case 85: /* arithExprPrime: addOp term arithExprPrime  */
#line 802 "parser.y"
    { 
        write_derivation("arithExprPrime → addOp term arithExprPrime");
        (yyval.ast_node) = (yyvsp[-2].ast_node);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2294 "parser.tab.c"
    break;

  case 86: /* arithExprPrime: %empty  */
#line 809 "parser.y"
    { 
        write_derivation("arithExprPrime → ε");
        (yyval.ast_node) = NULL;
    }
#line 2303 "parser.tab.c"
    break;

  case 87: /* addOp: PLUS  */
#line 817 "parser.y"
    { 
        write_derivation("addOp → +");
        (yyval.ast_node) = create_binary_expr("+", NULL, NULL, line, column);
    }
#line 2312 "parser.tab.c"
    break;

  case 88: /* addOp: MINUS  */
#line 822 "parser.y"
    { 
        write_derivation("addOp → -");
        (yyval.ast_node) = create_binary_expr("-", NULL, NULL, line, column);
    }
#line 2321 "parser.tab.c"
    break;

  case 89: /* addOp: OR  */
#line 827 "parser.y"
    { 
        write_derivation("addOp → or");
        (yyval.ast_node) = create_binary_expr("or", NULL, NULL, line, column);
    }
#line 2330 "parser.tab.c"
    break;

  case 90: /* term: factor termPrime  */
#line 835 "parser.y"
    { 
        write_derivation("term → factor termPrime");
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
            add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        } else {
            (yyval.ast_node) = (yyvsp[-1].ast_node);
        }
    }
#line 2344 "parser.tab.c"
    break;

  case 91: /* termPrime: multOp factor termPrime  */
#line 848 "parser.y"
    { 
        write_derivation("termPrime → multOp factor termPrime");
        (yyval.ast_node) = (yyvsp[-2].ast_node);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2355 "parser.tab.c"
    break;

  case 92: /* termPrime: %empty  */
#line 855 "parser.y"
    { 
        write_derivation("termPrime → ε");
        (yyval.ast_node) = NULL;
    }
#line 2364 "parser.tab.c"
    break;

  case 93: /* multOp: MULT  */
#line 863 "parser.y"
    { 
        write_derivation("multOp → *");
        (yyval.ast_node) = create_binary_expr("*", NULL, NULL, line, column);
    }
#line 2373 "parser.tab.c"
    break;

  case 94: /* multOp: DIV  */
#line 868 "parser.y"
    { 
        write_derivation("multOp → /");
        (yyval.ast_node) = create_binary_expr("/", NULL, NULL, line, column);
    }
#line 2382 "parser.tab.c"
    break;

  case 95: /* multOp: AND  */
#line 873 "parser.y"
    { 
        write_derivation("multOp → and");
        (yyval.ast_node) = create_binary_expr("and", NULL, NULL, line, column);
    }
#line 2391 "parser.tab.c"
    break;

  case 96: /* factor: INTLIT  */
#line 881 "parser.y"
    { 
        write_derivation("factor → intLit");
        (yyval.ast_node) = create_int_node((yyvsp[0].int_val), line, column);
    }
#line 2400 "parser.tab.c"
    break;

  case 97: /* factor: FLOATLIT  */
#line 886 "parser.y"
    { 
        write_derivation("factor → floatLit");
        (yyval.ast_node) = create_float_node((yyvsp[0].float_val), line, column);
    }
#line 2409 "parser.tab.c"
    break;

  case 98: /* factor: LPAREN expr RPAREN  */
#line 891 "parser.y"
    { 
        write_derivation("factor → ( expr )");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
    }
#line 2418 "parser.tab.c"
    break;

  case 99: /* factor: NOT factor  */
#line 896 "parser.y"
    { 
        write_derivation("factor → not factor");
        (yyval.ast_node) = create_unary_expr("not", (yyvsp[0].ast_node), line, column);
    }
#line 2427 "parser.tab.c"
    break;

  case 100: /* factor: sign factor  */
#line 901 "parser.y"
    { 
        write_derivation("factor → sign factor");
        (yyval.ast_node) = (yyvsp[-1].ast_node);
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2437 "parser.tab.c"
    break;

  case 101: /* factor: ID factorTail  */
#line 907 "parser.y"
    { 
        write_derivation("factor → id factorTail");
        ASTNode* id_node = create_string_node(AST_ID, (yyvsp[-1].str_val), line, column);
        if ((yyvsp[0].ast_node)) {
            (yyval.ast_node) = (yyvsp[0].ast_node);
            add_child((yyval.ast_node), id_node);
        } else {
            (yyval.ast_node) = create_ast_node(AST_VARIABLE, line, column);
            add_child((yyval.ast_node), id_node);
        }
    }
#line 2453 "parser.tab.c"
    break;

  case 102: /* factor: SELF factorSelfTail  */
#line 919 "parser.y"
    { 
        write_derivation("factor → self factorSelfTail");
        ASTNode* self_node = create_string_node(AST_ID, "self", line, column);
        (yyval.ast_node) = (yyvsp[0].ast_node);
        add_child((yyval.ast_node), self_node);
    }
#line 2464 "parser.tab.c"
    break;

  case 103: /* factorTail: LPAREN aParams RPAREN  */
#line 929 "parser.y"
    { 
        write_derivation("factorTail → ( aParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_CALL_EXPR, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2474 "parser.tab.c"
    break;

  case 104: /* factorTail: LBRACKET expr RBRACKET  */
#line 935 "parser.y"
    { 
        write_derivation("factorTail → [ expr ]");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2484 "parser.tab.c"
    break;

  case 105: /* factorTail: DOT ID factorMemberTail  */
#line 941 "parser.y"
    { 
        write_derivation("factorTail → . id factorMemberTail");
        (yyval.ast_node) = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2495 "parser.tab.c"
    break;

  case 106: /* factorTail: %empty  */
#line 948 "parser.y"
    { 
        write_derivation("factorTail → ε");
        (yyval.ast_node) = NULL;
    }
#line 2504 "parser.tab.c"
    break;

  case 107: /* factorMemberTail: LPAREN aParams RPAREN  */
#line 956 "parser.y"
    { 
        write_derivation("factorMemberTail → ( aParams )");
        (yyval.ast_node) = create_ast_node(AST_FUNC_CALL_EXPR, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2514 "parser.tab.c"
    break;

  case 108: /* factorMemberTail: LBRACKET expr RBRACKET  */
#line 962 "parser.y"
    { 
        write_derivation("factorMemberTail → [ expr ]");
        (yyval.ast_node) = create_ast_node(AST_ARRAY_ACCESS, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
    }
#line 2524 "parser.tab.c"
    break;

  case 109: /* factorMemberTail: %empty  */
#line 968 "parser.y"
    { 
        write_derivation("factorMemberTail → ε");
        (yyval.ast_node) = NULL;
    }
#line 2533 "parser.tab.c"
    break;

  case 110: /* factorSelfTail: DOT ID factorMemberTail  */
#line 976 "parser.y"
    { 
        write_derivation("factorSelfTail → . id factorMemberTail");
        (yyval.ast_node) = create_ast_node(AST_MEMBER_ACCESS, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-1].str_val), line, column));
        if ((yyvsp[0].ast_node)) add_child((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2544 "parser.tab.c"
    break;

  case 111: /* sign: PLUS  */
#line 986 "parser.y"
    { 
        write_derivation("sign → +");
        (yyval.ast_node) = create_unary_expr("+", NULL, line, column);
    }
#line 2553 "parser.tab.c"
    break;

  case 112: /* sign: MINUS  */
#line 991 "parser.y"
    { 
        write_derivation("sign → -");
        (yyval.ast_node) = create_unary_expr("-", NULL, line, column);
    }
#line 2562 "parser.tab.c"
    break;

  case 113: /* type: INTEGER_KW  */
#line 1000 "parser.y"
    { 
        write_derivation("type → integer");
        (yyval.ast_node) = create_type_node(TYPE_INTEGER, line, column);
    }
#line 2571 "parser.tab.c"
    break;

  case 114: /* type: FLOAT_KW  */
#line 1005 "parser.y"
    { 
        write_derivation("type → float");
        (yyval.ast_node) = create_type_node(TYPE_FLOAT, line, column);
    }
#line 2580 "parser.tab.c"
    break;

  case 115: /* type: ID  */
#line 1010 "parser.y"
    { 
        write_derivation("type → id");
        (yyval.ast_node) = create_ast_node(AST_TYPE, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[0].str_val), line, column));
    }
#line 2590 "parser.tab.c"
    break;

  case 116: /* returnType: type  */
#line 1019 "parser.y"
    { 
        write_derivation("returnType → type");
        (yyval.ast_node) = (yyvsp[0].ast_node);
    }
#line 2599 "parser.tab.c"
    break;

  case 117: /* returnType: VOID  */
#line 1024 "parser.y"
    { 
        write_derivation("returnType → void");
        (yyval.ast_node) = create_type_node(TYPE_VOID, line, column);
    }
#line 2608 "parser.tab.c"
    break;

  case 118: /* fParams: ID COLON type arraySizeList fParamsTail  */
#line 1033 "parser.y"
    { 
        write_derivation("fParams → id : type arraySizeList fParamsTail");
        (yyval.ast_node) = create_ast_node(AST_PARAM_DECL, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-4].str_val), line, column));
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_sibling((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2621 "parser.tab.c"
    break;

  case 119: /* fParams: %empty  */
#line 1042 "parser.y"
    { 
        write_derivation("fParams → ε");
        (yyval.ast_node) = NULL;
    }
#line 2630 "parser.tab.c"
    break;

  case 120: /* fParamsTail: COMMA ID COLON type arraySizeList fParamsTail  */
#line 1050 "parser.y"
    { 
        write_derivation("fParamsTail → , id : type arraySizeList fParamsTail");
        (yyval.ast_node) = create_ast_node(AST_PARAM_DECL, line, column);
        add_child((yyval.ast_node), create_string_node(AST_ID, (yyvsp[-4].str_val), line, column));
        if ((yyvsp[-2].ast_node)) add_child((yyval.ast_node), (yyvsp[-2].ast_node));
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_sibling((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2643 "parser.tab.c"
    break;

  case 121: /* fParamsTail: %empty  */
#line 1059 "parser.y"
    { 
        write_derivation("fParamsTail → ε");
        (yyval.ast_node) = NULL;
    }
#line 2652 "parser.tab.c"
    break;

  case 122: /* aParams: expr aParamsTail  */
#line 1067 "parser.y"
    { 
        write_derivation("aParams → expr aParamsTail");
        (yyval.ast_node) = create_ast_node(AST_ARG_LIST, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_sibling((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2663 "parser.tab.c"
    break;

  case 123: /* aParams: %empty  */
#line 1074 "parser.y"
    { 
        write_derivation("aParams → ε");
        (yyval.ast_node) = NULL;
    }
#line 2672 "parser.tab.c"
    break;

  case 124: /* aParamsTail: COMMA expr aParamsTail  */
#line 1082 "parser.y"
    { 
        write_derivation("aParamsTail → , expr aParamsTail");
        (yyval.ast_node) = create_ast_node(AST_ARG_LIST, line, column);
        if ((yyvsp[-1].ast_node)) add_child((yyval.ast_node), (yyvsp[-1].ast_node));
        if ((yyvsp[0].ast_node)) add_sibling((yyval.ast_node), (yyvsp[0].ast_node));
    }
#line 2683 "parser.tab.c"
    break;

  case 125: /* aParamsTail: %empty  */
#line 1089 "parser.y"
    { 
        write_derivation("aParamsTail → ε");
        (yyval.ast_node) = NULL;
    }
#line 2692 "parser.tab.c"
    break;


#line 2696 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1095 "parser.y"


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
