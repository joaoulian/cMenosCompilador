#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>

int reg;

void percorreLista(){
  cel *temp;
  reg = 0;
  temp = malloc(sizeof(cel));
  *temp = *f->inicio;
  while (temp != NULL){
    converteParaMaquina(temp);
    if (temp->prox == f->fim->prox){
      temp = NULL;
    }
    else {
      *temp = *temp->prox;
    }
  }
}

void converteParaMaquina(cel *temp){
  /*BEGIN ATRIBUICAO*/
  if (strcmp(temp->nome,"asg") == 0){
    if (temp->op2Flag == 0){
      reg++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
  }
  /*END ATRIBUICAO*/
}
