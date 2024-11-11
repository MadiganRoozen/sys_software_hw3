#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symtab.h"

#define INITIAL_STACK_CAPACITY 8
#define HASH_TABLE_SIZE 64

//Each node contains the list of attributes for an entry
typedef struct SymbolNode {
    SymbolAttributes attributes;
    struct SymbolNode * next
} SymbolNode;

//HashTable per each scope
typedef struct HashTable {
    SymbolNode **table;  // Array of pointers to SymbolNode linked lists (one per bucket)
    int size;            // Number of buckets in the hash table
} HashTable;

// Hash table structure for each scope - dynamically grow the capacity as needed
typedef struct SymbolTable {
    HashTable **scopes;
    int scopes_count;
    int scope_cap;
};

// Create a new hash table for a single scope
HashTable *create_hash_table(){
    HashTable * ht = malloc(sizeof(HashTable));
    ht->size = HASH_TABLE_SIZE;
    ht->table = calloc(ht->size, sizeof(SymbolNode *)); //init buckets to NULL
    return ht;
}

// free a hash table and its nested attributes
void free_hash_table(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        SymbolNode *node = ht->table[i];
        while (node) {
            SymbolNode *temp = node;
            node = node->next;
            free(temp->attributes.name);
            free(temp->attributes.type);
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}


// Hash function to get index NOT SURE IF NEEDED

//----------------SymbolTable Functions--------------------//

//create a symbol table
SymbolTable * create_symbol_table() {
    SymbolTable *symtab = malloc(sizeof(SymbolTable));
    symtab->scope_cap = INITIAL_STACK_CAPACITY;
    symtab->scopes_count = 0;
    symtab->scopes = malloc(symtab->scope_cap * sizeof(HashTable *));
    return symtab;
}

//free the symbol table
void free_symbol_table(SymbolTable *symtab) {
    for (int i = 0; i < symtab->scopes_count; i++) {
        free_hash_table(symtab->scopes[i]);
    }
    free(symtab->scopes);
    free(symtab);
}


//create a new scope on the stack of scopes in the symbol table
void push_scope(SymbolTable *symtab){
    //reallocate new space by double
    if(symtab->scopes_count == symtab->scope_cap){
        symtab->scope_cap *= 2;
        symtab->scopes = realloc(symtab->scopes, symtab->scope_cap * sizeof(HashTable *));
    }
    //hashtable[+1] = new hash table
    symtab->scopes[symtab->scopes_count++] = create_hash_table();
}

// pop the top scope
void pop_scope(SymbolTable * symtab){
    //while theres at least one scope, free the top scope
    if(symtab->scopes_count > 0){
        free_hash_table(symtab->scopes[--symtab->scopes_count]);
    }
}

//------------Symbol functions-----------------//

unsigned int hash(const char *name, int table_size) {
    unsigned int hash = 0;
    while (*name) {
        hash = (hash << 5) + *name++;  // Simple hashing with left shift and addition
    }
    return hash % table_size;  // Ensures the hash index is within the bounds of the table size
}

void declare_symbol(SymbolTable *symtab, const char * name, const char * type, int value){
    if(symtab->scopes_count) return; //no scopes available

    HashTable * cur_scope = symtab->scopes[symtab->scopes_count - 1]; // set current to the top of the scope
    unsigned int hash_index = hash(name, cur_scope->size);
    //Create a new symbol node
    SymbolNode *node = malloc(sizeof(SymbolNode));
    node->attributes.name = strdup(name);
    node->attributes.type = strdup(type);
    node->attributes.level = symtab->scopes_count - 1;
    node->attributes.index = 0;
    node->attributes.value = value;
    node->next = cur_scope->table[hash_index];
}

//attempt to look for if the symbols exist
SymbolAttributes *lookup_symbol(SymbolTable * symtab, const char *name) {

    //reverse order declaration to make sure it finds the closest declaration
    for(int i = symtab->scopes_count - 1; i >= 0; i--) {

        HashTable *scope = symtab->scopes[i];
        unsigned int hash_index = hash(name, scope->size);
        SymbolNode *node = scope->table[hash_index];

        //traverse the linkedlist within the scope 
        while (node) {
            if (strcmp(node->attributes.name, name) == 0) {
                return &node->attributes;
            }
            node = node->next;
        }
    }
    return NULL;  // Symbol not found
}
    
