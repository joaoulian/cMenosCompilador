#ifndef _SYMTAB_H_
#define _SYMTAB_H_
#include "globals.h"

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert_first( char * name, char * name2, int lineno, int loc, char * escopo , ExpType tipo, TipType tipoId);
void st_insert( char * name, int lineno);
char* st_lookupEscopo ( char * name );
ExpType st_lookupTipo ( char * name );
TipType st_lookupTipoId ( char * name );

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name );

/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE * listing);


#endif
