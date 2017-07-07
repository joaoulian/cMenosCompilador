#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interToMachine.h"

static void cGen(TreeNode * tree);

cel *elemento;

int indicadorLinha = 0;
int ehVetor = 0;
int tempAtribuicaoVetor = -1;
int indiceT = 1;
int verificaCall = 0;
int verificaCall2 = 0;
int linhaElse[30];
int linhaFinal[30];
int linhaWhile[30];
int linhaFinalWhile[30];
int whileCorrente = 0;
int ifCorrente = 0;
int hashValue = 0;

TipType auxTipo;

//[INICIO] Gera os nós de declarações (if, while, retorno)
static void genStmt(TreeNode * tree) {
  TreeNode * p1, * p2, * p3;
  switch (tree->kind.stmt) {

    //[INICIO] Gera o if
    case IfK:
      p1 = tree->child[0];
      p2 = tree->child[1];
      p3 = tree->child[2];
      cGen(p1);
      fprintf(listing, "if_false t%d ", indiceT-1);
      indicadorLinha++;
      linhaElse[ifCorrente] = indicadorLinha;
      elemento->op1Num = indiceT-1;
      elemento->op1Flag = 3;
      elemento->nome = "if_f";
      elemento->op2Flag = 2;
      elemento->temp = 0;
      elemento->prox = NULL;
      if (p3 != NULL) {
        fprintf(listing, "goto L%d\n", linhaElse[ifCorrente]);
        elemento->op2Num = linhaElse[ifCorrente];
        indicadorLinha++;
        linhaFinal[ifCorrente] = indicadorLinha;
      }
      else {
        linhaFinal[ifCorrente] = indicadorLinha;
        fprintf(listing, "goto L%d\n", linhaFinal[ifCorrente]);
        elemento->op2Num = linhaFinal[ifCorrente];
      }
      insereFila(f, *elemento);
      elemento->op1Flag = -1;
      elemento->op2Flag = -1;
      ifCorrente++;
      cGen(p2);
      ifCorrente--;
      if (p3 != NULL){
        fprintf(listing, "goto L%d\n", linhaFinal[ifCorrente]);
        elemento->nome = "got";
        elemento->op1Flag = 2;
        elemento->op1Num = linhaFinal[ifCorrente];
        elemento->op2Flag = -1;
        elemento->op2Num = 0;
        elemento->temp = 0;
        elemento->prox = NULL;
        insereFila(f, *elemento);
        elemento->op1Flag = -1;
        elemento->op2Flag = -1;
        fprintf(listing, "L%d: ", linhaElse[ifCorrente]);
        elemento->nome = "lab";
        elemento->op1Flag = 2;
        elemento->op1Num = linhaElse[ifCorrente];
        elemento->op2Flag = -1;
        elemento->temp = 0;
        elemento->tempFlag = -1;
        elemento->prox = NULL;
        insereFila(f, *elemento);
        elemento->op2Flag = -1;
        elemento->op1Flag = -1;
        elemento->temp = 0;
        cGen(p3);
      }
      fprintf(listing, "L%d: ", linhaFinal[ifCorrente]);
      elemento->nome = "lab";
      elemento->op1Flag = 2;
      elemento->op1Num = linhaFinal[ifCorrente];
      elemento->op2Flag = -1;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op2Flag = -1;
      elemento->op1Flag = -1;
      elemento->temp = 0;
      break;
    //[FIM] Gera o if

    //[INICIO] Gera o while
    case WhileK:
      p1 = tree->child[0];
      p2 = tree->child[1];
      indicadorLinha++;
      fprintf(listing, "L%d: ", indicadorLinha);
      linhaWhile[whileCorrente] = indicadorLinha;
      elemento->nome = "lab";
      elemento->op1Flag = 2;
      elemento->op1Num = indicadorLinha;
      elemento->op2Flag = -1;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op2Flag = -1;
      elemento->op1Flag = -1;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      cGen(p1);
      fprintf(listing, "if_false t%d", indiceT-1);
      indicadorLinha++;
      linhaFinalWhile[whileCorrente] = indicadorLinha;
      fprintf(listing, " goto L%d\n", indicadorLinha);
      elemento->nome = "if_f";
      elemento->op1Flag = 3;
      elemento->op1Num = indiceT-1;
      elemento->op2Num = indicadorLinha;
      elemento->op2Flag = 2;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->op1Flag = -1;
      elemento->op2Flag = -1;
      whileCorrente++;
      cGen(p2);
      whileCorrente--;
      fprintf(listing, "goto L%d\n", linhaWhile[whileCorrente]);
      elemento->nome = "got";
      elemento->op1Flag = 2;
      elemento->op1Num = linhaWhile[whileCorrente];
      elemento->op2Flag = -1;
      elemento->op2Num = 0;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op1Flag = -1;
      elemento->op2Flag = -1;
      fprintf(listing, "L%d: ", linhaFinalWhile[whileCorrente]);
      elemento->nome = "lab";
      elemento->op1Flag = 2;
      elemento->op1Num = linhaFinalWhile[whileCorrente];
      elemento->op2Flag = -1;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      elemento->tempFlag = -1;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op2Flag = -1;
      elemento->op1Flag = -1;
      elemento->temp = 0;
      elemento->tempFlag = -1;
      fprintf(listing, "\n");
      break;
    //[FIM] Gera o while

    //[INICIO] Gera o retorno
    case ReturnK:
      p1 = tree->child[0];
      if (p1->kind.expression == CallK) {
        verificaCall = 2;
        if (p1 != NULL)
          cGen(p1);
        fprintf(listing, "return t%d\n", indiceT-1);
        elemento->nome = "ret";
        elemento->op1Flag = 3;
        elemento->op1Num = indiceT-1;
        elemento->op2Flag = -1;
        elemento->op2Num = 0;
        elemento->prox = NULL;
        elemento->temp = 0;
        elemento->tempFlag = -1;
        insereFila(f, *elemento);
        elemento->op1Flag = -1;
        elemento->op2Flag = -1;
        elemento->temp = 0;
        elemento->tempFlag = -1;
        fprintf(listing, "\n");
      }
      else {
        if (p1->kind.expression == OpK){
          if (p1 != NULL)
            cGen(p1);
          fprintf(listing, "return t%d", indiceT-1);
          elemento->nome = "ret";
          elemento->op1Flag = 3;
          elemento->op1Num = indiceT-1;
          elemento->op2Flag = -1;
          elemento->op2Num = 0;
          elemento->prox = NULL;
          elemento->temp = 0;
          elemento->tempFlag = -1;
          insereFila(f, *elemento);
          elemento->op2Flag = -1;
          elemento->op1Flag = -1;
          fprintf(listing, "\n");
        }
        else {
          fprintf(listing, "return ");
          if (p1 != NULL)
            cGen(p1);
          elemento->nome = "ret";
          elemento->op2Flag = -1;
          elemento->op2Num = 0;
          elemento->prox = NULL;
          elemento->temp = 0;
          elemento->tempFlag = -1;
          insereFila(f, *elemento);
          elemento->op2Flag = -1;
          elemento->op1Flag = -1;
          fprintf(listing, "\n");
        }
      }
      break;
      //[FIM] Gera o retorno

  }
}
//[FIM] Gera os nós de declarações (if, while, retorno)

