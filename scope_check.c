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

	scope_check_block(scope_list, &program_AST);


}

//Starts all Declaration Checking of AST
void scope_check_block(linked_list* scope_list, block_t* prog) {

	//Entering new Scope
	scope_check_enter_scope(scope_list);

	//Checking Declarations
		//Constants
		scope_check_const_decls(scope_list->tail, prog->const_decls);

		//Variables
		scope_check_var_decls(scope_list->tail, prog->var_decls);

		//Procedures
		scope_check_proc_decls(scope_list, prog->proc_decls);

	//Checking Statements
	scope_check_statements(scope_list, prog->stmts);

	//Leaving Scope of Block
	scope_check_leave_scope(scope_list);
}

//Checks all Constant Declarations for Declaration Errors
void scope_check_const_decls(scope_node* cur_scope, const_decls_t consts_decls) {

	//Base Case - No Declarations Present
	if (consts_decls.start == NULL)
		return;

	const_decl_t* const_decl = consts_decls.start;

	//For Each Constant Declaration List in Scope
	while (const_decl != NULL) {
		const_def_list_t const_list = const_decl->const_def_list;

		//Base Case - No Declarations Present
		if (const_list.start == NULL) {
			const_decl = const_decl->next;
			continue;
		}

		const_def_t* const_def = const_list.start;

		//For Each Constant Declaration List
		while (const_def != NULL) {
			ident_t ident = const_def->ident;

			scope_check_declare_ident(cur_scope, &ident);
			
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
			var_decl = var_decl->next;
			continue;
		}

		ident_t* ident = ident_list.start;

		while (ident != NULL) {
			scope_check_declare_ident(cur_scope, ident);
			ident = ident->next;
		}
	}

}

void scope_check_proc_decls(linked_list* scope_list, proc_decls_t procs_decls) {

	//Base Case - No Declarations Present
	if (procs_decls.proc_decls == NULL)
		return;

	proc_decl_t* proc_decl = procs_decls.proc_decls;

	while (proc_decl != NULL) {
		ident_t* ident = malloc(sizeof(ident_t));
		ident->name = proc_decl->name;
		ident->file_loc = proc_decl->file_loc;
		ident->type_tag = proc_decl->type_tag;
		ident->next = NULL;

		scope_check_declare_ident(scope_list->tail, ident);

		// free(ident);

		scope_check_block(scope_list, proc_decl->block);
	}

}

void scope_check_statements(linked_list* scope_list, stmts_t stmts) {

	//Base Case - No Statements Present
	if (stmts.stmts_kind == empty_stmts_e)
		return;

	stmt_list_t state_list = stmts.stmt_list;

	//No Statements in List
	if (state_list.start == NULL)
		return;

	stmt_t* stmt = state_list.start;

	while (stmt != NULL) {

		switch (stmt->stmt_kind) {

			case assign_stmt:
				scope_check_assign_stmt(scope_list->tail, stmt);
				break;

			case call_stmt:

				break;

			case if_stmt:

				break;

			case while_stmt:

				break;

			case read_stmt:

				break;

			case print_stmt:

				break;

			case block_stmt:
				scope_check_block(scope_list, stmt->data.block_stmt.block);
				break;

			default:

				break;
		}

		stmt = stmt->next;

	}
}

void scope_check_assign_stmt(scope_node* cur_scope, stmt_t* stmt) {

	//Checking Identifier
		//Creating Ident_t for Identifier Name
		ident_t* ident = malloc(sizeof(ident_t));
		ident->name = stmt->data.assign_stmt.name;
		ident->file_loc = stmt->data.assign_stmt.file_loc;
		ident->type_tag = stmt->data.assign_stmt.type_tag;
		ident->next = NULL;

		//Checking Ident
		scope_check_declare_ident(cur_scope, ident);

		//Free - No Longer Needed
		// free(ident);

	//Checking Expression
	scope_check_expr(cur_scope, stmt->data.assign_stmt.expr);


}

void scope_check_expr(scope_node* cur_scope, expr_t* expr) {

	//Base Case - No Expression
	if (expr == NULL)
		return;

	switch (expr->expr_kind) {

		case expr_bin:
			scope_check_bin_expr(cur_scope, expr);
			break;

		case expr_negated:
			scope_check_neg_expr(cur_scope, expr);
			break;

		case expr_ident:
			scope_check_in_scope_vis(cur_scope, expr);
			break;

		case expr_number:
			
			break;

		default:

			break;
	}
}

void scope_check_bin_expr(scope_node* cur_scope, expr_t* expr) {

}

void scope_check_neg_expr(scope_node* cur_scope, expr_t* expr) {

}

void scope_check_in_scope_vis(scope_node* cur_scope, expr_t* expr) {

}



//Adds a new Scope Node to the End of the List
void scope_check_enter_scope(linked_list* list) {

	scope++;

	scope_node* s_node = malloc(sizeof(scope_node));
	s_node->scope = scope;
	s_node->next = NULL;
	s_node->prev = NULL;
	s_node->idents = NULL;
	s_node->idents_tail = NULL;

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

	scope_check_free_ident_list(list->tail);

	free(list->tail);
	list->tail = new_tail;
	list->tail->next = NULL;

}

//Add an Ident Node to the end of the Ident List
void scope_check_declare_ident(scope_node* cur_scope, ident_t* ident) {

	//Check if Ident Is Present In Scope Declaration Already
		//In Declarations Already - Bail With Error
		if (scope_check_in_scope_decl(cur_scope, ident)) {
			bail_with_prog_error(*(ident->file_loc), "Variable \"%s\" has already been declared!", ident->name);
		}
		//Not In Declarations Yet - Declare
		else {
			//New Node
			ident_node* new_node = malloc(sizeof(ident_node));
			new_node->next = NULL;
			new_node->ident = malloc(strlen(ident->name));
			strcpy(new_node->ident, ident->name);

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

bool scope_check_in_scope_decl(scope_node* cur_scope, ident_t* ident) {

	//Base Case - No Idents Present In Scope
	if (cur_scope->idents == NULL)
		return false;

	ident_node* identifier = cur_scope->idents;

	while (identifier != NULL) {
		if (strcmp(identifier->ident, ident->name) == 0)
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