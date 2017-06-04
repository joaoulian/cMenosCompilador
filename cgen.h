/****************************************************/
/* File: cgen.h                                     */
/* The code generator interface to the TINY compiler*/
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _CGEN_H_
#define _CGEN_H_

/* Procedure codeGen generates code to a code
 * file by traversal of the syntax tree. The
 * second parameter (codefile) is the file name
 * of the code file, and is used to print the
 * file name as a comment in the code file
 */
void codeGen(TreeNode * syntaxTree, char * codefile);

//[INICIO] Estrutura da quádrupla
typedef struct cel {
  char * nome;
  int op1Num;
  int op1Flag; //0->constante; 1->hash; 2->label; 3->temporario
  int op2Num;
  int op2Flag; //0->constante; 1->hash; 2->label; 3->temporario
  int temp;
  struct cel *prox;
} cel;
//[FIM] Estrutura da quádrupla

//[INICIO] Estrutura da fila de quádruplas
typedef struct fila {
  cel * inicio;
  cel * fim;
} fila;
//[FIM] Estrutura da fila de quádruplas

fila *f;

int posMain;

#endif
