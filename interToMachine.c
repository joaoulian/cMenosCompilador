#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>

int reg;
int posTemporario1;
int posTemporario2;

void percorreLista(){
  cel *temp;
  reg = 0;
  posTemporario1 = 0;
  posTemporario2 = 0;
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
  if (strcmp(temp->nome,"asg") == 0){
    if (reg < 3) reg++;
    else reg = 0;
    if (temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else if (temp->op2Flag == 1){

    }
    else if (temp->op2Flag == 3){
      fprintf(listing, "sw $s%d, %d\n", posTemporario1, temp->op1Num);
      posTemporario1 = 0;
    }
  }
  else if ((strcmp(temp->nome, "sum") == 0)
          || (strcmp(temp->nome, "sub") == 0)
          || (strcmp(temp->nome, "div") == 0)
          || (strcmp(temp->nome, "mul") == 0)){
    reg = 0;
    if (temp->op1Flag == 1 && temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        reg++;
        fprintf(listing, "move $s%d, $s%d\n", reg, reg-1);
        posTemporario1 = reg;
      }
    }
    else if (temp->op1Flag == 3 && temp->op2Flag == 3){

    }
    reg = 0;
  }
}
