#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <string.h>

static int location = 0;

static void traverse( TreeNode * t,
                      void (* preProc) (TreeNode *),
                      void (* postProc) (TreeNode *) )
{
    if (t != NULL)
    {preProc(t);
        {int i;
         for (i=0; i < MAXCHILDREN; i++)
           traverse(t->child[i],preProc,postProc);
        }
        postProc(t);
        traverse(t->sibling,preProc,postProc);
    }
}

/* nullProc is a do-nothing procedure to
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{
    if (t==NULL) return;
    else return;
}

static void typeError(TreeNode * t, char * message)
{
    fprintf(listing,"Erro de tipo na linha %d: %s\n",t->lineno,message);
    Error = TRUE;
}

static void declError(TreeNode * t, char * message)
{
    fprintf(listing,"Erro de declaracao na linha %d: %s\n",t->lineno,message);
    Error = TRUE;
}

char * cat(char * s, char * v) //concatena s e v
{
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    n = n+strlen(v);
    t = malloc(n);
    if (t==NULL)
        fprintf(listing,"Estouro de memoria na linha %d\n",lineno);
    else
    {
        strcpy(t,s);
        strcat(t,v);
    }
    return t;
}

/* Procedure insertNode inserts
 * identifiers stored in t into
 * the symbol table
 */
