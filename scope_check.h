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
	char* ident;
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


void scope_check_program(block_t program_AST);

void scope_check_block(linked_list* scope_list, block_t* prog);

void scope_check_const_decls(scope_node* cur_scope, const_decls_t consts_decls);

void scope_check_var_decls(scope_node* cur_scope, var_decls_t vars_decls);

void scope_check_proc_decls(linked_list* scope_list, proc_decls_t procs_decls);

void scope_check_statements(linked_list* scope_list, stmts_t stmts);

void scope_check_assign_stmt(scope_node* cur_scope, assign_stmt_t stmt);

void scope_check_call_stmt(scope_node* cur_scope, call_stmt_t stmt);

void scope_check_if_stmt(linked_list* scope_list, if_stmt_t stmt);

void scope_check_while_stmt(linked_list* scope_list, while_stmt_t stmt);

void scope_check_read_stmt(scope_node* cur_scope, read_stmt_t stmt);

void scope_check_print_stmt(scope_node* cur_scope, print_stmt_t stmt);

void scope_check_condition(scope_node* cur_scope, condition_t stmt);

void scope_check_db_condition(scope_node* cur_scope, db_condition_t db_cond);

void scope_check_rel_condition(scope_node* cur_scope, rel_op_condition_t rel_cond);

void scope_check_expr(scope_node* cur_scope, expr_t* expr);

void scope_check_bin_expr(scope_node* cur_scope, binary_op_expr_t bin_expr);

void scope_check_neg_expr(scope_node* cur_scope, negated_expr_t neg_expr);

void scope_check_in_scope_vis(scope_node* cur_scope, ident_t ident);

void scope_check_enter_scope(linked_list* list);

void scope_check_leave_scope(linked_list* list);

void scope_check_declare_ident(scope_node* cur_scope, ident_t* ident);

bool scope_check_in_scope_decl(scope_node* cur_scope, ident_t* ident);

void scope_check_free_ident_list(scope_node* node);