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
	struct ident_node* next;
	struct ident_node* prev;
} ident_node;

typedef struct scope_node {
	int scope;
	struct scope_node* next;
	struct scope_node* prev;
	ident_node* idents;
} scope_node;

typedef struct linked_list {
	scope_node* head;
	scope_node* tail;
} linked_list;


void scope_check_program(block_t program_AST);

void add_scope_node(linked_list* list, int scope);

void remove_scope_node(linked_list* list);

void add_ident_node(ident_node* node, char* ident);

void free_ident_list(scope_node* node);