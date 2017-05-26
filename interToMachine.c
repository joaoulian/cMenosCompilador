#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include "interToMachine.h"
#include <stdio.h>
#include <stdlib.h>

int reg;
int posTemporario1;
int contLinha;

void inicializaFilaFun(funFila *f) {
  f->inicio = NULL;
  f->fim = NULL;
}

void insereFilaFun(funFila *f, funCel c) {
  funCel *novo;
  novo = malloc(sizeof(funCel));
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

void inicializaFilaParam(paramFila *f) {
  f->inicio = NULL;
  f->fim = NULL;
}

void insereFilaParam(paramFila *f, paramCel c) {
  paramCel *novo;
  novo = malloc(sizeof(paramCel));
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

funCel *funcao;
paramCel *param;

void percorreLista(){
  cel *temp;
  contLinha = 0;
  fun = malloc(sizeof(funFila));
  par = malloc(sizeof(paramFila));
  funcao = malloc(sizeof(funCel));
  param = malloc(sizeof(paramCel));
  inicializaFilaFun(fun);
  inicializaFilaParam(par);
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
    if (fun->fim == NULL){
      fprintf(listing, "\n");
      funcao->hash = temp->op1Num;
      funcao->posicaoInicio = 0;
      funcao->prox = NULL;
      funcao->posicaoFim = 0;
      //insereFilaFun(fun, *funcao);
    }
    else {
      fun->fim->posicaoFim = contLinha;
      funcao->hash = temp->op1Num;
      funcao->posicaoInicio = contLinha+1;;
      funcao->prox = NULL;
      //insereFilaFun(fun, *funcao);
    }
  }
  else if (strcmp(temp->nome, "par") == 0){
    if (temp->op1Flag == 1){
      param->tipo = 1;
      param->valor = temp->op1Num;
      param->prox = NULL;
      insereFilaParam(par, *param);
    }
  }
  else if (strcmp(temp->nome, "cal") == 0){
    reg = 0;
    int i;
    int posParam = temp->op1Num;
    paramCel *aux = malloc(sizeof(paramCel));
    *aux = *par->inicio;
    for (i = 0; i < temp->op2Num; i++){
      if (aux->tipo == 1){
        fprintf(listing, "lw $s%d, %d\n", reg, aux->valor);
        posParam++;
        fprintf(listing, "sw $s%d, %d\n", reg, posParam);
        aux = aux->prox;
      }
    }
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (reg < 3) reg++;
    else reg = 0;
    if (temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
      contLinha = contLinha + 2;
    }
    else if (temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
      contLinha = contLinha + 2;
    }
    else if (temp->op2Flag == 3){
      fprintf(listing, "sw $s%d, %d\n", posTemporario1, temp->op1Num);
      contLinha++;
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
      contLinha = contLinha + 3;
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 0 && temp->op2Flag  == 1){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      contLinha = contLinha + 3;
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag  == 0){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      contLinha = contLinha + 3;
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      contLinha = contLinha + 3;
      if (temp->temp > 0){
        reg++;
        if (posTemporario1 == 3){
          posTemporario1 = reg + 1;
        }
        else {
          posTemporario1 = reg;
        }
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 3 && temp->op2Flag == 3){
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, posTemporario1-1);
      contLinha++;
      posTemporario1 = 0;
    }
  }
}
