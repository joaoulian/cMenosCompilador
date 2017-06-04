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

funCel * buscaFilaFunByHash(funFila *oi, int hashValue){
  funCel *temp;
  temp = malloc(sizeof(funCel));
  *temp = *oi->inicio;
  while(temp != NULL){
    if (temp->hash == hashValue){
      return temp;
    }
    temp = temp->prox;
  }
  return NULL;
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
int listaLabel[30];

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
  if (posMain > 0){
    contLinha++;
  }
  posTemporario1 = 2;
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
  if (posMain > 0){
    funcao = buscaFilaFunByHash(fun, posMain);
    fprintf(listing, "memoriaDeInstrucoes[0] = ");
    fprintf(listing, "j %d\n", funcao->posicaoInicio);
  }
}

void converteParaMaquina(cel *temp){
  if (strcmp(temp->nome,"lab") == 0){
    if (temp->op1Flag == 2){
      listaLabel[temp->op1Num] = contLinha;
    }
    else {
      if (fun->fim == NULL){
        funcao->hash = temp->op1Num;
        funcao->posicaoInicio = contLinha;
        funcao->prox = NULL;
        funcao->posicaoFim = 0;
        insereFilaFun(fun, *funcao);
      }
      else {
        fprintf(listing, "\n");
        fun->fim->posicaoFim = contLinha;
        contLinha++;
        funcao->hash = temp->op1Num;
        funcao->posicaoInicio = contLinha;
        funcao->prox = NULL;
        insereFilaFun(fun, *funcao);
      }
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
    int hashValue;
    int posParam = temp->op1Num;
    paramCel *aux = malloc(sizeof(paramCel));
    funCel *tempFun = malloc(sizeof(funCel));
    *aux = *par->inicio;
    for (i = 0; i < temp->op2Num; i++){
      if (aux->tipo == 1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "lw $s%d, %d\n", reg, aux->valor);
        posParam++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "sw $s%d, %d\n", reg, posParam);
        aux = aux->prox;
      }
    }
    tempFun = buscaFilaFunByHash(fun, temp->op1Num);
    if (tempFun->hash > -1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "j %d\n", tempFun->posicaoInicio);
      contLinha++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", tempFun->posicaoFim);
      fprintf(listing, "j %d\n", contLinha-1);
      contLinha++;
    }
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (temp->op2Flag == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else if (temp->op2Flag == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else if (temp->op2Flag == 3){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
  }
  else if ((strcmp(temp->nome, "eq") == 0)){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    if (temp->op1Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
    }
    else if (temp->op1Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
    }
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    if (temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
    }
    else if (temp->op2Flag == 1){
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
    }
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    fprintf(listing, "eq $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
  }
  else if ((strcmp(temp->nome, "if_f") == 0)){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    fprintf(listing, "li $s0, 1\n");
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    fprintf(listing, "beq $s%d, $s0, %d\n", reg, temp->op2Num);
  }
  else if ((strcmp(temp->nome, "sum") == 0)
          || (strcmp(temp->nome, "sub") == 0)
          || (strcmp(temp->nome, "div") == 0)
          || (strcmp(temp->nome, "mul") == 0)){
    reg = 0;
    if (temp->op1Flag == 0 && temp->op2Flag == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, posTemporario1, reg-1, reg-2);
      if (temp->temp > 0){
        if (posTemporario1 == 2){
          posTemporario1 = reg + 1;
        }
      }
    }
    else if (temp->op1Flag == 0 && temp->op2Flag  == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, posTemporario1, reg-1, reg-2);
      if (temp->temp > 0){
        if (posTemporario1 == 2){
          posTemporario1 = reg + 1;
        }
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag  == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, posTemporario1, reg-1, reg-2);
      if (temp->temp > 0){
        if (posTemporario1 == 2){
          posTemporario1 = reg + 1;
        }
      }
    }
    else if (temp->op1Flag == 1 && temp->op2Flag == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, posTemporario1, reg-1, reg-2);
      if (temp->temp > 0){
        if (posTemporario1 == 2){
          posTemporario1 = reg + 1;
        }
      }
    }
    else if (temp->op1Flag == 3 && temp->op2Flag == 3){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, posTemporario1-1);
      posTemporario1 = 2;
    }
  }
}
