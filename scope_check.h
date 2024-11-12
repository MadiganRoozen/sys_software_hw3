#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "utilities.h"
#include "id_use.h"


//LinkedList Data Structures

typedef struct ident_node {
	char* name;
	char* var_type;
	struct ident_node* next;
} ident_node;

typedef struct scope_node {
	int scope;
	struct scope_node* next;
	struct scope_node* prev;
	ident_node* idents;
	ident_node* idents_tail;
} scope_node;

typedef struct linked_list {
	scope_node* head;
	scope_node* tail;
} linked_list;

//"Main Method" if you will - Initializes Linked_List and starts AST checking Program
void scope_check_program(block_t program_AST);

//Starts all Declaration Checking of AST block_t* prog
void scope_check_block(linked_list* scope_list, block_t* prog);

//Checks Validity of constant declarations - If Already Declared, Error - If Not, Declare
void scope_check_const_decls(scope_node* cur_scope, const_decls_t consts_decls);

//Checks Validity of variable declarations - If Already Declared, Error - If Not, Declare
void scope_check_var_decls(scope_node* cur_scope, var_decls_t vars_decls);

//Checks Validity of procedure declarations - If Already Declared, Error - If Not, Declare
void scope_check_proc_decls(linked_list* scope_list, proc_decls_t procs_decls);

//Checks Validity of All Statements in stmts_t stmts - Throws Error if given improper stmt_kind
void scope_check_statements(linked_list* scope_list, stmts_t stmts);

//Checks Validity of assign statement stmt
void scope_check_assign_stmt(scope_node* cur_scope, assign_stmt_t stmt);

//Checks Validity of call statement stmt
void scope_check_call_stmt(scope_node* cur_scope, call_stmt_t stmt);

//Checks Validity of if statement stmt
void scope_check_if_stmt(linked_list* scope_list, if_stmt_t stmt);

//Checks Validity of while statement stmt
void scope_check_while_stmt(linked_list* scope_list, while_stmt_t stmt);

//Checks Validity of read statement stmt
void scope_check_read_stmt(scope_node* cur_scope, read_stmt_t stmt);

//Checks Validity of print statement stmt
void scope_check_print_stmt(scope_node* cur_scope, print_stmt_t stmt);

//Checks Validity of condition_t stmt based on all possible condition types - Throws Error is improper cond_kind given
void scope_check_condition(scope_node* cur_scope, condition_t stmt);

//Checks Validity of divisible condition db_cond
void scope_check_db_condition(scope_node* cur_scope, db_condition_t db_cond);

//Checks Validity of relative operator condition rel_cond
void scope_check_rel_condition(scope_node* cur_scope, rel_op_condition_t rel_cond);

//Checks Validity of expr_t* expr based on each possible expression type - Throws Error if improper expr_kind given
void scope_check_expr(scope_node* cur_scope, expr_t* expr);

//Check Validity of binary operator expression bin_expr
void scope_check_bin_expr(scope_node* cur_scope, binary_op_expr_t bin_expr);

//Check Validity of negated expression neg_expr
void scope_check_neg_expr(scope_node* cur_scope, negated_expr_t neg_expr);

//Determine if ident matches and ident_node*s present in cur_scope and upwards (all nested scopes)
void scope_check_in_scope_vis(scope_node* cur_scope, ident_t ident);

//Enters a new scope via allocating and adding a new scope_node* to the end of the linked list
void scope_check_enter_scope(linked_list* list);

//Leaves the current scope via freeing and removing the end scope_node* of the linked list
void scope_check_leave_scope(linked_list* list);

//Checks if ident is present in cur_scope identifiers already - Adds if not present, error if present
void scope_check_declare_ident(scope_node* cur_scope, ident_t* ident, char* type);

//Search & return ident_node* found in cur_scope that matches ident - Returns NULL if not found
ident_node* scope_check_in_scope_decl(scope_node* cur_scope, ident_t* ident);

//Free the ident_node* idents list from scope_node* node
void scope_check_free_ident_list(scope_node* node);