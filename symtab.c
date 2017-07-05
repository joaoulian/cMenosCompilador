#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
int hash (char * key)
{
    int temp = 0;
    int i = 0;
    while (key[i] != '\0')
    {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
        ++i;
    }
    return temp;
}

/* the list of line numbers of the source
 * code in which a variable is referenced
 */
typedef struct LineListRec
{
    int lineno;
    struct LineListRec * next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name,
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{
    char * name;
    char * name2;
    LineList lines;
    int memloc ;
    char * escopo;
    TipType tipoId;
    ExpType tipo;
    struct BucketListRec * next;
} * BucketList;

/* the hash table */
BucketList hashTable[SIZE];

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert_first( char * name, char * name2, int lineno, int loc, char * escopo , ExpType tipo, TipType tipoId)
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->name2 = name2;
    l->escopo = escopo;
    l->tipo=tipo;
    l->tipoId=tipoId;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l;
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, int lineno)
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
}

/* Function st_lookup returns the memory
 * location of a variable or -1 if not found
 */
int st_lookup ( char * name )
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->memloc;
}


ExpType st_lookupTipo ( char * name )
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->tipo;
}

/* Function st_lookup returns the tipo
 * location of a variable or -1 if not found
 */
TipType st_lookupTipoId ( char * name )
{
    int h = hash(name);
    BucketList l =  hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
    if (l == NULL) return -1;
    else return l->tipoId;
}

int buscaMemoria(char *name){
  int i;
  for (i = 0; i < SIZE; i++){
    if (hashTable[i] != NULL){
      BucketList l = hashTable[i];
      while (l != NULL){
        if (strcmp(name,l->name2) == 0){
          return l->memloc;
        }
        l = l->next;
      }
    }
  }
  return -1;
}

int buscaMemoriaComEscopo(char *name, char *escopo){
  int i;
  for (i = 0; i < SIZE; i++){
    if (hashTable[i] != NULL){
      BucketList l = hashTable[i];
      while (l != NULL){
        if ((strcmp(name,l->name2) == 0) && (strcmp(escopo, l->escopo) == 0)){
          return l->memloc;
        }
        else if (strcmp(name, l->name2) == 0 && (strcmp(l->escopo, "programa") == 0)){
          return l->memloc;
        }
        l = l->next;
      }
    }
  }
  return -1;
}

int buscaTipoComEscopo(char *name, char *escopo){
  int i;
  for (i = 0; i < SIZE; i++){
    if (hashTable[i] != NULL){
      BucketList l = hashTable[i];
      while (l != NULL){
        if ((strcmp(name,l->name2) == 0) && strcmp(escopo, l->escopo) == 0){
          return l->tipoId;
        }
        l = l->next;
      }
    }
  }
  return -1;
}

void printSymTab(FILE * listing)
{ int i;
    fprintf(listing," Nome   Memoria  Tipo     Declaração     Escopo      Numero linha\n");
    fprintf(listing,"------- ------- -------   ---------    ----------   --------------\n");
    for (i=0; i<SIZE; ++i)
    {if (hashTable[i] != NULL)
        { BucketList l = hashTable[i];
            while (l != NULL)
            {   LineList t = l->lines;
                fprintf(listing,"%-8s ",l->name2);
                fprintf(listing, "%3d     ",l->memloc);
                if(l->tipo==Integer)
                {
                    fprintf(listing,"INT      ");
                }
                else if(l->tipo==Void)
                {
                    fprintf(listing,"VOID     ");
                }
                else if(l->tipo==Boolean)
                {
                    fprintf(listing,"BOOL     ");
                }
                if(l->tipoId==Funcao)
                {
                    fprintf(listing,"Funcao    ");
                }
                else if(l->tipoId==Variavel)
                {
                    fprintf(listing,"Variavel  ");
                }
                else if(l->tipoId==Vetor)
                {
                    fprintf(listing,"Vetor     ");
                }
                fprintf(listing,"     %-10s ",l->escopo);
                while (t != NULL)
                {
                    fprintf(listing,"%4d ",t->lineno);
                    t = t->next;
                }
                fprintf(listing,"\n");
                l = l->next;
            }
        }
    }
}
