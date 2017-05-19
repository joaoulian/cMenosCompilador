#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include "interToMachine.h"

static void cGen(TreeNode * tree);

cel *elemento;

int indicadorLinha = 0;
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
      cGen(p1);
      linhaWhile[whileCorrente] = indicadorLinha;
      elemento->nome = "lab";
      elemento->op1Flag = 2;
      elemento->op1Num = indicadorLinha;
      elemento->op2Flag = -1;
      elemento->temp = 0;
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op2Flag = -1;
      elemento->op1Flag = -1;
      elemento->temp = 0;
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
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->temp = 0;
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
      elemento->prox = NULL;
      insereFila(f, *elemento);
      elemento->op2Flag = -1;
      elemento->op1Flag = -1;
      elemento->temp = 0;
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
        insereFila(f, *elemento);
        elemento->op1Flag = -1;
        elemento->op2Flag = -1;
        elemento->temp = 0;
        fprintf(listing, "\n");
      }
      else {
        fprintf(listing, "return ");
        elemento->nome = "ret";
        if (p1 != NULL)
          cGen(p1);
        elemento->op2Flag = -1;
        elemento->op2Num = 0;
        elemento->prox = NULL;
        elemento->temp = 0;
        insereFila(f, *elemento);
        elemento->op2Flag = -1;
        elemento->op1Flag = -1;
        fprintf(listing, "\n");
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
        elemento->op1Num = hash(tree->attr.name);
        elemento->op2Flag = -1;
        elemento->op2Num = 0;
        elemento->temp = 0;
        elemento->prox = NULL;
        insereFila(f, *elemento);
        elemento->op1Flag = -1;
        elemento->op2Flag = -1;
        elemento->temp = 0;
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
        else {
          elemento->op2Num = tree->attr.val;
          elemento->op2Flag = 0;
        }
        break;
      //[FIM] Gera constante

      //[INICIO] Gera variavel
      case VariavelK:
        if (tree != NULL)
          fprintf(listing, "%s", tree->attr.name);
        if (elemento->op1Flag == -1){
          elemento->op1Num = buscaMemoria(tree->attr.name);
          elemento->op1Flag = 1;
        }
        else {
          elemento->op2Num = buscaMemoria(tree->attr.name);
          elemento->op2Flag = 1;
        }
        break;
      //[FIM] Gera variavel

      //[INICIO] Gera chamada de função
      case CallK:
         contCall2 = indicadorLinha;
         int temp = 0;
         TreeNode * other;
         if (tree != NULL && tree->child[0] != NULL)
           other = tree->child[0];
         while (other != NULL) {
           temp++;
           if (other->kind.expression == VariavelK) {
             fprintf(listing, "param: ");
             elemento->nome = "par";
             elemento->op1Flag = 1;
             elemento->op1Num = buscaMemoria(other->attr.name);
             elemento->op2Flag = -1;
             elemento->op2Num = 0;
             elemento->prox = NULL;
             elemento->temp = 0;
             insereFila(f, *elemento);
             elemento->op1Flag = -1;
             fprintf(listing, "%s ", other->attr.name);
           }
           else {
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
               elemento->prox = NULL;
               insereFila(f, *elemento);
               elemento->temp = 0;
               elemento->op2Flag = -1;
               elemento->op1Flag = -1;
             }
           }
           fprintf(listing, "\n");
           other = other->sibling;
         }
         if (verificaCall == 0)
           fprintf(listing, "call %s, %d\n", tree->attr.name, temp);
           elemento->nome = "cal";
           elemento->op1Flag = 1;
           elemento->op1Num = buscaMemoria(tree->attr.name);
           elemento->op2Flag = 0;
           elemento->op2Num = temp;
           elemento->temp = 0;
           elemento->prox = NULL;
           insereFila(f, *elemento);
           elemento->op2Flag = -1;
           elemento->op1Flag = -1;
           elemento->temp = 0;
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
           indiceT++;
         }

         verificaCall = 0;
         break;
      //[FIM] Gera chamada de função

      //[INICIO] Gera vetor
      case VetorK:
        if (tree->child[0] != NULL) {
          if (tree->child[0]->nodekind == CallK || tree->child[0]->nodekind == OpK) {
            if (tree->child[0] != NULL) {
              cGen(tree->child[0]);
            }
            fprintf(listing, "%s[t%d]", tree->attr.name, indiceT-1);
          }
          else {
            fprintf(listing, "%s[", tree->attr.name);
            elemento->op1Flag = 1;
            elemento->op1Num = buscaMemoria(tree->attr.name);
            if (tree->child[0] != NULL)
              cGen(tree->child[0]);
            fprintf(listing, "]");
          }
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
              if (p1 != NULL)
                cGen(p1);
              elemento->nome = "asg";
              fprintf(listing, " = t%d\n", indiceT-1);
              elemento->temp = indiceT-1;
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
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
            }
          }
          else if (p2->kind.expression == VariavelK || p2->kind.expression == ConstK) {
            if (p1 != NULL)
              cGen(p1);
            elemento->nome = "asg";
            elemento->temp = 0;
            fprintf(listing, " = ");
            if (p2 != NULL)
              cGen(p2);
            insereFila(f, *elemento);
            elemento->op1Flag = -1;
            elemento->op2Flag = -1;
            fprintf(listing, "\n");
          }
          else if (p2->kind.expression == CallK) {
            verificaCall = 1;
            if (p2 != NULL)
              cGen(p2);
            int temp = 0;
            TreeNode * other = p2->child[0];
            while (other != NULL) {
              temp++;
              other = other->sibling;
            }
            fprintf(listing, "t%d", indiceT);
            elemento->nome = "cal";
            elemento->op1Flag = 1;
            elemento->op1Num = buscaMemoria(p2->attr.name);
            elemento->op2Flag = 0;
            elemento->op2Num = temp;
            elemento->temp = indiceT;
            elemento->prox = NULL;
            insereFila(f, *elemento);
            elemento->op1Flag = -1;
            elemento->op2Flag = -1;
            elemento->temp = 0;
            fprintf(listing, " = ");
            fprintf(listing, "call %s, %d\n", p2->attr.name, temp);
            if (p1 != NULL)
              cGen(p1);
            elemento->nome = "asg";
            elemento->op2Flag = 3;
            elemento->op2Num = indiceT;
            elemento->prox = NULL;
            elemento->temp = 0;
            insereFila(f, *elemento);
            elemento->temp = 0;
            elemento->op2Flag = -1;
            elemento->op1Flag = -1;
            fprintf(listing, " = t%d", indiceT);
            indiceT++;
            fprintf(listing, "\n");
            verificaCall = 0;
          }
          else if (p2->kind.expression == VetorK) {
            if (p1->kind.expression == VariavelK || p1->kind.expression == VetorK){
              fprintf(listing, "t%d = ", indiceT);
              elemento->temp = indiceT;
              elemento->nome = "asg";
              if (p2 != NULL)
                cGen(p2);
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op1Flag = -1;
              elemento->op2Flag = -1;
              elemento->temp = 0;
              fprintf(listing, "\n");
              if (p1 != NULL)
                cGen(p1);
              fprintf(listing, " = t%d", indiceT);
              elemento->nome = "asg";
              elemento->temp = indiceT;
              elemento->prox = NULL;
              insereFila(f, *elemento);
              elemento->op2Flag = -1;
              elemento->op1Flag = -1;
              indiceT++;
              fprintf(listing, "\n");
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
          if (p1->kind.expression == ConstK) {
            fprintf(listing, "t%d = ", indiceT);
            elemento->temp = indiceT;
            indiceT++;
            if (p1 != NULL) {
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
            fprintf(listing, "\n");
          }
          if (p1->kind.expression != VariavelK && p1->kind.expression != VetorK && p1->kind.expression != CallK && p1->kind.expression != ConstK) {
            if (p1 != NULL) {
              cGen(p1);
            }
            if (p2 != NULL){
              cGen(p2);
            }
            indiceT++;
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
            fprintf(listing, "\n");
          } else if (p1->kind.expression == VariavelK) {
            if (p2->kind.expression != VetorK && p1->kind.expression != CallK) {
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
              fprintf(listing, "\n");
            }
          }
        } else if (p1 != NULL && p3 != NULL) {
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
    if (temp->temp == 0){
      fprintf(listing, "__)");
    }
    else {
      fprintf(listing, "t%d)", temp->temp);
    }
    fprintf(listing, "\n");
    temp = temp->prox;
  }
}
//[FIM] Função que imprime a fila de quádruplas

//[INICIO] Função que imprime o tipo de operando de acordo com a flag
void verificaFlag (int flag, int number){
  if (flag == 0 || flag == 1){
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
  strcat(s, codefile);
  f = malloc(sizeof(fila));
  elemento = malloc(sizeof(cel));
  elemento->temp = 0;
  elemento->op1Flag = -1;
  elemento->op2Flag = -1;
  inicializaFila(f);
  cGen(syntaxTree);
  fprintf(listing, "\n\n");
  imprimeFila(f);
  percorreLista();
}
//[FIM] Função que inicia a geração de código intermediário através da áŕvore sintática
