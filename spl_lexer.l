/* $Id: spl_lexer_definitions_top.l,v 1.4 2024/10/06 19:16:25 leavens Exp leavens $ */
/* Lexical Analyzer for SPL */

%option header-file = "spl_lexer.h"
%option outfile = "spl_lexer.c"
%option yylineno
%option bison-bridge

%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include "ast.h"
#include "parser_types.h"
#include "utilities.h"
#include "lexer.h"

 /* Tokens generated by Bison */
#include "spl.tab.h"

 /* need declaration of fileno, part of the C standard library.
   (Putting an extern declaration here shuts off a gcc warning.) */
extern int fileno(FILE *stream);

/* The filename of the file being read */
static char *input_filename;

/* Have any errors been noted? */
static bool errors_noted;

/* The value of a token */
extern YYSTYPE yylval;

/* The FILE used by the generated lexer */
extern FILE *yyin;

// We are not using yyunput or input
#define YY_NO_UNPUT
#define YY_NO_INPUT

#undef yywrap   /* sometimes a macro by default */

// apparently strdup is not declared in <string.h>
extern char *strdup(const char *s);

// set the lexer's value for a token in yylval as an AST
static void tok2ast(int code) {
    AST t;
    t.token.file_loc = file_location_make(input_filename, yylineno);
    t.token.type_tag = token_ast;
    t.token.code = code;
    t.token.text = strdup(yytext);
    yylval = t;
}

static void ident2ast(const char *name) {
    AST t;
    assert(input_filename != NULL);
    t.ident.file_loc = file_location_make(input_filename, yylineno);
    t.ident.type_tag = ident_ast;
    t.ident.name = strdup(name);
    yylval = t;
}

static void number2ast(unsigned int val)
{
    AST t;
    t.number.file_loc = file_location_make(input_filename, yylineno);
    t.number.type_tag = number_ast;
    t.number.text = strdup(yytext);
    t.number.value = val;
    yylval = t;
}

/* Error Message Buffer Allocation */
#define BUFFER_SIZE 100

%}

 /* you can add actual definitions below, before the %% */

DIGIT [0-9]
LETTER [a-zA-Z]
LETTER_OR_DIGIT [a-zA-Z0-9]
IDENT {LETTER}{LETTER-OR-DIGIT}*
NUMBER {DIGIT}+

PLUS \+
MINUS -
MULT \*
DIV /

PUNCTUATION ('.'|';'|'='|','|':='|'('|')')
RESERVED_WORD ('const'|'var'|'proc'|'call'|'begin'|'end'|'if'|'then'|'else'|'while'|'do'|'read'|'print'|'skip'|'divisible'|'by')
REL_OPS ('=='|'!='|'<'|'<='|'>'|'>=')

BLANK [ ]
TAB [\t]
VT [\v]
FORMFEED [\f]
NEWLINE [\n]
CR [\r]
PERCENT-SIGN '%'
NON_NL [^\n]
EOL ({NEWLINE}|{CR}{NEWLINE})
COMMENT {PERCENT-SIGN}{NON-NL}*{NEWLINE}
IGNORED ({BLANK}|{TAB}|{VT}|{FORMFEED}|{EOL}|{COMMENT})

 /* the rules section starts after the %% below */

%%

/* Rules and Tokens to be Returned / Error Handling */

"const"        { return constsym; }         /* const = 271 */
"var"          { return varsym; }           /* var = 272 */
"proc"         { return procsym; }          /* proc = 273 */
"call"         { return callsym; }          /* call = 274 */
"begin"        { return beginsym; }         /* begin = 275 */
"end"          { return endsym; }           /* end = 276 */
"if"           { return ifsym; }            /* if = 277 */
"then"         { return thensym; }          /* then = 278 */
"else"         { return elsesym; }          /* else = 279 */
"while"        { return whilesym; }         /* while = 280 */
"do"           { return dosym; }            /* do = 281 */
"read"         { return readsym; }          /* read = 282 */
"print"        { return printsym; }         /* print = 283 */
"divisible"    { return divisiblesym; }     /* divisible = 284 */
"by"           { return bysym; }            /* by = 285 */

