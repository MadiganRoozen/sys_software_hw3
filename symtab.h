#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "utilities.h"
#include "id_use.h"

#define HASH_SIZE 256  // Size of the hash table for each scope

//struct for the symbol table. SymbolTable should be a stack of Scope objects from the scope files
// Structure for an individual symbol
typedef struct Symbol {
    char *name;
    struct Symbol *next;  // For chaining in hash table buckets
} Symbol;

// Structure for a symbol table (one per scope)
typedef struct SymbolTable {
    Symbol *table[HASH_SIZE];
} SymbolTable;

// Structure for the scope stack
typedef struct Scope {
    SymbolTable *symbols;
    struct Scope *next;   // Pointer to the previous scope in the stack
} Scope;



//initialize the symbol table
void symtab_initialize();

unsigned int hash(const char *name);

SymbolTable *create_symbol_table();

void enter_scope();

void exit_scope();

void declare_identifier(const char *name);

void use_identifier(const char *name);


