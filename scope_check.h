#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "utilities.h"
#include "id_use.h"

//LinkedList Data Structures

typedef struct ident_node {
	char* ident;
	ident_node* next;
	ident_node* prev;
} ident_node;

typedef struct Node {
	int scope;
	Node* next;
	Node* prev;
	ident_node* idents;
} Node;



void scope_check_program(block_t program_AST);