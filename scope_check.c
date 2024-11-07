#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if it will be needed here, but left it in case
#include "id_attrs.h"
#include "ast.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//Implementation Utilized through LinkedLists

void scope_check_program(block_t program_AST) {
	
	SymbolTable sym_table = create_symbol_table();
}