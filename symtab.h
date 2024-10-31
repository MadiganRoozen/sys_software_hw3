#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "symtab.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//struct for the symbol table. SymbolTable should be a stack of Scope objects from the scope files
typedef struct SymbolTable{

} SymbolTable;

//initialize the symbol table
extern void init();

//create a scope object and add it to the stack
extern void add_scope();

//remove a scope object from the stack
extern void remove_scope();