//[INICIO] Gera os nós de declarações de função
static void genDecli(TreeNode * tree) {
  TreeNode * p1, * p2;
  switch (tree->kind.decl) {

    case FunK:
      if (tree != NULL) {
        fprintf(listing,  "\n%s:\n", tree->attr.name);
        elemento->nome = "lab";
        elemento->op1Flag = 1;
        elemento->op1Num = buscaMemoria(tree->attr.name);
        if (strcmp(tree->attr.name, "main") == 0){
          posMain = elemento->op1Num;
        }
        elemento->op2Flag = -1;
        elemento->op2Num = 0;
        elemento->temp = 0;
        elemento->tempFlag = -1;
        elemento->prox = NULL;
        insereFila(f, *elemento);
        elemento->op1Flag = -1;
        elemento->op2Flag = -1;
        elemento->temp = 0;
        elemento->tempFlag = -1;
      }
      if (tree->child[1] != NULL)
        cGen(tree->child[1]);
      break;
    default:
      break;

  }
}
//[FIM] Gera os nós de declarações

static void genParam(TreeNode * tree) {
  TreeNode * p1, * p2;
  switch (tree->kind.param) {

    case DeclK:

    default:
      break;
  }
}

//[INICIO] Funções que gera o tipo de operação e insere na lista
static void showOp(TokenType op){
  if (op == TIMES){
    fprintf(listing, " * ");
    elemento->nome = "mul";
  }
  if (op == OVER){
    fprintf(listing, " / ");
    elemento->nome = "div";
  }
  if (op == PLUS){
    fprintf(listing, " + ");
    elemento->nome = "sum";
  }
  if (op == MINUS){
    fprintf(listing, " - ");
    elemento->nome = "sub";
  }
  if (op == EQ){
    fprintf(listing, " == ");
    elemento->nome = "eq";
  }
  if (op == NEQ){
    fprintf(listing, " != ");
    elemento->nome = "neq";
  }
  if (op == LEQ){
    fprintf(listing, " <= ");
    elemento->nome = "leq";
  }
  if (op == MEQ){
    fprintf(listing, " >= ");
    elemento->nome = "meq";
  }
  if (op == MT){
    fprintf(listing, " > ");
    elemento->nome = "mor";
  }
  if (op == LT){
    fprintf(listing, " < ");
    elemento->nome = "les";
  }
}
//[FIM] Funções que gera o tipo de operação e insere na lista

