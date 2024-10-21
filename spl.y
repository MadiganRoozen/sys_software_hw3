 /* $Id: bison_spl_y_top.y,v 1.2 2024/10/09 18:18:55 leavens Exp $ */

%code top {
#include <stdio.h>
}

%code requires {

 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

 /* the following passes file_name to yyerror,
    and declares it as an formal parameter of yyparse. */
%parse-param { char const *file_name }

%token <ident> identsym
%token <number> numbersym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token <token> multsym    "*"
%token <token> divsym     "/"

%token <token> periodsym  "."
%token <token> semisym    ";"
%token <token> eqsym      "="
%token <token> commasym   ","
%token <token> becomessym ":="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%token <token> constsym   "const"
%token <token> varsym     "var"
%token <token> procsym    "proc"
%token <token> callsym    "call"
%token <token> beginsym   "begin"
%token <token> endsym     "end"
%token <token> ifsym      "if"
%token <token> thensym    "then"
%token <token> elsesym    "else"
%token <token> whilesym   "while"
%token <token> dosym      "do"
%token <token> readsym    "read"
%token <token> printsym   "print"
%token <token> divisiblesym "divisible"
%token <token> bysym      "by"

%token <token> eqeqsym    "=="
%token <token> neqsym     "!="
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="

%type <block> program

%type <block> block

%type <const_decls> constDecls
%type <const_decl> constDecl
%type <const_def_list> constDefList
%type <const_def> constDef

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <ident_list> identList

%type <proc_decls> procDecls
%type <proc_decl> procDecl


%type <stmts> stmts
%type <empty> empty
%type <stmt_list> stmtList
%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <call_stmt> callStmt
%type <if_stmt> ifStmt
%type <while_stmt> whileStmt
%type <read_stmt> readStmt
%type <print_stmt> printStmt
%type <block_stmt> blockStmt

%type <condition> condition
%type <db_condition> dbCondition
%type <rel_op_condition> relOpCondition
%type <token> relOp

%type <expr> expr
%type <expr> term
%type <expr> factor

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
block_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(block_t t);
}

%%
 /* Write your grammar rules below and before the next %% */

program: block periodsym;

block: beginsym constDecls varDecls procDecls stmts endsym;

constDecls: constDecl;
constDecl: constsym constDefList;
constDefList: constDef | constDefs commasym constDef;
constDef: ident eqsym number;

varDecls: varDecl
varDecl: varsym identList;
identList: ident | idents commasym ident

procDecls: procDecl;
procDecl: procsym ident block;

stms: empty | stmtList;
empty: /*empty*/;
stmtList: stms | stmtList semisym stmt
stmt: assignStmt | callStmt | ifStmt | whileStmt | readStmt | printStmt | blockStmt;
assignStmt: ident becomessym expr;
callStmt: callsym ident;
ifStmt: ifsym condition thensym stmts elsesym stmts endsym | ifsym condition thensm stmts endsym;
whileStmt: whilesym condition dosym stmts endsym;
readStmt: readsym ident;
printStmt: printsym expr;
blockStmt: block;

condition: dbCondition | relOpCondition;
dbCondition: divisiblesym expr bysym expr;
relOpCondition: expr relOp expr;
relOp: eqeqsym | neqsym | ltsym | leqsym | gtsym | geqsym;

expr: | expr plus term | expr minus term;
term factor | term mult factor | term div factor;
factor ident | number | sign factor | lparensym expr rparensym;
sign: minus | plus;


%%

// Set the program's ast to be ast
void setProgAST(block_t ast) { progast = ast; }

