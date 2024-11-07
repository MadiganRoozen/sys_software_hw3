#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"//not sure if it will be needed here, but left it in case
#include "id_attrs.h"
#include "ast.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//Implementation Utilized through LinkedLists

void scope_check_program(block_t program_AST) {
	
	linked_list* scope_list = malloc(sizeof(linked_list));
	scope_list->head = NULL;
	scope_list->tail = NULL;

	//Test printing AST to see structure
	stmts_t* statements = program_AST->stmts;


}

//Adds a new Scope Node to the End of the List
void add_scope_node(linked_list* list, int scope) {

	scope_node* s_node = malloc(sizeof(scope_node));
	s_node->scope = scope;
	s_node->next = NULL;
	s_node->prev = NULL;
	s_node->idents = NULL;

	if (list->head == NULL) {
		list->head = s_node;
		list->tail = s_node;

	} 
	else {
		s_node->prev = list->tail;
		list->tail->next = s_node;
		list->tail = s_node;
	}

}

//Removes the Scope Node at the End of the list
void remove_scope_node(linked_list* list) {

	//Check if List is Already Empty
	if (list->head == NULL)
		return;

	//Check if Only One Node in List
	if (list->head == list->tail) {
		free(list->head);
		list->head = NULL;
		list->tail = NULL;
		return;
	}

	scope_node* new_tail = list->tail->prev;

	free_ident_list(list->tail);

	free(list->tail);
	list->tail = new_tail;
	list->tail->next = NULL;

}

//Add an Ident Node to the end of the Ident List
void add_ident_node(ident_node* node, char* ident) {

	//New Node
	ident_node* new_node = malloc(sizeof(ident_node));
	new_node->next = NULL;
	new_node->prev = NULL;
	new_node->ident = NULL;

	//Traverse to end of ident list
	while (node->next != NULL)
		node = node->next;

	
	node->next = new_node;
	new_node->prev = node;
	new_node->ident = malloc(strlen(ident));
	strcpy(new_node->ident, ident);


}

//Free ident list inside a scope_node
void free_ident_list(scope_node* node) {

	//Check ident list of tail to free
	if (node->idents != NULL) {
		ident_node* ident = node->idents;
		ident_node* next_node;

		while (ident != NULL) {

			next_node = ident->next;
			free(ident->ident);
			ident = ident->next;
			free(ident);
			ident = next_node;

		}

	}
}