//[INICIO] Gera os nós de expressões
static void genExpression(TreeNode * tree) {
  int contCall2;
  TreeNode * p1, * p2, * p3;
  if (tree != NULL) {
    switch (tree->kind.expression) {

      //[INICIO] Gera constante
      case ConstK:
        fprintf(listing, "%d", tree->attr.val);
        if (elemento->op1Flag == -1){
          elemento->op1Num = tree->attr.val;
          elemento->op1Flag = 0;
        }
        else if (elemento->op2Flag == -1){
          elemento->op2Num = tree->attr.val;
          elemento->op2Flag = 0;
        }
        else {
          elemento->temp = tree->attr.val;
          elemento->tempFlag = 2;
        }
        break;
      //[FIM] Gera constante

      //[INICIO] Gera variavel
      case VariavelK:
        if (tree != NULL)
        fprintf(listing, "%s", tree->attr.name);
        if (elemento->op1Flag == -1){
          elemento->op1Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
          elemento->op1Flag = 1;
        }
        else if (elemento->op2Flag == -1){
          elemento->op2Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
          elemento->op2Flag = 1;
        }
        else {
          elemento->temp = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
          elemento->tempFlag = 1;
        }
        break;
      //[FIM] Gera variavel

      //[INICIO] Gera chamada de função
      case CallK:
         contCall2 = indicadorLinha;
         int temp = 0;
         TreeNode * other;
         if (strcmp(tree->attr.name, "input") == 0){
           fprintf(listing, "t%d = call %s\n", indiceT, tree->attr.name);
           verificaCall = 0;
           elemento->nome = "cal";
           elemento->op1Flag = 1;
           elemento->op1Num = buscaMemoria(tree->attr.name);
           elemento->temp = indiceT;
           elemento->prox = NULL;
           insereFila(f, *elemento);
           elemento->op2Flag = -1;
           elemento->op1Flag = -1;
           elemento->temp = 0;
           elemento->tempFlag = -1;
           indiceT++;
         }
         else {
           if (tree != NULL && tree->child[0] != NULL)
            other = tree->child[0];
            while (other != NULL) {
               auxTipo = buscaTipoComEscopo(other->attr.name, other->escopo);
               temp++;
               if (auxTipo == Variavel) {
                 fprintf(listing, "param: ");
                 elemento->nome = "par";
                 elemento->op1Flag = 1;
                 elemento->op1Num = buscaMemoriaComEscopo(other->attr.name, other->escopo);
                 elemento->op2Flag = -1;
                 elemento->op2Num = 0;
                 elemento->prox = NULL;
                 elemento->temp = 0;
                 elemento->tempFlag = -1;
                 insereFila(f, *elemento);
                 elemento->op1Flag = -1;
                 fprintf(listing, "%s ", other->attr.name);
               }
               else if (auxTipo == Vetor){
                 fprintf(listing, "param: ");
                 elemento->nome = "par";
                 elemento->op1Flag = 5;
                 elemento->op1Num = buscaMemoriaComEscopo(other->attr.name, other->escopo);
                 elemento->op2Flag = -1;
                 elemento->op2Num = 0;
                 elemento->prox = NULL;
                 elemento->temp = 0;
                 elemento->tempFlag = -1;
                 insereFila(f, *elemento);
                 elemento->op1Flag = -1;
                 fprintf(listing, "%s ", other->attr.name);
               }
             if (other->kind.expression == CallK) {
               verificaCall = 3;
               cGen(other);
               TreeNode * a;
               int b = 0;
               a = other;
               while (a != NULL) {
                 b++;
                 a = a->sibling;
               }
               fprintf(listing, "param: t%d\n", indiceT);
               fprintf(listing, "call %s, %d\n", tree->attr.name, b);
               verificaCall = 1;
               indiceT++;
             } else if (other->kind.expression == OpK) {
               p1 = other->child[0];
               p2 = other->child[1];
               if (p1 != NULL && p2 != NULL) {
                 if (p1->kind.expression == ConstK) {
                   fprintf(listing, "t%d = ", indiceT);
                   elemento->temp = indiceT;
                   indiceT++;
                   if (p1 != NULL) {
                     cGen(p1);
                   }
                   showOp(other->attr.op);
                   if (p2 != NULL){
                     cGen(p2);
                   }
                   elemento->prox = NULL;
                   insereFila(f, *elemento);
                   elemento->op1Flag = -1;
                   elemento->op2Flag = -1;
                   elemento->temp = 0;
                   elemento->tempFlag = -1;
                   fprintf(listing, "\n");
                 }
                 if (p1->kind.expression != VariavelK && p1->kind.expression != VetorK && p1->kind.expression != CallK && p1->kind.expression != ConstK) {
                   if (p1 != NULL) {
                     cGen(p1);
                   }
                   else {
                     indiceT++;
                     fprintf(listing, "t%d = ", indiceT);
                   }
                   if (p2->kind.expression != VariavelK && p2->kind.expression != VetorK && p1->kind.expression != CallK) {
                     if (p2 != NULL)
                       cGen(p2);
                     fprintf(listing, "t%d = ", indiceT);
                     elemento->temp = indiceT;
                     fprintf(listing, "t%d", indiceT-1);
                     elemento->op1Flag = 3;
                     elemento->op1Num = indiceT-1;
                     showOp(other->attr.op);
                     elemento->op2Flag = 3;
                     elemento->op2Num = indiceT-2;
                     elemento->prox = NULL;
                     insereFila(f, *elemento);
                     elemento->op1Flag = -1;
                     elemento->op2Flag = -1;
                     elemento->temp = 0;
                     elemento->tempFlag = -1;
                     fprintf(listing, "t%d", indiceT-2);
                     indiceT++;
                   } else if (p2->kind.expression == VariavelK) {
                     fprintf(listing, "\nt%d = ", indiceT);
                     elemento->temp = indiceT;
                     fprintf(listing, "t%d", indiceT-1);
                     elemento->op1Flag = 3;
                     elemento->op1Num = indiceT-1;
                     showOp(other->attr.op);
                     indiceT++;
                     if (p2 != NULL)
                       cGen(p2);
                     elemento->prox = NULL;
                     insereFila(f, *elemento);
                     elemento->op1Flag = -1;
                     elemento->op2Flag = -1;
                     elemento->temp = 0;
                     elemento->tempFlag = -1;
                     fprintf(listing, "\n");
                   }
                 } else if (p1->kind.expression == VariavelK) {
                   if (p2->kind.expression != VetorK && p1->kind.expression != CallK && p2->kind.expression != OpK) {
                     fprintf(listing, "t%d = ", indiceT);
                     elemento->temp = indiceT;
                     if (p1 != NULL){
                       cGen(p1);
                     }
                     showOp(other->attr.op);
                     indiceT++;
                     if (p2 != NULL){
                       cGen(p2);
                     }
                     elemento->prox = NULL;
                     insereFila(f, *elemento);
                     elemento->op1Flag = -1;
                     elemento->op2Flag = -1;
                     elemento->temp = 0;
                     elemento->tempFlag = -1;
                     fprintf(listing, "\n");
                   }
                   if (p2->kind.expression == OpK){
                       if (p2 != NULL)
                        cGen(p2);
                       fprintf(listing, "t%d = ", indiceT);
                       elemento->temp = indiceT;
                       if (p1 != NULL)
                         cGen(p1);
                       showOp(other->attr.op);
                       fprintf(listing, "t%d", indiceT-1);
                       elemento->op2Flag = 3;
                       elemento->op2Num = indiceT-1;
                       elemento->prox = NULL;
                       insereFila(f, *elemento);
                       elemento->op2Flag = -1;
                       elemento->op1Flag = -1;
                       elemento->temp = 0;
                       elemento->tempFlag = -1;
                       indiceT++;
                       fprintf(listing, "\n");
                   }
                 }
               }
               fprintf(listing, "param: t%d", indiceT-1);
               elemento->nome = "par";
               elemento->op1Flag = 3;
               elemento->op1Num  = indiceT - 1;
               elemento->op2Flag = -1;
               elemento->op2Num = 0;
               elemento->temp = 0;
               elemento->tempFlag = -1;
               elemento->prox = NULL;
               insereFila(f, *elemento);
               elemento->temp = 0;
               elemento->tempFlag = -1;
               elemento->op2Flag = -1;
               elemento->op1Flag = -1;
             }
             fprintf(listing, "\n");
             other = other->sibling;
           }
           if (verificaCall == 0){
             fprintf(listing, "call %s, %d\n", tree->attr.name, temp);
             elemento->nome = "cal";
             elemento->op1Flag = 1;
             elemento->op1Num = buscaMemoria(tree->attr.name);
             elemento->op2Flag = 0;
             elemento->op2Num = temp;
             elemento->temp = 0;
             elemento->tempFlag = -1;
             elemento->prox = NULL;
             insereFila(f, *elemento);
             elemento->op2Flag = -1;
             elemento->op1Flag = -1;
             elemento->temp = 0;
             elemento->tempFlag = -1;
           }
           if (verificaCall == 2 || verificaCall == 3) {
             fprintf(listing, "t%d = call %s, %d\n", indiceT, tree->attr.name, temp);
             verificaCall = 0;
             elemento->nome = "cal";
             elemento->op1Flag = 1;
             elemento->op1Num = buscaMemoria(tree->attr.name);
             elemento->op2Flag = 0;
             elemento->op2Num = temp;
             elemento->temp = indiceT;
             elemento->prox = NULL;
             insereFila(f, *elemento);
             elemento->op2Flag = -1;
             elemento->op1Flag = -1;
             elemento->temp = 0;
             elemento->tempFlag = -1;
             indiceT++;
           }

           verificaCall = 0;
         }
         break;
      //[FIM] Gera chamada de função

      //[INICIO] Gera vetor
      case VetorK:
        if (tree->child[0] != NULL) {
          if (tree->child[0]->kind.expression == CallK || tree->child[0]->kind.expression == OpK) {
            if (ehVetor == 1){
              if (tree->child[0] != NULL) {
                cGen(tree->child[0]);
              }
              fprintf(listing, "%s[t%d]", tree->attr.name, indiceT-1);
              elemento->op1Flag = 4;
              elemento->op1Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
            }
            else {
              if (tree->child[0] != NULL) {
                cGen(tree->child[0]);
              }
              fprintf(listing, "t%d = ", indiceT);
              tempAtribuicaoVetor = indiceT;
              fprintf(listing, "%s[t%d]", tree->attr.name, indiceT-1);
              elemento->nome = "asg";
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT;
              elemento->op2Flag = 4;
              elemento->op2Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              elemento->temp = indiceT-1;
              elemento->tempFlag = 0;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              indiceT++;
              fprintf(listing, "\n");
            }
          }
          else if (tree->child[0]->kind.expression == VariavelK){
            if (ehVetor == 2){
              fprintf(listing, "t%d = ", indiceT);
              elemento->nome = "asg";
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT;
              tempAtribuicaoVetor = indiceT;
              indiceT++;
              elemento->op2Flag = 5;
              elemento->op2Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              fprintf(listing, "%s[", tree->attr.name);
              if (tree->child[0] != NULL)
                cGen(tree->child[0]);
              fprintf(listing, "]");
              insereFila(f, *elemento);
              fprintf(listing, "\n");
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->tempFlag = -1;
            }
            else {
              elemento->op1Flag = 5;
              elemento->op1Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              fprintf(listing, "%s[", tree->attr.name);
              elemento->op2Flag = -1;
              if (tree->child[0] != NULL)
                cGen(tree->child[0]);
              fprintf(listing, "]");
            }
          }
          else if (tree->child[0]->kind.expression == ConstK){
            if (ehVetor == 2){
              fprintf(listing, "t%d = ", indiceT);
              elemento->nome = "asg";
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT;
              tempAtribuicaoVetor = indiceT;
              indiceT++;
              elemento->op2Flag = 5;
              elemento->op2Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              fprintf(listing, "%s[", tree->attr.name);
              if (tree->child[0] != NULL)
                cGen(tree->child[0]);
              elemento->temp = elemento->temp + 1;
              fprintf(listing, "]");
              insereFila(f, *elemento);
              fprintf(listing, "\n");
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->tempFlag = -1;
            }
            else {
              elemento->op1Flag = 5;
              elemento->op1Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo);
              fprintf(listing, "%s[", tree->attr.name);
              elemento->op2Flag = -1;
              if (tree->child[0] != NULL)
                cGen(tree->child[0]);
              fprintf(listing, "]");
              elemento->op2Num = elemento->op2Num + 1;
            }
          }
          /*else {
            if (elemento->op1Flag == -1){
              elemento->op1Flag = -2;
              elemento->op2Flag = -2;
            }
            else {
              if (elemento->op1Flag != 5){
                elemento->op2Flag = -2;
              }
            }
            fprintf(listing, "%s[", tree->attr.name);
            if (tree->child[0] != NULL)
              cGen(tree->child[0]);
            fprintf(listing, "]");
            if (elemento->op1Flag == -2){
              elemento->op1Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo) + (tree->child[0]->attr.val + 1);
              elemento->op1Flag = 4;
              elemento->op2Flag = -1;
            }
            else {
              if (elemento->op1Flag == 5){
                elemento->temp = buscaMemoriaComEscopo(tree->attr.name, tree->escopo) + (tree->child[0]->attr.val + 1);
                elemento->tempFlag = 1;
              }
              else {
                elemento->op2Num = buscaMemoriaComEscopo(tree->attr.name, tree->escopo) + (tree->child[0]->attr.val + 1);
                elemento->op2Flag = 4;
              }
            }
          }*/
        }
        break;
      //[FIM] Gera vetor

      //[INICIO] Gera atribuição
      case EqualK:
        p1 = tree->child[0];
        p2 = tree->child[1];
        if (p1 != NULL && p2 != NULL) {
          if (p2->kind.expression == OpK) {
            if (p1->kind.expression == VetorK){
              if (p2 != NULL)
                cGen(p2);
              ehVetor = 0;
              if (p1 != NULL)
                cGen(p1);
              elemento->nome = "asg";
              fprintf(listing, " = t%d\n", indiceT-1);
              elemento->temp = indiceT-1;
              elemento->tempFlag = 0;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
            }
            else {
              if (p2 != NULL)
                cGen(p2);
              if (p1 != NULL)
                cGen(p1);
              elemento->nome = "asg";
              fprintf(listing, " = t%d\n", indiceT-1);
              elemento->op2Num  = indiceT-1;
              elemento->op2Flag = 3;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
            }
          }
          else if (p2->kind.expression == VariavelK || p2->kind.expression == ConstK) {
            if (p1 != NULL){
              ehVetor = 0;
              cGen(p1);
            }
            elemento->nome = "asg";
            fprintf(listing, " = ");
            if (p2 != NULL)
              cGen(p2);
            insereFila(f, *elemento);
            elemento->op1Flag = -1;
            elemento->op2Flag = -1;
            fprintf(listing, "\n");
          }
          else if (p2->kind.expression == CallK) {
            verificaCall = 2;
            if (p2 != NULL)
              cGen(p2);
            if (p1->kind.expression == VetorK){
              if (p1 != NULL)
                cGen(p1);
              elemento->nome = "asg";
              elemento->tempFlag = 0;
              elemento->temp = indiceT-1;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->temp = 0;
              elemento->tempFlag = -1;
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              fprintf(listing, " = t%d", indiceT-1);
              indiceT++;
              fprintf(listing, "\n");
              verificaCall = 0;
            }
            else {
              if (p1 != NULL)
                cGen(p1);
              elemento->nome = "asg";
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              elemento->prox = NULL;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              insereFila(f, *elemento);
              elemento->temp = 0;
              elemento->tempFlag = -1;
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              fprintf(listing, " = t%d", indiceT-1);
              indiceT++;
              fprintf(listing, "\n");
              verificaCall = 0;
            }
          }
          else if (p2->kind.expression == VetorK) {
            if (p1->kind.expression == VariavelK) {
              if (p2 != NULL){
                ehVetor = 2;
                cGen(p2);
              }
              if (p1 != NULL)
                cGen(p1);
              fprintf(listing, " = ");
              fprintf(listing, "t%d\n", indiceT-1);
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
            }
            else if (p1->kind.expression == VetorK){
              if ((p2->child[0]->kind.expression == CallK || p2->child[0]->kind.expression == OpK) && p1->child[0]->kind.expression == OpK) {
                if (p2 != NULL)
                  cGen(p2);
                ehVetor = 1;
                if (p1 != NULL)
                  cGen(p1);
                elemento->nome = "asg";
                fprintf(listing, " = ");
                if (tempAtribuicaoVetor > -1){
                  fprintf(listing, "t%d\n", tempAtribuicaoVetor);
                }
                elemento->temp = tempAtribuicaoVetor;
                elemento->prox = NULL;
                insereFila(f, *elemento);
                elemento->op2Flag = -1;
                elemento->op1Flag = -1;
              }
              else if (p1->child[0]->kind.expression == VariavelK || p1->child[0]->kind.expression == ConstK){
                  ehVetor = 2;
                  if (p2 != NULL)
                    cGen(p2);
                  ehVetor = 0;
                  if (p1 != NULL)
                    cGen(p1);
                  elemento->nome = "asg";
                  fprintf(listing, " = ");
                  if (tempAtribuicaoVetor > -1){
                    fprintf(listing, "t%d\n", tempAtribuicaoVetor);
                  }
                  elemento->temp = tempAtribuicaoVetor;
                  elemento->tempFlag = 0;
                  elemento->prox = NULL;
                  insereFila(f, *elemento);
                  elemento->op2Flag = -1;
                  elemento->op1Flag = -1;
                  elemento->tempFlag = -1;
              }
            }
          }
        }
        break;
      //[FIM] Gera atribuição

      //[INICIO] Gera operação
      case OpK:
        p1 = tree->child[0];
        p2 = tree->child[1];
        p3 = tree->child[2];
        int contOp = indiceT;
        if (p1 != NULL && p2 != NULL) {
          if (p1->kind.expression == VetorK){ //[INICIO] p1: VetorK
            if (p1->child[0]->kind.expression == VariavelK || p1->child[0]->kind.expression == ConstK){
              if (p1 != NULL) {
                ehVetor = 2;
                cGen(p1);
              }
              if (p2->kind.expression == VetorK){
                if (p2->child[0]->kind.expression == VariavelK || p2->child[0]->kind.expression == ConstK){
                  if (p2 != NULL){
                    cGen(p2);
                  }
                  fprintf(listing, "t%d = t%d", indiceT-2);
                  elemento->temp = indiceT;
                  elemento->tempFlag = 0;
                  indiceT++;
                  showOp(tree->attr.op);
                  fprintf(listing, "t%d\n", indiceT-1);
                  elemento->op1Flag = 3;
                  elemento->op1Num = indiceT-2;
                  elemento->op2Flag = 3;
                  elemento->op2Num = indiceT-1;
                  elemento->prox = NULL;
                  insereFila(f, *elemento);
                  elemento->op1Flag = -1;
                  elemento->op2Flag = -1;
                }
              }
              else if (p2->kind.expression == CallK){
                if (p2 != NULL){
                  cGen(p2);
                }
                fprintf(listing, "t%d = t%d", indiceT-2);
                elemento->temp = indiceT;
                elemento->tempFlag = 0;
                indiceT++;
                showOp(tree->attr.op);
                fprintf(listing, "t%d\n", indiceT-1);
                elemento->op1Flag = 3;
                elemento->op1Num = indiceT-2;
                elemento->op2Flag = 3;
                elemento->op2Num = indiceT-1;
                elemento->prox = NULL;
                insereFila(f, *elemento);
                elemento->op1Flag = -1;
                elemento->op2Flag = -1;
              }
              else if (p2->kind.expression == ConstK || p2->kind.expression == VariavelK){
                fprintf(listing, "t%d = t%d", indiceT, indiceT-1);
                elemento->temp = indiceT;
                elemento->tempFlag = 0;
                elemento->op1Flag = 3;
                elemento->op1Num = indiceT-1;
                indiceT++;
                showOp(tree->attr.op);
                if (p2 != NULL){
                  cGen(p2);
                }
                elemento->prox = NULL;
                insereFila(f, *elemento);
                elemento->op1Flag = -1;
                elemento->op2Flag = -1;
                elemento->temp = 0;
                elemento->tempFlag = -1;
                fprintf(listing, "\n");
              }
            }
          } //[FIM] p1: VetorK
          else if (p1->kind.expression == OpK) { //[INICIO] p1: OpK
            if (p2->kind.expression == OpK){
              if (p1 != NULL) {
                cGen(p1);
              }
              if (p2 != NULL){
                cGen(p2);
              }
              fprintf(listing, "t%d = t%d", indiceT, indiceT-1);
              showOp(tree->attr.op);
              fprintf(listing, "t%d", indiceT-2);
              indiceT++;
              elemento->temp = indiceT-1;
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-2;
              elemento->op2Num = indiceT-3;
              elemento->op2Flag = 3;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
            else if (p2->kind.expression == ConstK || p2->kind.expression == VariavelK) {
              if (p1 != NULL) {
                cGen(p1);
              }
              fprintf(listing, "t%d = t%d", indiceT, indiceT-1);
              indiceT++;
              elemento->temp = indiceT-1;
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-2;
              showOp(tree->attr.op);
              if (p2 != NULL){
                cGen(p2);
              }
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
            else if (p2->kind.expression == VetorK) {
              if (p1 != NULL) {
                cGen(p1);
              }
              if (p2->child[0]->kind.expression == VariavelK || p2->child[0]->kind.expression == ConstK){
                if (p2 != NULL) {
                  ehVetor = 2;
                  cGen(p2);
                }
                fprintf(listing, "t%d = t%d", indiceT, indiceT-2);
                showOp(tree->attr.op);
                fprintf(listing, "t%d", indiceT-1);
                indiceT++;
                elemento->temp = indiceT-1;
                elemento->op1Flag = 3;
                elemento->op1Num = indiceT-3;
                elemento->op2Num = indiceT-2;
                elemento->op2Flag = 3;
                insereFila(f, *elemento);
                elemento->op1Flag = -1;
                elemento->op2Flag = -1;
                elemento->temp = 0;
                elemento->tempFlag = -1;
                fprintf(listing, "\n");
              }
            }
            else if (p2->kind.expression == CallK){
              if (p2 != NULL){
                cGen(p2);
              }
              fprintf(listing, "t%d = t%d", indiceT-2);
              elemento->temp = indiceT;
              elemento->tempFlag = 0;
              indiceT++;
              showOp(tree->attr.op);
              fprintf(listing, "t%d\n", indiceT-1);
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-2;
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
            }
          } //[FIM] p1: OpK
          else if (p1->kind.expression == VariavelK || p1->kind.expression == ConstK) { //[INICIO] p1: VarK || ConstK
            if (p2->kind.expression == VariavelK || p2->kind.expression == ConstK) {
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              indiceT++;
              if (p1 != NULL){
                cGen(p1);
              }
              showOp(tree->attr.op);
              if (p2 != NULL){
                cGen(p2);
              }
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
            else if (p2->kind.expression == OpK){
              if (p2 != NULL){
                cGen(p2);
              }
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              if (p1 != NULL){
                cGen(p1);
              }
              showOp(tree->attr.op);
              fprintf(listing, "t%d", indiceT-1);
              elemento->prox = NULL;
              elemento->temp = indiceT;
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              indiceT++;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
            else if (p2->kind.expression == CallK){
              if (p2 != NULL){
                verificaCall = 2;
                cGen(p2);
              }
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              elemento->tempFlag = 0;
              if (p1 != NULL){
                cGen(p1);
              }
              showOp(tree->attr.op);
              fprintf(listing, "t%d\n", indiceT-1);
                            indiceT++;
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
            }
            else if (p2->kind.expression == VetorK){
              if (p2->child[0]->kind.expression == VariavelK || p2->child[0]->kind.expression == ConstK){
                if (p2 != NULL) {
                  ehVetor = 2;
                  cGen(p2);
                }
                fprintf(listing, "t%d = ", indiceT);
                if (p1 != NULL)
                  cGen(p1);
                elemento->temp = indiceT;
                elemento->tempFlag = 0;
                elemento->op2Flag = 3;
                elemento->op2Num = indiceT-1;
                showOp(tree->attr.op);
                fprintf(listing, "t%d", indiceT-1);
                indiceT++;
                elemento->prox = NULL;
                insereFila(f, *elemento);
                elemento->op1Flag = -1;
                elemento->op2Flag = -1;
                fprintf(listing, "\n");
              }
            }
          } //[FIM] p1: VarK || ConstK
          else if (p1->kind.expression == CallK){
            if (p1 != NULL){
              verificaCall = 2;
              cGen(p1);
            }
            if (p2->kind.expression == OpK){
              if (p2 != NULL){
                cGen(p2);
              }
              fprintf(listing, "t%d = t%d", indiceT, indiceT-2);
              elemento->temp = indiceT;
              elemento->prox = NULL;
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-1;
              showOp(tree->attr.op);
              fprintf(listing, "t%d", indiceT-1);
              elemento->op2Flag = 3;
              elemento->op2Num = indiceT-1;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              indiceT++;
              fprintf(listing, "\n");
            }
            else {
              fprintf(listing, "t%d = t%d", indiceT, indiceT-1);
              elemento->temp = indiceT;
              elemento->prox = NULL;
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-1;
              showOp(tree->attr.op);
              if (p2 != NULL)
                cGen(p2);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              indiceT++;
              fprintf(listing, "\n");
            }
          }
        }
        else if (p1 != NULL && p3 != NULL) {
          if (p3->kind.expression == OpK){
            cGen(p3);
            fprintf(listing, "t%d = ", indiceT);
            elemento->temp = indiceT;
            elemento->prox = NULL;
            cGen(p1);
            showOp(tree->attr.op);
            fprintf(listing, "t%d", indiceT-1);
            elemento->op2Flag = 3;
            elemento->op2Num = indiceT-1;
            insereFila(f, *elemento);
            elemento->op2Flag = -1;
            elemento->op1Flag = -1;
            elemento->temp = 0;
            elemento->tempFlag = -1;
            indiceT++;
            fprintf(listing, "\n");
          }
          else if (p1->kind.expression != VariavelK && p1->kind.expression != VetorK && p1->kind.expression != CallK && p3->kind.expression != OpK) {
            if (p1 != NULL) {
              cGen(p1);
            }
            if (p3->kind.expression != VariavelK && p3->kind.expression != VetorK && p3->kind.expression != CallK) {
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              fprintf(listing, "t%d", indiceT-1);
              showOp(tree->attr.op);
              indiceT++;
              if (p3 != NULL)
                cGen(p3);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");

            } else if (p3->kind.expression == VariavelK) {
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              fprintf(listing, "t%d", indiceT-1);
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-1;
              showOp(tree->attr.op);
              indiceT++;
              if (p3 != NULL)
                cGen(p3);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
          }
          else if (p1->kind.expression == VariavelK) {
            if (p3->kind.expression != VetorK && p3->kind.expression != CallK) {
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              if (p1 != NULL)
                cGen(p1);
              showOp(tree->attr.op);
              indiceT++;
              if (p3 != NULL)
                cGen(p3);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
          }
          else if (p1->kind.expression == VetorK){
            if (p3->kind.expression != VetorK && p3->kind.expression != CallK) {
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              elemento->nome = "asg";
              elemento->op2Flag = -1;
              elemento->op2Num = 0;
              elemento->prox = NULL;
              if (p1 != NULL)
                cGen(p1);
              insereFila(f, *elemento);
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              indiceT++;
              fprintf(listing, "\nt%d = t%d", indiceT, indiceT-1);
              elemento->temp = indiceT;
              elemento->op1Flag = 3;
              elemento->op1Num = indiceT-1;
              showOp(tree->attr.op);
              if (p3 != NULL)
                cGen(p3);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              elemento->tempFlag = -1;
              fprintf(listing, "\n");
            }
          }
        }
        break;
      //[FIM] Gera operação

      default:
        fprintf(listing,"BUG: Operando desconhecido");
        break;

    }
  }
}
//[FIM] Gera os nós de expressões

//[INICIO] Função que seleciona os nós da árvore e chama as suas respectivas funções
static void cGen(TreeNode * tree) {
  if (tree != NULL) {
    switch (tree->nodekind) {
    case StmtK:
      genStmt(tree);
      break;
    case DeclKi:
      genDecli(tree);
      break;
    case ExpressionK:
      genExpression(tree);
      break;
    case ParamK:
      genParam(tree);
      break;
    default:
      break;
    }
    cGen(tree->sibling);
  }
}
//[FIM] Função que seleciona os nós da árvore e chama as suas respectivas funções

//[INICIO] Função que inicializa a fila de quádruplas
void inicializaFila(fila *f) {
  f->inicio = NULL;
  f->fim = NULL;
}
//[FIM] Função que inicializa a fila de quádruplas

//[INICIO] Função que insere uma quádrupla no fim da lista
void insereFila (fila *f, cel c) {
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
//[FIM] Função que insere uma quádrupla no fim da lista

//[INICIO] Função que imprime a fila de quádruplas
void imprimeFila(fila *f){
  cel *temp;
  temp = malloc(sizeof(cel));
  *temp = *f->inicio;
  while(temp != NULL){
    fprintf(listing, "(");
    fprintf(listing, "%s, ", temp->nome);
    verificaFlag(temp->op1Flag, temp->op1Num);
    verificaFlag(temp->op2Flag, temp->op2Num);
    if (temp->tempFlag == -1){
      if (temp->temp == 0){
        fprintf(listing, "__)");
      }
      else {
        fprintf(listing, "t%d)", temp->temp);
      }
    }
    else if (temp->tempFlag == 1 || temp->tempFlag == 2){
      fprintf(listing, "%d)", temp->temp);
    }
    else if (temp->tempFlag == 0){
      fprintf(listing, "t%d)", temp->temp);
    }
    fprintf(listing, "\n");
    temp = temp->prox;
  }
}
//[FIM] Função que imprime a fila de quádruplas

//[INICIO] Função que imprime o tipo de operando de acordo com a flag
void verificaFlag (int flag, int number){
  if (flag == 0 || flag == 1 || flag == 4 || flag == 5){
    fprintf(listing, "%d, ", number);
  }
  if (flag == 2){
    fprintf(listing, "L%d, ", number);
  }
  if (flag == 3){
    fprintf(listing, "t%d, ", number);
  }
  if (flag == -1){
    fprintf(listing, "__, ");
  }
}
//[FIM] Função que imprime o tipo de operando de acordo com a flag

//[INICIO] Função que inicia a geração de código intermediário através da áŕvore sintática
void codeGen(TreeNode * syntaxTree, char * codefile) {
  char * s = malloc(strlen(codefile) + 7);
  strcpy(s, "File: ");
  posMain = 0;
  strcat(s, codefile);
  f = malloc(sizeof(fila));
  elemento = malloc(sizeof(cel));
  elemento->temp = 0;
  elemento->tempFlag = -1;
  elemento->op1Flag = -1;
  elemento->op2Flag = -1;
  inicializaFila(f);
  cGen(syntaxTree);
  fprintf(listing, "\n\n");
  imprimeFila(f);
  percorreLista();
}
//[FIM] Função que inicia a geração de código intermediário através da áŕvore sintática
