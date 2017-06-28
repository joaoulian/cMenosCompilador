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
int loadRegister = 0;
int ultimaFuncao = -1;
int recebeRetorno = 0;

void mapeiaLabel(cel *temp);

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
int linhaLabel[30];
int linhaChamadaFuncao[200];
int variavelRetorno[200];
int indiceChamadaFuncao = 0;
int indicePilha = 0;

void percorreLista(){
  cel *temp;
  cel *temp2;
  contLinha = 0;
  fun = malloc(sizeof(funFila));
  par = malloc(sizeof(paramFila));
  funcao = malloc(sizeof(funCel));
  param = malloc(sizeof(paramCel));
  inicializaFilaFun(fun);
  inicializaFilaParam(par);
  reg = 0;
  fprintf(listing, "\n");
  posTemporario1 = 3;
  temp = malloc(sizeof(cel));
  temp2 = malloc(sizeof(cel));
  *temp2 = *f->inicio;
  if (posMain > 0) contLinha++;
  while (temp2 != NULL){
    mapeiaLabel(temp2);
    if (temp2->prox == f->fim->prox){
      temp2 = NULL;
    }
    else {
      *temp2 = *temp2->prox;
    }
  }
  contLinha = 0;
  if (posMain > 0){
    funcao = buscaFilaFunByHash(fun, posMain);
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    fprintf(listing, "j %d\n", funcao->posicaoInicio);
    contLinha++;
  }
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
  fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
  fprintf(listing, "hlt\n");
}

void mapeiaLabel(cel *temp){
  if (strcmp(temp->nome,"lab") == 0){
    if (temp->op1Flag == 2){
      linhaLabel[temp->op1Num] = contLinha;
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
        fun->fim->posicaoFim = contLinha;
        funcao->hash = temp->op1Num;
        contLinha = contLinha + 2;
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
      contLinha = contLinha + 2;
      insereFilaParam(par, *param);
    }
  }
  else if (strcmp(temp->nome, "cal") == 0){
    reg = 0;
    int i;
    int hashValue;
    int posParam = temp->op1Num;
    funCel *tempFun = malloc(sizeof(funCel));
    tempFun = buscaFilaFunByHash(fun, temp->op1Num);
    contLinha = contLinha + 2;
    if (tempFun->hash > -1){
      linhaChamadaFuncao[indiceChamadaFuncao] = contLinha+1;
      indiceChamadaFuncao++;
      contLinha = contLinha + 1;
    }
  }
  else if (strcmp(temp->nome, "got") == 0){
    contLinha++;
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (temp->op2Flag == 0 || temp->op2Flag == 1){
      contLinha = contLinha + 2;
    }
    else if (temp->op2Flag == 3){
      contLinha++;
    }
  }
  else if ((strcmp(temp->nome, "eq") == 0)){
    contLinha = contLinha + 3;
  }
  else if ((strcmp(temp->nome, "if_f") == 0)){
    contLinha = contLinha + 2;
  }
  else if ((strcmp(temp->nome, "sum") == 0)
          || (strcmp(temp->nome, "sub") == 0)
          || (strcmp(temp->nome, "div") == 0)
          || (strcmp(temp->nome, "mul") == 0)){
    if ((temp->op1Flag == 0 && temp->op2Flag == 0)
        || (temp->op1Flag == 0 && temp->op2Flag  == 1)
        || (temp->op1Flag == 1 && temp->op2Flag  == 0)
        || (temp->op1Flag == 1 && temp->op2Flag == 1)){
      if (temp->temp > 0){
        contLinha = contLinha + 4;
      }
      else {
        contLinha = contLinha + 3;
      }
    }
    else if (temp->op1Flag == 3 && temp->op2Flag == 3){
      contLinha++;
    }
  }
}

