#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>

int reg;
int posTemporario1;

typedef struct funCel {
  char * nome;
  int posicaoInicio;
  int posicaoFim;
  struct cel *prox;
} funCel;

typedef struct funFila {
  cel * inicio;
  cel * fim;
} funFila;

void inicializaFilaFun(funFila *f) {
  f->inicio = NULL;
  f->fim = NULL;
}

void insereFilaFun(funFila *f, funCel c) {
  cel *novo;
  novo = malloc(sizeof(cel));
  *novo = c;
  if (f->fim == NULL){
    f->fim = novo;
    f->inicio = novo;
  }
  else {
    f->fim->prox = novo;
    f->fim = novo;
  }
}

funFila *f;
funCel *funcao;

void percorreLista(){
  inicializaFilaFun(f);
  cel *temp;
  reg = 0;
  fprintf(listing, "\n");
  posTemporario1 = 0;
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
  if (strcmp(temp->nome,"lab") == 0){
    if (f->fim == NULL){
      strcpy(funcao->nome, temp->nome);
      funcao->posicaoInicio = temp->op1Num;
      funcao->prox = NULL;
      funcao->posicaoFim = 0;
      insereFilaFun(f, funcao);
    }
    else {
      f->fim->posicaoFim = temp->op1Num-1;
      strcpy(funcao->nome, temp->nome);
      funcao->posicaoInicio = temp->op1Num;
      funcao->prox = NULL;
      insereFilaFun(f, funcao);
    }
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (reg < 3) reg++;
    else reg = 0;
    if (temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else if (temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else if (temp->op2Flag == 3){
      fprintf(listing, "sw $s%d, %d\n", posTemporario1, temp->op1Num);
    }
  }
  else if ((strcmp(temp->nome, "sum") == 0)
          || (strcmp(temp->nome, "sub") == 0)
          || (strcmp(temp->nome, "div") == 0)
          || (strcmp(temp->nome, "mul") == 0)){
    reg = 0;
    if (temp->op1Flag == 0 && temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
      }
    }
    else if (temp->op1Flag == 0 && temp->op2Flag  == 1){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag  == 0){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
      }
    }
    else if (temp->op1Flag == 3 && temp->op2Flag == 3){
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, posTemporario1-1);
      posTemporario1 = 0;
    }
  }
}
