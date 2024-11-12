#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"//not sure if it will be needed here, but left it in case
#include "id_attrs.h"
#include "ast.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"

//Global Current Scope Counter (never really used just for visuals)
int scope = 0;

//Implementation Utilized through LinkedLists

//"Main Method" if you will - Initializes Linked_List and starts AST checking Program
void scope_check_program(block_t program_AST) {
	
	linked_list* scope_list = malloc(sizeof(linked_list));
	scope_list->head = NULL;
	scope_list->tail = NULL;

	//Running Checks
	scope_check_block(scope_list, &program_AST);


}

//Starts all Declaration Checking of AST block_t* prog
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

//Checks Validity of constant declarations - If Already Declared, Error - If Not, Declare
void scope_check_const_decls(scope_node* cur_scope, const_decls_t consts_decls) {
	// printf("checking consts\n");
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
			// printf("checking const dec\n");

			char* var_type = malloc(sizeof(char)*9);
			strcpy(var_type, "constant");

			//Declare / Error on Declare
			scope_check_declare_ident(cur_scope, &ident, var_type);

			//Iterate
			const_def = const_def->next;
			
		}

		//Iterate
		const_decl = const_decl->next;

	}
}

//Checks Validity of variable declarations - If Already Declared, Error - If Not, Declare
void scope_check_var_decls(scope_node* cur_scope, var_decls_t vars_decls) {

	//Base Case - No Declarations Present
	if (vars_decls.var_decls == NULL)
		return;

	var_decl_t* var_decl = vars_decls.var_decls;

	//For Each Variable Declaration List in Scope
	while (var_decl != NULL) {
		ident_list_t ident_list = var_decl->ident_list;

		//Base Case - No Idents Present
		if (ident_list.start == NULL) {
			var_decl = var_decl->next;
			return;
		}

		ident_t* ident = ident_list.start;

		//Iterate Through All Idents
		while (ident != NULL) {
			// printf("declaring var ident: %s\n", ident->name);

			char* var_type = malloc(sizeof(char)*9);
			strcpy(var_type, "variable");

			//Declare / Error on Declare
			scope_check_declare_ident(cur_scope, ident, var_type);
			ident = ident->next;
		}

		//Iterate
		var_decl = var_decl->next;
	}

}

//Checks Validity of procedure declarations - If Already Declared, Error - If Not, Declare
void scope_check_proc_decls(linked_list* scope_list, proc_decls_t procs_decls) {

	//Base Case - No Declarations Present
	if (procs_decls.proc_decls == NULL)
		return;

	proc_decl_t* proc_decl = procs_decls.proc_decls;

	//Check all declarations
	while (proc_decl != NULL) {

		//Creating Ident for Usage
		ident_t* ident = malloc(sizeof(ident_t));
		ident->name = proc_decl->name;
		ident->file_loc = proc_decl->file_loc;
		ident->type_tag = proc_decl->type_tag;
		ident->next = NULL;

		char* var_type = malloc(sizeof(char)*10);
		strcpy(var_type, "procedure");

		//Declare / Error on Declare
		scope_check_declare_ident(scope_list->tail, ident, var_type);

		//Check Rest of Procedure Code
		scope_check_block(scope_list, proc_decl->block);

		//Iterate
		proc_decl = proc_decl->next;
	}

}

//Checks Validity of All Statements in stmts_t stmts - Throws Error if given improper stmt_kind
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

		//Switch For Each Statement Type
		switch (stmt->stmt_kind) {

			//Assign Statement
			case assign_stmt:
				scope_check_assign_stmt(scope_list->tail, stmt->data.assign_stmt);
				break;

			//Call Statement
			case call_stmt:
				scope_check_call_stmt(scope_list->tail, stmt->data.call_stmt);
				break;

			//If Statement
			case if_stmt:
				scope_check_if_stmt(scope_list, stmt->data.if_stmt);
				break;

			//While Statement
			case while_stmt:
				scope_check_while_stmt(scope_list, stmt->data.while_stmt);
				break;

			//Read Statement
			case read_stmt:
				scope_check_read_stmt(scope_list->tail, stmt->data.read_stmt);
				break;

			//Print Statement
			case print_stmt:
				scope_check_print_stmt(scope_list->tail, stmt->data.print_stmt);
				break;

			//Block Statement
			case block_stmt:
				scope_check_block(scope_list, stmt->data.block_stmt.block);
				break;

			//Error - Improper Statement Given
			default:
				//Unsure of Implementation - Improper Statement Given
				bail_with_error("Unexpected stmt_kind (%d) in scope_check_condition", stmt->stmt_kind);
				break;
		}

		//Iterate to Next Statement
		stmt = stmt->next;

	}
}