void converteParaMaquina(cel *temp){
  reg = 0;
  if (strcmp(temp->nome,"lab") == 0){
    if(temp->op1Flag != 2){
        if (ultimaFuncao == -1){
          ultimaFuncao = temp->op1Num;
        }
        else {
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "lw $s32, %d\n", ultimaFuncao);
          contLinha++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "jr $s32\n");
          indicePilha++;
          contLinha++;
          ultimaFuncao = temp->op1Num;
        }
        fprintf(listing, "\n");
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
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    fprintf(listing, "li $s32, %d\n", contLinha+3);
    contLinha++;
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    fprintf(listing, "sw $s32, %d\n", temp->op1Num);
    contLinha++;
    if (tempFun->hash > -1){
      /*fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "li $s31, %d\n", contLinha+2);*/
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "j %d\n", tempFun->posicaoInicio);
      contLinha++;
    }
    if (temp->temp > 0){
      recebeRetorno = 1;
    }
    else {
      recebeRetorno = 0;
    }

    inicializaFilaParam(par);
  }
  else if (strcmp(temp->nome, "par") == 0){
    if (temp->op1Flag == 1){
      param->tipo = 1;
      param->valor = temp->op1Num;
      param->prox = NULL;
      insereFilaParam(par, *param);
    }
  }
  else if (strcmp(temp->nome, "ret") == 0){
    if (temp->op1Flag == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "lw $s30, %d\n", temp->op1Num);
      contLinha++;
    }
    else if (temp->op1Flag == 3){
      if (recebeRetorno == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "move $s30, $s%d\n", posTemporario1-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "li $s30, %d\n", temp->op1Num);
      contLinha++;
    }
  }
  else if (strcmp(temp->nome, "got") == 0){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    fprintf(listing, "j %d\n", linhaLabel[temp->op1Num]);
    contLinha++;
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (temp->op1Flag == 5){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      contLinha++;
      reg++;
      if (temp->op2Flag == 1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
        contLinha++;
        if (temp->tempFlag == 0){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "sr $s%d, $s%d\n", posTemporario1, reg);
          contLinha++;
        }
        else if (temp->tempFlag == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "lw $s%d, %d\n", posTemporario1, temp->temp);
          contLinha++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "sr $s%d, $s%d\n", posTemporario1, reg);
          contLinha++;
        }
      }
    }
    else if (temp->op1Flag == 4){
      if (temp->tempFlag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "sw $s%d, %d\n", posTemporario1, temp->op1Num);
        contLinha++;
      }
      else if (temp->op2Flag == 4){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 3){
      if (temp->op2Flag == 5){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        if (temp->tempFlag == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "lw $s%d, %d\n", reg, temp->temp);
          contLinha++;
          reg++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
          contLinha++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "lr $s%d, $s%d\n", posTemporario1, reg);
          contLinha++;
        }
      }
    }
    if (temp->op1Flag == 4 && temp->op2Flag == 3){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg+1, posTemporario1-1, reg-1);
      reg++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "sr $s%d, $s%d\n", loadRegister, reg);
      contLinha++;
    }
    else if (temp->op1Flag == 4 && temp->op2Flag == 1 || temp->op2Flag == 4 && temp->op1Flag == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
    }
    else {
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
        if (recebeRetorno == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          contLinha++;
          fprintf(listing, "sw $s30, %d\n", temp->op1Num);
        }
        else {
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          contLinha++;
          fprintf(listing, "sw $s%d, %d\n", posTemporario1-1, temp->op1Num);
        }
      }
      else if (temp->op2Flag == 4 && temp->op1Flag == 3){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg+1, posTemporario1-1, reg-1);
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        fprintf(listing, "lr $s%d, $s%d\n", posTemporario1-1, reg);
        loadRegister = posTemporario1-1;
      }
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
    posTemporario1 = reg;
  }
  else if ((strcmp(temp->nome, "if_f") == 0)){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    fprintf(listing, "li $s0, 1\n");
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    contLinha++;
    fprintf(listing, "beq $s%d, $s0, %d\n", posTemporario1, linhaLabel[temp->op2Num]);
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
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", reg+1, reg);
        posTemporario1 = posTemporario1+1;
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
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
        posTemporario1 = posTemporario1+1;
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
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
        posTemporario1 = posTemporario1+1;
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
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
        posTemporario1 = posTemporario1+1;
      }
    }
    else if (temp->op1Flag == 3){
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg+1, posTemporario1, reg-1);
        if (temp->temp > 0){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          contLinha++;
          fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg+1);
        }
      }
      else if (temp->op2Flag == 1 || temp->op2Flag == 4){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, reg-1);
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
      }
      else if (temp->op2Flag == 3){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, posTemporario1-1);
      }
    }
    else if (temp->op1Flag == 4){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        reg++;
      }
      else if (temp->op2Flag == 4){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        reg++;
      }
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      contLinha++;
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        contLinha++;
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
      }
    }
  }
}
