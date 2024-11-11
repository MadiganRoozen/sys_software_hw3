#ifndef SYMTAB_H
#define SYMTAB_H


/*
 * SymbolAttributes - stores the attributes for each symbol in a scope.
 * Attributes include the symbol's name, type, scope level, index, value, and a mark for auxiliary uses.
 */

typedef struct {
    char *name;   // The name of the symbol
    char *type;   // The type of the symbol (e.g., int, float)
    int level;    // Scope level (depth) of the symbol in nested blocks
    int index;    // Stack index of the scope where the symbol was declared
    int value;    // Value associated with the symbol, if applicable
    int mark;     // if the variable is allowed to be used within the scope
} SymbolAttributes;

typedef struct SymbolTable SymbolTable;

void free_symtab(SymbolTable * symtab);

// Function to push a new scope onto the symbol table's scope stack
void push_scope(SymbolTable *symtab);

// Function to pop the top scope from the symbol table's scope stack
void pop_scope(SymbolTable *symtab);

// Function to declare a symbol in the current scope with specific attributes
void declare_symbol(SymbolTable *symtab, const char *name, const char *type, int value);

// Function to look up a symbol by name, starting from the current scope and moving outward
SymbolAttributes *lookup_symbol(SymbolTable *symtab, const char *name);

#endif // SYMTAB_H