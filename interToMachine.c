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
int numVetores = 0;

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
int indicePilha = 0;
int retornoFuncao[200];

void percorreLista(){
  cel *temp;
  cel *temp2;
  contLinha = 1;
  fun = malloc(sizeof(funFila));
  par = malloc(sizeof(paramFila));
  funcao = malloc(sizeof(funCel));
  param = malloc(sizeof(paramCel));
  inicializaFilaFun(fun);
  inicializaFilaParam(par);
  reg = 1;
  fprintf(listing, "\n");
  posTemporario1 = 4;
  temp = malloc(sizeof(cel));
  temp2 = malloc(sizeof(cel));
  *temp2 = *f->inicio;
  if (posMain > 0) contLinha++;
  inicializaFilaParam(par);
  while (temp2 != NULL){
    mapeiaLabel(temp2);
    if (temp2->prox == f->fim->prox){
      temp2 = NULL;
    }
    else {
      *temp2 = *temp2->prox;
    }
  }
  if (numVetores > 0){
    fun->inicio->posicaoInicio = fun->inicio->posicaoInicio + numVetores + 1;
    fun->inicio->posicaoFim = fun->inicio->posicaoFim + numVetores + 1;
  }
  contLinha = numVetores + 1;
  if (posMain > 2){
    funcao = buscaFilaFunByHash(fun, posMain);
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    fprintf(listing, "j %d\n", funcao->posicaoInicio);
    converteTipoJ(16, funcao->posicaoInicio);
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
  converteTipoT(18);
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
      contLinha = contLinha;
      insereFilaParam(par, *param);
    }
    else if (temp->op1Flag == 5){
      param->tipo = 5;
      param->valor = temp->op1Num;
      param->prox = NULL;
      contLinha = contLinha;
      insereFilaParam(par, *param);
    }
  }
  else if (strcmp(temp->nome, "cal") == 0){
    reg = 1;
    int i;
    int hashValue;
    if (temp->op1Num == 1 || temp->op1Num == 0){
      contLinha++;
    }
    else {
      int posParam = temp->op1Num;
      funCel *tempFun = malloc(sizeof(funCel));
      tempFun = buscaFilaFunByHash(fun, temp->op1Num);
      contLinha = contLinha + 3;
      if (tempFun->hash > -1){
        contLinha++;
      }
      if (temp->temp > 0){
        retornoFuncao[temp->temp] = 1;
      }
      else {
        retornoFuncao[temp->temp] = 0;
      }
    }
  }
  else if (strcmp(temp->nome, "got") == 0){
    contLinha++;
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (temp->op1Flag == 5){
      reg = 1;
      numVetores++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", numVetores);
      int aux;
      aux = contLinha;
      contLinha = numVetores;
      converteTipoE(25, reg, temp->op1Num);
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      contLinha = aux;
      contLinha++;
      numVetores++;
      aux = contLinha;
      contLinha = numVetores;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", numVetores);
      converteTipoE(24, reg, temp->op1Num);
      contLinha = aux;
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
      contLinha++;
      contLinha = contLinha + 3;
    }
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
    if (temp->op1Flag == 3){
      if (temp->op2Flag == 0){
        contLinha = contLinha + 2;
        if (temp->temp > 0){
          contLinha++;
        }
      }
      else if (temp->op2Flag == 1 || temp->op2Flag == 4){
        contLinha = contLinha + 3;
      }
      else if (temp->op2Flag == 3){
        contLinha++;
      }
    }
    else if (temp->op1Flag == 4){
      contLinha = contLinha + 2;
      if (temp->op2Flag == 0){
        contLinha++;
      }
      else if (temp->op2Flag == 4){
        contLinha++;
      }
      if (temp->temp > 0){
        contLinha++;
      }
    }
    else {
      if (temp->op1Flag == 0){ //const op ~
        contLinha++;
      }
      else if (temp->op1Flag == 1){ //var op ~
        contLinha++;
      }
      if (temp->op2Flag == 0){
        contLinha++;
      }
      else if (temp->op2Flag == 1){
        contLinha++;
      }
      contLinha++;
      if (temp->temp > 0){
        contLinha++;
      }
    }
  }
}