"+"            { return plussym; }          /* "+" = 260 */
"-"            { return minussym; }         /* "-" = 261 */
"*"            { return multsym; }          /* "*" = 262 */
"/"            { return divsym; }           /* "/" = 263 */
"="            { return eqsym; }            /* "=" = 266 */
":="           { return becomessym; }       /* ":=" = 268 */
"=="           { return eqeqsym; }          /* "==" = 286 */
"!="           { return neqsym; }           /* "!=" = 287 */
"<"            { return ltsym; }            /* "<" = 288 */
"<="           { return leqsym; }           /* "<=" = 289 */
">"            { return gtsym; }            /* ">" = 290 */
">="           { return geqsym; }           /* ">=" = 291 */


[a-zA-Z][a-zA-Z0-9]* {
    return identsym;  /* Identifier symbol = 258 */
}


[0-9]+ { 

    /* Too Large Number Handling */

    int length = strlen(yytext);
    char* endchar;


    if (length <= 10) {
        long int num = strtol(yytext, &endchar, 10);

        if (num <= INT_MAX)
            return numbersym;

    }

    /* Error Message Buffer Memory Allocation */
    char buffer[BUFFER_SIZE];

    /* Error Message Creation */
    snprintf(buffer, sizeof(buffer), "Number (%s) is too large!\n", yytext);

    /* Calling yyerror Function to Print Error Message */
    yyerror(lexer_filename(), buffer);

    return numbersym;  /* Number symbol = 259 */
}


";"            { return semisym; }          /* ";" = 265 */
"."            { return periodsym; }        /* "." = 264 */
","            { return commasym; }         /* "," = 267 */
"("            { return lparensym; }        /* "(" = 269 */
")"            { return rparensym; }        /* ")" = 270 */

[ \t\r\n]+          { /* Ignore whitespace */ }
"%"[^\n]*         { /* Ignore comments starting with % */ }

/* Any Other Invalid Characters */
.              {
    
    /* Allocating Memory for Buffer Error Message */
    char buffer[BUFFER_SIZE];
    
    /* Creating and Storing Error Message */
    snprintf(buffer, sizeof(buffer),"invalid character: '%c' ('\\%03o')", yytext[0], yytext[0]);
    
    /* Calling/Handling Error Function */
    yyerror(lexer_filename(), buffer);
    
    return YYerror;  /* Error token = 256 */
}

%%

void print_lex_char(char* token) {
    printf("");
}

 /* This code goes in the user code section of the spl_lexer.l file,
   following the last %% above. */

// Requires: fname != NULL
// Requires: fname is the name of a readable file
// Initialize the lexer and start it reading
// from the given file name
void lexer_init(char *fname)
{
    errors_noted = false;
    yyin = fopen(fname, "r");
    if (yyin == NULL) {
	bail_with_error("Cannot open %s", fname);
    }
    input_filename = fname;
}

// Close the file yyin
// and return 0 to indicate that there are no more files
int yywrap() {
    if (yyin != NULL) {
	int rc = fclose(yyin);
	if (rc == EOF) {
	    bail_with_error("Cannot close %s!", input_filename);
	}
    }
    input_filename = NULL;
    return 1;  /* no more input */
}

// Return the name of the current input file
const char *lexer_filename() {
    return input_filename;
}

// Return the line number of the next token
unsigned int lexer_line() {
    return yylineno;
}

/* Report an error to the user on stderr */
void yyerror(const char *filename, const char *msg)
{
    fflush(stdout);
    fprintf(stderr, "%s:%d: %s\n", input_filename, lexer_line(), msg);
    errors_noted = true;
}

// On standard output:
// Print a message about the file name of the lexer's input
// and then print a heading for the lexer's output.
void lexer_print_output_header()
{
    printf("Tokens from file %s\n", lexer_filename());
    printf("%-6s %-4s  %s\n", "Number", "Line", "Text");
}

// Have any errors been noted by the lexer?
bool lexer_has_errors()
{
    return errors_noted;
}

// Print information about the token t to stdout
// followed by a newline
void lexer_print_token(enum yytokentype t, unsigned int tline,
		       const char *txt)
{
    printf("%-6d %-4d \"%s\"\n", t, tline, txt);
}

/* Read all the tokens from the input file
 * and print each token on standard output
 * using the format in lexer_print_token */
void lexer_output()
{
    lexer_print_output_header();
    AST dummy;
    yytoken_kind_t t;
    do {
	t = yylex(&dummy);
	if (t == YYEOF) {
	    break;
        }

    /* Only Printing Token Output if Token is Not An Error */
    if (t != YYerror)
        lexer_print_token(t, yylineno, yytext);

    } while (t != YYEOF);
}