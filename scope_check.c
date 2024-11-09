#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"//not sure if it will be needed here, but left it in case
#include "id_attrs.h"
#include "ast.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//Global Current Scope Counter
int scope = 0;

//Implementation Utilized through LinkedLists

void scope_check_program(block_t program_AST) {
	
	linked_list* scope_list = malloc(sizeof(linked_list));
	scope_list->head = NULL;
	scope_list->tail = NULL;

	scope_check_block(scope_list, program_AST);


}

//Starts all Declaration Checking of AST
void scope_check_block(linked_list* scope_list, block_t prog) {

	//Entering new Scope
	scope_check_enter_scope(scope_list);

	//Checking Declarations
		//Constants
		scope_check_const_decls(scope_list->tail, prog.consts_decls);

		//Variables
		scope_check_var_decls(scope_list->tail, prog.var_decls);

		//Procedures

	//Checking Statements
		

}

//Checks all Constant Declarations for Declaration Errors
void scope_check_const_decls(scope_node* cur_scope, const_decls_t consts_decls) {

	//Base Case - No Declarations Present
	if (consts_decls.start == NULL)
		return;

	const_decl_t* const_decl = consts_decls.start;

	//For Each Constant Declaration List in Scope
	while (const_decl != NULL) {
		const_def_list_t const_list = const_decl.const_def_list;

		//Base Case - No Declarations Present
		if (const_list.start == NULL) {
			const_decl = const_decl->next;
			continue;
		}

		const_def_t* const_def = const_list.start;

		//For Each Constant Declaration List
		while (const_def != NULL) {
			ident_t ident = const_def.ident;

			//For Each Ident in Ident List
			while (ident != NULL) {
				scope_check_declare_ident(cur_scope, ident);
				ident = ident->next;
			}
		}

	}
}

void scope_check_var_decls(scope_node* cur_scope, var_decls_t vars_decls) {

	//Base Case - No Declarations Present
	if (vars_decls.var_decls == NULL)
		return;

	var_decl_t* var_decl = vars_decls.var_decls;

	//For Each Variable Declaration List in Scope
	while (var_decl != NULL) {
		ident_list_t ident_list = var_decl->ident_list;

		if (ident_list.start == NULL) {
			var_decl = vars_decl->next;
			continue;
		}

		//Ending Here for Night

	}

}

//Adds a new Scope Node to the End of the List
void scope_check_enter_scope(linked_list* list) {

	scope++;

	scope_node* s_node = malloc(sizeof(scope_node));
	s_node->scope = scope;
	s_node->next = NULL;
	s_node->prev = NULL;
	s_node->idents = NULL;
	s_node->ident_tail = NULL;

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
void scope_check_leave_scope(linked_list* list) {

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
void scope_check_declare_ident(scope_node* cur_scope, ident_t ident) {

	//Check if Ident Is Present In Scope Declaration Already
		//In Declarations Already - Bail With Error
		if (scope_check_in_scope_decl(cur_scope, ident)) {
			bail_with_prog_error(ident.file_loc, "Variable \"%s\" has already been declared!", ident.name);
		}
		//Not In Declarations Yet - Declare
		else {
			//New Node
			ident_node* new_node = malloc(sizeof(ident_node));
			new_node->next = NULL;
			new_node->ident = malloc(strlen(ident));
			strcpy(new_node->ident, ident);

			//Check if Ident List in Scope is Empty
			if (cur_scope->idents == NULL) {
				cur_scope->idents = new_node;
				cur_scope->idents_tail = new_node;
				return;
			}

			//Adding Ident to End of Ident List
			cur_scope->idents_tail->next = new_node;
			cur_scope->idents_tail = new_node;

		}
}

bool scope_check_in_scope_decl(scope_node* cur_scope, ident_t ident) {

	//Base Case - No Idents Present In Scope
	if (cur_scope->idents == NULL)
		return false;

	ident_t* identifier = cur_scope->idents;

	while (identifier != NULL) {
		if (strcmp(identifier->name, ident.name) == 0)
			return true;
	}

	return false;
}

//Free ident list inside a scope_node
void scope_check_free_ident_list(scope_node* node) {

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