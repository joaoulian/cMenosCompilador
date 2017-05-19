#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>

void percorreLista(){
  cel *temp;
  temp = malloc(sizeof(cel));
  *temp = *f->inicio;
  while (temp != NULL){
    fprintf(listing, "%s\n", temp->nome);
    if (temp->prox == f->fim->prox){
      temp = NULL;
    }
    else {
      *temp = *temp->prox;
    }
  }
}

void converteParaMaquina(cel *temp){
  
}