//Checks Validity of assign statement stmt
void scope_check_assign_stmt(scope_node* cur_scope, assign_stmt_t stmt) {

	//Checking Identifier
		//Creating Ident_t for Identifier Name
		ident_t* ident = malloc(sizeof(ident_t));
		ident->name = stmt.name;
		ident->file_loc = stmt.file_loc;
		ident->type_tag = stmt.type_tag;
		ident->next = NULL;

		//Checking Ident
		scope_check_in_scope_vis(cur_scope, *ident);

		//Free - No Longer Needed
		free(ident);

	//Checking Expression
	scope_check_expr(cur_scope, stmt.expr);


}

//Checks Validity of call statement stmt
void scope_check_call_stmt(scope_node* cur_scope, call_stmt_t stmt) {

	//Creating Call Ident
	ident_t* ident = malloc(sizeof(ident_t));
	ident->name = stmt.name;
	ident->file_loc = stmt.file_loc;
	ident->type_tag = stmt.type_tag;
	ident->next = NULL;


	//Check If Call Ident is Declared Already
	scope_check_in_scope_vis(cur_scope, *ident);

	//Free Ident
	free(ident);
}

//Checks Validity of if statement stmt
void scope_check_if_stmt(linked_list* scope_list, if_stmt_t stmt) {
	
	//Check Condition
	scope_check_condition(scope_list->tail, stmt.condition);

	//Check Then Statements if Not Null
	if (stmt.then_stmts != NULL)
		scope_check_statements(scope_list, *(stmt.then_stmts));

	//Check Else if Else is Not Null
	if (stmt.else_stmts != NULL)
		scope_check_statements(scope_list, *(stmt.else_stmts));

}

//Checks Validity of while statement stmt
void scope_check_while_stmt(linked_list* scope_list, while_stmt_t stmt) {
	
	//Check Condition
	scope_check_condition(scope_list->tail, stmt.condition);

	//Check Body Statements
	if (stmt.body != NULL)
		scope_check_statements(scope_list, *(stmt.body));

}	

//Checks Validity of read statement stmt
void scope_check_read_stmt(scope_node* cur_scope, read_stmt_t stmt) {
	
	//Create Identifier
	ident_t* ident = malloc(sizeof(ident_t));
	ident->name = stmt.name;
	ident->file_loc = stmt.file_loc;
	ident->type_tag = stmt.type_tag;
	ident->next = NULL;

	//Check if Read ident is Declared and Ready
	scope_check_in_scope_vis(cur_scope, *ident);

	//Free Ident
	free(ident);

}

//Checks Validity of print statement stmt
void scope_check_print_stmt(scope_node* cur_scope, print_stmt_t stmt) {
	
	//Check Expression
	scope_check_expr(cur_scope, &(stmt.expr));
}

//Checks Validity of condition_t stmt based on all possible condition types - Throws Error is improper cond_kind given
void scope_check_condition(scope_node* cur_scope, condition_t stmt) {

	//Switch on Kind of Condition
	switch (stmt.cond_kind) {

		case ck_db:
			scope_check_db_condition(cur_scope, stmt.data.db_cond);
			break;

		case ck_rel:
			scope_check_rel_condition(cur_scope, stmt.data.rel_op_cond);
			break;

		default:
			//Unsure of Implimentation - Improper Condition
			bail_with_error("Unexpected cond_kind (%d) in scope_check_condition", stmt.cond_kind);
			break;
	}
}

//Checks Validity of divisible condition db_cond
void scope_check_db_condition(scope_node* cur_scope, db_condition_t db_cond) {

	//Check Conditions Divisor
	scope_check_expr(cur_scope, &(db_cond.divisor));

	//Check Conditions Dividend
	scope_check_expr(cur_scope, &(db_cond.dividend));
}

//Checks Validity of relative operator condition rel_cond
void scope_check_rel_condition(scope_node* cur_scope, rel_op_condition_t rel_cond) {
	
	//Check Expression 1
	scope_check_expr(cur_scope, &(rel_cond.expr1));

	//Check Expression 2
	scope_check_expr(cur_scope, &(rel_cond.expr2));
}	

//Checks Validity of expr_t* expr based on each possible expression type - Throws Error if improper expr_kind given
void scope_check_expr(scope_node* cur_scope, expr_t* expr) {

	//Base Case - No Expression
	if (expr == NULL)
		return;

	switch (expr->expr_kind) {

		case expr_bin:
			scope_check_bin_expr(cur_scope, expr->data.binary);
			break;

		case expr_negated:
			scope_check_neg_expr(cur_scope, expr->data.negated);
			break;

		case expr_ident:
			scope_check_in_scope_vis(cur_scope, expr->data.ident);
			break;

		case expr_number:
			//Nothing to Check
			break;

		default:
			//Unsure of Implementation - Improper Expression Given
			bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr", expr->expr_kind);
			break;
	}
}

