#include <stdio.h>
#include <stdlib.h>
#include "parser.h"//not sure if this is needed here, but leaving it in case
#include "id_attrs.h"
#include "ast.h"
#include "symtab.h"
#include "scope_check.h"
#include "utilities.h"
#include "id_use.h"
#include <string.h>


//Initialize Symbol Table
void symtab_initialize() {

	SymbolTable sym_table = create_symbol_table();
	
}

// Top of the scope stack
Scope *current_scope = NULL;

// Hash function for storing identifiers in symbol tables
unsigned int hash(const char *name) {
    unsigned int hash_val = 0;
    for (const char *p = name; *p != '\0'; p++) {
        hash_val = (hash_val * 31) + *p;
    }
    return hash_val % HASH_SIZE;
}

// Function to create a new symbol table
SymbolTable *create_symbol_table() {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (table) {
        for (int i = 0; i < HASH_SIZE; i++) {
            table->table[i] = NULL;
        }
    }
    return table;
}

// Function to create and enter a new scope
void enter_scope() {
    Scope *new_scope = (Scope *)malloc(sizeof(Scope));
    new_scope->symbols = create_symbol_table();
    new_scope->next = current_scope;
    current_scope = new_scope;
}

// Function to exit the current scope
void exit_scope() {
    if (current_scope == NULL) {
        return;  // No scope to exit
    }

    // Free all symbols in the current scope's symbol table
    for (int i = 0; i < HASH_SIZE; i++) {
        Symbol *symbol = current_scope->symbols->table[i];
        while (symbol) {
            Symbol *temp = symbol;
            symbol = symbol->next;
            free(temp->name);
            free(temp);
        }
    }

    // Free the symbol table and pop the scope
    free(current_scope->symbols);
    Scope *old_scope = current_scope;
    current_scope = current_scope->next;
    free(old_scope);
}

// Function to declare a new identifier in the current scope
void declare_identifier(const char *name) {
    if (current_scope == NULL) {
        printf("Error: No scope available for declaration.\n");
        return;
    }

    unsigned int index = hash(name);
    Symbol *symbol = current_scope->symbols->table[index];

    // Check if identifier already exists in the current scope
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            printf("Error: Redeclaration of '%s' in the same scope.\n", name);
            return;
        }
        symbol = symbol->next;
    }

    // Add new symbol to the current scope's symbol table
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    strcpy(new_symbol->name, name);
    new_symbol->next = current_scope->symbols->table[index];
    current_scope->symbols->table[index] = new_symbol;
}

// Function to check if an identifier is declared in any visible scope
void use_identifier(const char *name) {
    Scope *scope = current_scope;

    // Traverse scope stack to find the identifier in visible scopes
    while (scope) {
        unsigned int index = hash(name);
        Symbol *symbol = scope->symbols->table[index];

        // Check if identifier is found in the current scope
        while (symbol) {
            if (strcmp(symbol->name, name) == 0) {
                return;  // Identifier found, no error
            }
            symbol = symbol->next;
        }
        scope = scope->next;  // Move to the next outer scope
    }

    printf("Error: Undeclared identifier '%s' used.\n", name);
}
