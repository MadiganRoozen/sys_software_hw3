#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "utilities.h"
#include "id_use.h"

//checks the program's ast for 
void scope_check_program(block_t program_AST);