static void insertNode( TreeNode * t)
{
    switch (t->nodekind)
    {
    case DeclKi:
        switch (t->kind.decl)
        {
        case FunK:
            if (st_lookup(cat(t->attr.name,t->attr.name)) == -1) //Atingiu o final da lista de tratamento de colisao
                st_insert_first(cat(t->attr.name,t->attr.name), t->attr.name, t->lineno,location++,t->escopo, t->tipo, Funcao);
            else
                declError(t, "Erro: funcao ja declarada.");
            break;
        case VarK:
            if (t->tipo==Void)
            {
                declError(t, "Erro: variavel nao pode ser tipo void.");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1 )
            {
                declError(t, "Erro: variavel com mesmo nome de funcao.");
            }
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1) //correto, insere na tabela
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->tipo, Variavel);
            else
                declError(t, "Erro: variavel ja declarada.");
            break;
        case VetK:
            if (t->tipo==Void)
            {
                declError(t, "Erro: vetor nao pode ser tipo void.");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "Erro: vetor com mesmo nome de funcao.");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1){
              st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->tipo, Vetor);
              if(t->child[0] != NULL){
                if(t->child[0]->tipo==Integer){
                  location = location + (t->child[0]->attr.val);
                }
              }
            }
            else
                declError(t, "Erro: vetor ja declarado.");
            break;
        default:
            break;
        }
        break;
    case ParamK:
        switch (t->kind.param)
        {
        case DeclK:
            if (t->tipo==Void)
            {
                declError(t, "Erro: variavel nao pode ser tipo void.");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "Erro: mesmo nome usado em funcao.");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->tipo, Variavel);

            else
                declError(t, "Erro: nome ja utilizado em declaracao.");
            break;
        case DeclVK:
            if (t->tipo==Void)
            {
                declError(t, "Erro: vetor nao pode ser tipo void.");
            }
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "Erro: vetor com mesmo nome de funcao.");
            else if (st_lookup(cat(t->attr.name,t->escopo)) == -1)
                st_insert_first(cat(t->attr.name,t->escopo), t->attr.name, t->lineno,location++,t->escopo, t->tipo, Vetor);
            else
                declError(t, "Erro: nome ja utilizado em declaracao.");
            break;
        default:
            break;
        }
        break;
    case ExpressionK:
        switch (t->kind.expression)
        {
        case VariavelK:
            if (st_lookup(cat(t->attr.name,t->escopo)) == -1 && st_lookup(cat(t->attr.name,"programa")) == -1)
                declError(t, "Erro: variavel nao declarada");
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "Erro: variavel com mesmo nome de funcao.");
            else
            {
                if (st_lookup(cat(t->attr.name,t->escopo)) != -1)
                {
                    t->tipo=st_lookupTipo(cat(t->attr.name, t->escopo));
                    st_insert(cat(t->attr.name,t->escopo), t->lineno);
                }
                else if(st_lookup(cat(t->attr.name,"programa")) != -1)
                {
                    t->tipo=st_lookupTipo(cat(t->attr.name, "programa"));
                    st_insert(cat(t->attr.name,"programa"), t->lineno);
                }
            }
            break;
        case VetorK:
            if (st_lookup(cat(t->attr.name,t->escopo)) == -1 && st_lookup(cat(t->attr.name,"programa")) == -1)
                declError(t, "Erro: vetor nao declarado");
            else if (st_lookup(cat(t->attr.name,t->attr.name)) != -1)
                declError(t, "Erro: vetor com mesmo nome de funcao.");
            else
            {

                if (st_lookup(cat(t->attr.name,t->escopo)) != -1)
                {
                    t->tipo=st_lookupTipo(cat(t->attr.name, t->escopo));
                    st_insert(cat(t->attr.name,t->escopo), t->lineno);
                }
                else if(st_lookup(cat(t->attr.name,"programa")) != -1)
                {
                    t->tipo=st_lookupTipo(cat(t->attr.name, "programa"));
                    st_insert(cat(t->attr.name,"programa"), t->lineno);
                }
            }
            break;
        case CallK:
            if (st_lookup(cat(t->attr.name,t->attr.name)) == -1)
                declError(t, "Erro: funcao nao declarada.");
            else
            {
                t->tipo=st_lookupTipo(cat(t->attr.name, t->attr.name));
                st_insert(cat(t->attr.name,t->attr.name), t->lineno);
            }
            break;
        case OpK:
            if (
				(t->attr.op == LEQ) || (t->attr.op == MT) ||
				(t->attr.op == LT) || (t->attr.op == MEQ) ||
				(t->attr.op == EQ) || (t->attr.op == NEQ)
				)
                t->tipo = Boolean;
            else
                t->tipo = Integer;
            break;
        case UnaryK:
        case EqualK:
        case IdNovoK:
        default:
            break;
        }
        break;
    case StmtK:
    default:
        break;
    }
}

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
    st_insert_first(cat("input","input"), "input", 0,location++,"input", Integer, Funcao);
    st_insert_first(cat("output","output"), "output", 0,location++,"output", Void, Funcao);
    traverse(syntaxTree,insertNode,nullProc);
    if(st_lookup(cat("main","main")) == -1)
    {
        declError(syntaxTree, "Erro: main nao declarada.");
    }
    if (TraceAnalyze)
    {
        fprintf(listing,"\nTabela de símbolos:\n\n");
        printSymTab(listing);
    }
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{
    switch (t->nodekind)
    {
    case ExpressionK:
        switch (t->kind.expression)
        {
        case OpK:
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if (t->child[0]->tipo != t->child[1]->tipo)
                    typeError(t,"Erro: operacao realizada com tipos diferentes.");
            }
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if(t->child[0]->tipo!=t->tipo || t->child[0]->tipo!=t->tipo)
                {
                    typeError(t,"Erro: operacao realizada com tipos diferentes.");
                }
            }
            break;
        case UnaryK:
           if(t->attr.op == MINUS)
            {
                t->tipo = Integer;
            }
            if(t->tipo!=t->child[0]->tipo)
            {
                typeError(t,"Erro: operacao realizada com tipos diferentes.");
            }
        case EqualK:
            if(t->child[0] != NULL && t->child[1] != NULL)
            {
                if (t->child[0]->tipo != t->child[1]->tipo)
                    typeError(t,"Erro: operacao realizada com tipos diferentes.");
            }
            break;
        case VetorK:
            if(t->child[0] != NULL)
            {
                if(t->child[0]->tipo!=Integer)
                {
                    typeError(t,"Erro: esperava-se inteiro.");
                }
            }
            break;
        case ConstK:
        case VariavelK:
        case CallK:
        default:
            break;
        }
        break;
    case StmtK:
        switch (t->kind.stmt)
        {
        case IfK:
            if(t->child[0] != NULL)
            {
                if (t->child[0]->tipo != Boolean)
                    typeError(t->child[0],"Erro: teste do If nao eh um booleano");
            }
            break;
        case WhileK:
            if(t->child[0] != NULL)
            {
                if (t->child[0]->tipo != Boolean)
                    typeError(t->child[0],"Erro: teste do While nao eh um booleano");
            }
            break;
        case ReturnK:
            if (t->child[0]!=NULL)
            {
                if (t->child[0]->tipo != t->tipo)
                    typeError(t->child[0],"Erro: tipo de retorno nao correspondeo retorno da funcao");
            }
            else
            {
                if(t->tipo!=Void)
                {
                    typeError(t,"Erro: tipo de retorno nao correspondeo retorno da funcao");
                }
            }

        }
        break;
    case DeclKi:
        switch (t->kind.decl)
        {
        case VarK:
            break;
        case VetK:
            break;
        case FunK:
            break;
        default:
            break;
        }
        break;
    case ParamK:
        switch (t->kind.decl)
        {
        case DeclK:
            break;
        case DeclVK:
            break;
        default:
            break;
        }
        break;
    default:
        break;

    }
}

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
    traverse(syntaxTree,nullProc,checkNode);
}