//Check Validity of binary operator expression bin_expr
void scope_check_bin_expr(scope_node* cur_scope, binary_op_expr_t bin_expr) {

	//Check Expression 1
	scope_check_expr(cur_scope, bin_expr.expr1);

	//Check Expression 2
	scope_check_expr(cur_scope, bin_expr.expr2);

}

//Check Validity of negated expression neg_expr
void scope_check_neg_expr(scope_node* cur_scope, negated_expr_t neg_expr) {

	//Check Expression
	scope_check_expr(cur_scope, neg_expr.expr);

}

//Determine if ident matches and ident_node*s present in cur_scope and upwards (all nested scopes)
void scope_check_in_scope_vis(scope_node* cur_scope, ident_t ident) {

	//Check Current Scope And Work Upwards Towards Head
	while (cur_scope != NULL) {
		ident_node* identifiers = cur_scope->idents;

		//Check Identifiers List
		while (identifiers != NULL) {

			if (strcmp(identifiers->name, ident.name) == 0)
				return;

			identifiers = identifiers->next;
		}

		//Iterate
		cur_scope = cur_scope->prev;
	}

	//Error if not ever found - Usage of undeclared identifier
	bail_with_prog_error(*(ident.file_loc), "identifier \"%s\" is not declared!", ident.name);

}

//Enters a new scope via allocating and adding a new scope_node* to the end of the linked list
void scope_check_enter_scope(linked_list* list) {

	//Scope Value Increased
	scope++;

	//New Node Allocation
	scope_node* s_node = malloc(sizeof(scope_node));
	s_node->scope = scope;
	s_node->next = NULL;
	s_node->prev = NULL;
	s_node->idents = NULL;
	s_node->idents_tail = NULL;

	//List is Empty
	if (list->head == NULL) {
		list->head = s_node;
		list->tail = s_node;

	} 

	//List is Not Empty
	else {
		s_node->prev = list->tail;
		list->tail->next = s_node;
		list->tail = s_node;
	}

}

//Leaves the current scope via freeing and removing the end scope_node* of the linked list
void scope_check_leave_scope(linked_list* list) {

	//Decreasing Scope Value
	scope--;

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

	//Fixing Tail
	scope_node* new_tail = list->tail->prev;

	//Freeing Original Removal
	scope_check_free_ident_list(list->tail);

	free(list->tail);
	list->tail = new_tail;
	list->tail->next = NULL;

}

//Checks if ident is present in cur_scope identifiers already - Adds if not present, error if present
void scope_check_declare_ident(scope_node* cur_scope, ident_t* ident, char* type) {
	// printf("checking ident dec\n");
	//Check if Ident Is Present In Scope Declaration Already
	ident_node* decl_found = scope_check_in_scope_decl(cur_scope, ident);

		//In Declarations Already - Bail With Error
		if (decl_found != NULL) {

			bail_with_prog_error(*(ident->file_loc), "%s \"%s\" is already declared as a %s", type, ident->name, decl_found->var_type);
		}
		//Not In Declarations Yet - Declare
		else {

			//New Node Allocations / Setup
			ident_node* new_node = malloc(sizeof(ident_node));
			new_node->next = NULL;
			new_node->name = malloc(strlen(ident->name));
			strcpy(new_node->name, ident->name);
			new_node->var_type = type;

			//Check if Ident List in Scope is Empty
			if (cur_scope->idents == NULL) {
				cur_scope->idents = new_node;
				cur_scope->idents_tail = new_node;
				return;
			}

			//Adding Ident to End of Ident List - List is Not Empty
			cur_scope->idents_tail->next = new_node;
			cur_scope->idents_tail = new_node;

		}
}

//Search & return ident_node* found in cur_scope that matches ident - Returns NULL if not found
ident_node* scope_check_in_scope_decl(scope_node* cur_scope, ident_t* ident) {

	//Base Case - No Idents Present In Scope
	if (cur_scope->idents == NULL)
		return NULL;

	ident_node* identifier = cur_scope->idents;

	//Searching Idents
	while (identifier != NULL) {
		if (strcmp(identifier->name, ident->name) == 0)
			return identifier;
		
		identifier = identifier->next;
	}

	return NULL;
}

//Free the ident_node* idents list from scope_node* node
void scope_check_free_ident_list(scope_node* node) {

	//Check ident list of tail to free
	if (node->idents != NULL) {
		ident_node* ident = node->idents;
		ident_node* next_node;

		//For Each Identifier - Free Attributes
		while (ident != NULL) {

			next_node = ident->next;
			free(ident->name);
			free(ident->var_type);
			free(ident);
			ident = next_node;

		}

	}

}