void converteParaMaquina(cel *temp){
  reg = 1;
  if (strcmp(temp->nome,"lab") == 0){
    if(temp->op1Flag != 2){
        if (ultimaFuncao == -1){
          ultimaFuncao = temp->op1Num;
        }
        else {
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          fprintf(listing, "lw $s32, %d\n", ultimaFuncao);
          converteTipoE(23, 32, ultimaFuncao);
          contLinha++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(27, 32, 0);
          fprintf(listing, "jr $s32\n");
          indicePilha++;
          contLinha++;
          ultimaFuncao = temp->op1Num;
        }
        fprintf(listing, "\n");
    }
  }
  else if (strcmp(temp->nome, "cal") == 0){
    reg = 1;
    int i;
    int hashValue;
    if (temp->op1Num == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(19, posTemporario1, 0);
      fprintf(listing, "in $s%d\n", posTemporario1);
      contLinha++;
    }
    else if (temp->op1Num == 1){
      int posParam = temp->op1Num;
      paramCel *aux = malloc(sizeof(paramCel));
      *aux = *par->inicio;
      for (i = 0; i < temp->op2Num; i++){
        if (aux->tipo == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(23, reg, aux->valor);
          fprintf(listing, "lw $s%d, %d\n", reg, aux->valor);
          contLinha++;
          posParam++;
          aux = aux->prox;
        }
        else if (aux->tipo == 5){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(25, reg, aux->valor);
          fprintf(listing, "li $s%d, %d\n", reg, aux->valor);
          contLinha++;
          posParam++;
          aux = aux->prox;
        }
      }
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(20, reg, 0);
      fprintf(listing, "out $s%d\n", reg);
      contLinha++;
      inicializaFilaParam(par);
    }
    else {
      int posParam = temp->op1Num;
      paramCel *aux = malloc(sizeof(paramCel));
      funCel *tempFun = malloc(sizeof(funCel));
      *aux = *par->inicio;
      for (i = 0; i < temp->op2Num; i++){
        if (aux->tipo == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(23, reg, aux->valor);
          fprintf(listing, "lw $s%d, %d\n", reg, aux->valor);
          contLinha++;
          posParam++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(24, reg, posParam);
          fprintf(listing, "sw $s%d, %d\n", reg, posParam);
          contLinha++;
          aux = aux->prox;
        }
        if (aux->tipo == 5){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(25, reg, aux->valor);
          fprintf(listing, "li $s%d, %d\n", reg, aux->valor);
          contLinha++;
          posParam++;
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(24, reg, posParam);
          fprintf(listing, "sw $s%d, %d\n", reg, posParam);
          contLinha++;
          aux = aux->prox;
        }
      }
      tempFun = buscaFilaFunByHash(fun, temp->op1Num);
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(25, 32, contLinha+3);
      fprintf(listing, "li $s32, %d\n", contLinha+3);
      contLinha++;
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(24, 32, temp->op1Num);
      fprintf(listing, "sw $s32, %d\n", temp->op1Num);
      contLinha++;
      if (tempFun->hash > -1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoJ(16, tempFun->posicaoInicio);
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
  }
  else if (strcmp(temp->nome, "par") == 0){
    if (temp->op1Flag == 1){
      param->tipo = 1;
      param->valor = temp->op1Num;
      param->prox = NULL;
      insereFilaParam(par, *param);
    }
    else if (temp->op1Flag == 5){
      param->tipo = 5;
      param->valor = temp->op1Num;
      param->prox = NULL;
      insereFilaParam(par, *param);
    }
  }
  else if (strcmp(temp->nome, "ret") == 0){
    if (temp->op1Flag == 1){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(23, 30, temp->op1Num);
      fprintf(listing, "lw $s30, %d\n", temp->op1Num);
      contLinha++;
    }
    else if (temp->op1Flag == 3){
      if (recebeRetorno == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(22, posTemporario1-1, 30, 0);
        fprintf(listing, "move $s30, $s%d\n", posTemporario1-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 0){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(25, 30, temp->op1Num);
      fprintf(listing, "li $s30, %d\n", temp->op1Num);
      contLinha++;
    }
  }
  else if (strcmp(temp->nome, "got") == 0){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    converteTipoJ(16, linhaLabel[temp->op1Num]);
    fprintf(listing, "j %d\n", linhaLabel[temp->op1Num]);
    contLinha++;
  }
  else if (strcmp(temp->nome,"asg") == 0){
    if (temp->op1Flag == 5){ // vetor recebe
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(23, reg, temp->op1Num);
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      contLinha++;
      reg++;
      if (temp->op2Flag == 1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op2Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(1, reg-2, reg-1, reg);
        fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
        contLinha++;
      }
      else if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(1, reg-2, reg-1, reg);
        fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
        contLinha++;
      }
      if (temp->tempFlag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        if (retornoFuncao[temp->temp] == 1){
          converteTipoI(15, 30, reg, 0);
          fprintf(listing, "sr $s30, $s%d\n", reg);
        }
        else {
          converteTipoI(15, posTemporario1, reg, 0);
          fprintf(listing, "sr $s%d, $s%d\n", posTemporario1, reg);
        }
        contLinha++;
      }
      else {
        if (temp->tempFlag == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(23, posTemporario1, temp->temp);
          fprintf(listing, "lw $s%d, %d\n", posTemporario1, temp->temp);
        }
        else if (temp->tempFlag == 2){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(25, posTemporario1, temp->temp);
          fprintf(listing, "li $s%d, %d\n", posTemporario1, temp->temp);
        }
        contLinha++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(15, posTemporario1, reg, 0);
        fprintf(listing, "sr $s%d, $s%d\n", posTemporario1, reg);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 3){ // temporario recebe
      if (temp->op2Flag == 5){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        if (temp->tempFlag == 1){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(23, reg, temp->temp);
          fprintf(listing, "lw $s%d, %d\n", reg, temp->temp);
        }
        else if (temp->tempFlag == 2){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoE(25, reg, temp->temp);
          fprintf(listing, "li $s%d, %d\n", reg, temp->temp);
        }
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(1, reg-2, reg-1, reg);
        fprintf(listing, "sum $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
        contLinha++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(26, posTemporario1, reg, 0);
        fprintf(listing, "lr $s%d, $s%d\n", posTemporario1, reg);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 1){ // variavel recebe
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
      }
      else if (temp->op2Flag == 1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op2Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
      }
      else if (temp->op2Flag == 3){
        if (retornoFuncao[temp->op2Num] == 1) reg = 30;
        else reg = posTemporario1;
      }
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(24, reg, temp->op1Num);
      fprintf(listing, "sw $s%d, %d\n", reg, temp->op1Num);
      contLinha++;
    }
  }
  else if ((strcmp(temp->nome, "eq") == 0)){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    if (temp->op1Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
      converteTipoE(25, reg, temp->op1Num);
      reg++;
    }
    else if (temp->op1Flag == 1){
      converteTipoE(23, reg, temp->op1Num);
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      reg++;
    }
    contLinha++;
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    if (temp->op2Flag == 0){
      fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
      converteTipoE(25, reg, temp->op2Num);
      reg++;
    }
    else if (temp->op2Flag == 1){
      converteTipoE(23, reg, temp->op2Num);
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
      reg++;
    }
    contLinha++;
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    converteTipoR(26, reg-2, reg-1, reg);
    fprintf(listing, "eq $s%d, $s%d, $s%d\n", reg, reg-1, reg-2);
    contLinha++;
    posTemporario1 = reg;
  }
  else if ((strcmp(temp->nome, "if_f") == 0)){
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    converteTipoE(25, 0, 1);
    contLinha++;
    fprintf(listing, "li $s0, 1\n");
    fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
    converteTipoI(12, posTemporario1, 0, linhaLabel[temp->op2Num]);
    fprintf(listing, "beq $s%d, $s0, %d\n", posTemporario1, linhaLabel[temp->op2Num]);
    contLinha++;
  }
  else if ((strcmp(temp->nome, "sum") == 0)
          || (strcmp(temp->nome, "sub") == 0)
          || (strcmp(temp->nome, "div") == 0)
          || (strcmp(temp->nome, "mul") == 0)){
    if (temp->op1Flag == 3){
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(geraOpcode(temp->nome), posTemporario1, reg-1, reg+1);
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg+1, posTemporario1, reg-1);
        contLinha++;
        if (temp->temp > 0){
          fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
          converteTipoI(22, reg+1, posTemporario1, 0);
          fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg+1);
          contLinha++;
        }
      }
      else if (temp->op2Flag == 1 || temp->op2Flag == 4){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op2Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(geraOpcode(temp->nome), posTemporario1, reg-1, reg);
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, reg-1);
        contLinha++;
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(22, reg, posTemporario1, 0);
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
        contLinha++;
      }
      else if (temp->op2Flag == 3){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoR(geraOpcode(temp->nome), posTemporario1, posTemporario1-1, reg);
        fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, posTemporario1, posTemporario1-1);
        contLinha++;
      }
    }
    else if (temp->op1Flag == 4){
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoE(23, reg, temp->op1Num);
      fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
      contLinha++;
      reg++;
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
      }
      else if (temp->op2Flag == 4){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op2Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        reg++;
      }
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      converteTipoR(geraOpcode(temp->nome), reg-2, reg-1, reg);
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg, reg-1, reg-2);
      contLinha++;
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(22, reg, posTemporario1, 0);
        fprintf(listing, "move $s%d, $s%d\n", posTemporario1, reg);
        contLinha++;
      }
    }
    else {
      if (temp->op1Flag == 0){ //const op ~
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op1Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op1Num);
        contLinha++;
        reg++;
      }
      else if (temp->op1Flag == 1){ //var op ~
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op1Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op1Num);
        contLinha++;
        reg++;
      }
      if (temp->op2Flag == 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(25, reg, temp->op2Num);
        fprintf(listing, "li $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        posTemporario1 = reg;
      }
      else if (temp->op2Flag == 1){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoE(23, reg, temp->op2Num);
        fprintf(listing, "lw $s%d, %d\n", reg, temp->op2Num);
        contLinha++;
        posTemporario1 = reg;
      }
      fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
      fprintf(listing, "%s $s%d, $s%d, $s%d\n", temp->nome, reg+1, reg-1, posTemporario1);
      converteTipoR(geraOpcode(temp->nome), reg-1, posTemporario1, reg+1);
      contLinha++;
      reg++;
      posTemporario1 = 4;
      if (temp->temp > 0){
        fprintf(listing, "memoriaDeInstrucoes[%d] = ", contLinha);
        converteTipoI(22, reg, reg+1, 0);
        fprintf(listing, "move $s%d, $s%d\n", reg+1, reg);
        contLinha++;
      }
    }
  }
}

