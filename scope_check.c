/* $Id: scope_check.c,v 1.20 2023/11/13 14:10:00 leavens Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"
#include "scope_check.h"
#include "id_attrs.h"
#include "id_use.h"

// Build the symbol table for prog
// and check for duplicate declarations
// or uses of undeclared identifiers
// Return the modified AST with id_use pointers
block_t scope_check_program(block_t prog)
{
    
    scope_check_block(prog);

    symtab_leave_scope();
    
    return prog;
}

void scope_check_block(block_t prog) {

	//Entering New Scope
	symtab_enter_scope();

	//Checking Declarations
	scope_check_constDecls(prog.const_decls);
	scope_check_varDecls(prog.var_decls);
	scope_check_procDecls(prog.proc_decls);

	//Checking Statements
	scope_check_stmts(prog.stmts);

}

void scope_check_constDecls(const_decls_t consts) {

	const_decl_t *cdp = consts.start;
	id_kind idk = constant_idk;

	while (cdp != NULL) {

		//Checking Idents List for Each Declaration
		const_def_t* c_idents = cdp.const_def_list.start;

		while (c_idents != NULL) {
			scope_check_declare_ident(c_idents.ident, idk);
			c_idents = c_idents->next;
		}

		cdp = cdp->next;
	}
}

void scope_check_procDecls(proc_decls_t procs) {

	proc_decl_t *pdp = procs.proc_decls;
	id_kind idk = procedure_idk;

	while (pdp != NULL) {
		ident_t p_ident = malloc(sizeof(ident_t));
		p_ident.name = pdp.name;
		p_ident.file_loc = file_loc;

		scope_check_declare_ident(p_ident, idk);

		scope_check_block(pdp.block);
	}
}

// build the symbol table and check the declarations in vds
void scope_check_varDecls(var_decls_t vds)
{
    var_decl_t *vdp = vds.var_decls;
    id_kind idk = variable_idk;

    while (vdp != NULL) {
		
		//Checking Idents List for Each Declaration
    	scope_check_idents(vd.ident_list, idk);
		vdp = vdp->next;

    }
}

// // Add declarations for the names in vd,
// // reporting duplicate declarations
// void scope_check_varDecl(var_decl_t vd)
// {
// 	id_kind idk = variable_idk;
//     scope_check_idents(vd.ident_list, idk);
// }

// Add declarations for the names in ids
// to current scope as type t
// reporting any duplicate declarations
void scope_check_idents(ident_list_t ids, id_kind t)
{
    ident_t *idp = ids.start;

    while (idp != NULL) {

		scope_check_declare_ident(*idp, t);
		idp = idp->next;
    }
}

// Add declaration for id
// to current scope as type t
// reporting if it's a duplicate declaration
void scope_check_declare_ident(ident_t id, id_kind t)
{
    if (symtab_declared_in_current_scope(id.name)) {
        // only variables in FLOAT
		bail_with_prog_error(*(id.file_loc), "Variable \"%s\" has already been declared!", id.name);
    } 
    else {
		int ofst_cnt = symtab_scope_loc_count();
		id_attrs *attrs = create_id_attrs(*(id.file_loc), t, ofst_cnt);
		symtab_insert(id.name, attrs);
    }
}

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
stmts_t scope_check_stmts(stmts_t stmts)
{
    stmt_t *sp = stmts.stmt_list.start;
    while (sp != NULL) {
		*sp = scope_check_stmt(*sp);
		sp = sp->next;
    }
    return stmts;
}

// check the statement to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
stmt_t scope_check_stmt(stmt_t stmt)
{
    switch (stmt.stmt_kind) {

    	//Assign Statement
    	case assign_stmt:
			scope_check_assignStmt(stmt.data.assign_stmt);
			break;

		//Call Statement
		case call_stmt:

			break;

    // case begin_stmt:
	// stmt.data.begin_stmt
	//     = scope_check_beginStmt(stmt.data.begin_stmt);
	// break;

		//If Statement
	    case if_stmt:
			stmt.data.if_stmt = scope_check_ifStmt(stmt.data.if_stmt);
			break;

		//While Statement
		case while_stmt:

			break;

		//Read Statement
    	case read_stmt:
			stmt.data.read_stmt = scope_check_readStmt(stmt.data.read_stmt);
			break;

		//Print Statement
    	case print_stmt:
			stmt.data.print_stmt = scope_check_writeStmt(stmt.data.print_stmt);
			break;

		//Block Statement
		case block_stmt:

			break;

		//Improper Statement
    	default:
			bail_with_error("Call to scope_check_stmt with an AST that is not a statement!");
			break;
    }
    return stmt;
}

// check the statement for
// undeclared identifiers
// Return the modified AST with id_use pointers
void scope_check_assignStmt(assign_stmt_t stmt)
{
    const char *name = stmt.name;
    id_use* used = scope_check_ident_declared(*(stmt.file_loc), name);
    assert(used != NULL);  // since would bail if not declared
    *stmt.expr = scope_check_expr(*(stmt.expr));
    return stmt;
}

// check the statement for
// duplicate declarations and for
// undeclared identifiers
// Return the modified AST with id_use pointers
// begin_stmt_t scope_check_beginStmt(begin_stmt_t stmt)
// {
//     symtab_enter_scope();
//     scope_check_varDecls(stmt.var_decls);
//     stmt.stmts
// 	= scope_check_stmts(stmt.stmts);
//     symtab_leave_scope();
//     return stmt;
// }

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
if_stmt_t scope_check_ifStmt(if_stmt_t stmt)
{
    stmt.condition = scope_check_ifCondition(stmt.condition);
    *(stmt.then_stmts) = scope_check_stmts(*(stmt.then_stmts));
    *(stmt.else_stmts) = scope_check_stmts(*(stmt.else_stmts));
    return stmt;
}

condition_t scope_check_ifCondition(condition_t if_cond) 
{
	switch(if_cond.cond_kind) {
		case ck_db:
			if_cond.data.db_cond.dividend = scope_check_expr(if_cond.data.db_cond.dividend);
			if_cond.data.db_cond.divisor = scope_check_expr(if_cond.data.db_cond.divisor);
			break;

		case ck_rel:
			if_cond.data.rel_op_cond.expr1 = scope_check_expr(if_cond.data.rel_op_cond.expr1);
			if_cond.data.rel_op_cond.expr2 = scope_check_expr(if_cond.data.rel_op_cond.expr2);
			break;

	}


}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
read_stmt_t scope_check_readStmt(read_stmt_t stmt)
{
    id_use* used = scope_check_ident_declared(*(stmt.file_loc), stmt.name);
    return stmt;
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
print_stmt_t scope_check_writeStmt(print_stmt_t stmt)
{
    stmt.expr = scope_check_expr(stmt.expr);
    return stmt;
}

// check the expresion to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
expr_t scope_check_expr(expr_t exp)
{
    switch (exp.expr_kind) {
    case expr_bin:
		exp.data.binary = scope_check_binary_op_expr(exp.data.binary);
		break;
    case expr_ident:
		exp.data.ident = scope_check_ident_expr(exp.data.ident);
		break;
    case expr_number:
		// no identifiers are possible in this case, so just return
		break;
    case expr_negated:
		*(exp.data.negated.expr) = scope_check_expr(*(exp.data.negated.expr));
		break;
    default:
		bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr", exp.expr_kind);
		break;
    }
    return exp;
}

// check that all identifiers used in exp
// have been declared
// (if not, then produce an error)
// Return the modified AST with id_use pointers
binary_op_expr_t scope_check_binary_op_expr(binary_op_expr_t exp)
{
    *(exp.expr1) = scope_check_expr(*(exp.expr1));
    // (note: no identifiers can occur in the operator)
    *(exp.expr2) = scope_check_expr(*(exp.expr2));
    return exp;
}

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
// Return the modified AST with id_use pointers
ident_t scope_check_ident_expr(ident_t id)
{
    id_use* used = scope_check_ident_declared(*(id.file_loc), id.name);
    return id;
}

// check that name has been declared,
// if so, then return an id_use for it
// otherwise, produce an error 
id_use *scope_check_ident_declared(file_location floc, const char *name)
{
    id_use *ret = symtab_lookup(name);
    if (ret == NULL) {
		bail_with_prog_error(floc, "identifier \"%s\" is not declared!", name);
    }
    assert(symtab_lookup(name) != NULL);
    return ret;
}