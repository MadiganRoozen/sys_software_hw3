#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "id_attrs.h"
#include "ast.h"
#include "symtab.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//struct for scope objects to be put into SymbolTable
typedef struct Scope{

} Scope;

//creates a Scope object 
extern Scope create();