int geraOpcode(char * nome){
  if (strcmp(nome, "sum") == 0){
    return 1;
  }
  else if (strcmp(nome, "sub") == 0){
    return 3;
  }
}

void converteTipoR(int opcode, int r1, int r2, int r3){
  fprintf(code, "memoriaDeInstrucoes[%d] = {5'd%d, 5'd%d, 5'd%d, 5'd%d, 12'dx};\n", contLinha, opcode, r1, r2, r3);
}

void converteTipoI(int opcode, int r1, int r2, int imediato17){
  fprintf(code, "memoriaDeInstrucoes[%d] = {5'd%d, 5'd%d, 5'd%d, 17'd%d};\n", contLinha, opcode, r1, r2, imediato17);
}

void converteTipoJ(int opcode, int imediato27){
  fprintf(code, "memoriaDeInstrucoes[%d] = {5'd%d, 27'd%d};\n", contLinha, opcode, imediato27);
}

void converteTipoT(int opcode){
  fprintf(code, "memoriaDeInstrucoes[%d] = {5'd%d, 27'dx};\n", contLinha, opcode);
}

void converteTipoE(int opcode, int r1, int imediato22){
  fprintf(code, "memoriaDeInstrucoes[%d] = {5'd%d, 5'd%d, 22'd%d};\n", contLinha, opcode, r1, imediato22);
}
