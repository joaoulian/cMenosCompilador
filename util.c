#include "globals.h"
#include "util.h"
#include "cmenos.tab.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
      fprintf(listing,
         "reserved word: %s\n",tokenString);
      break;
    case LT: 		fprintf(listing,"<\n"); break;
    case MT: 		fprintf(listing,">\n"); break;
    case ATTR:	 		fprintf(listing,"=\n"); break;
    case LPAREN: 		fprintf(listing,"(\n"); break;
    case RPAREN: 		fprintf(listing,")\n"); break;
    case SEMI: 			fprintf(listing,";\n"); break;
    case PLUS: 			fprintf(listing,"+\n"); break;
    case MINUS: 		fprintf(listing,"-\n"); break;
    case TIMES: 		fprintf(listing,"*\n"); break;
    case OVER: 		fprintf(listing,"/\n"); break;
    case V: 			fprintf(listing,",\n"); break;
    case EQ: 			fprintf(listing,"==\n"); break;
    case LEQ: 	fprintf(listing,"<=\n"); break;
    case MEQ: 	fprintf(listing,">=\n"); break;
    case NEQ: 		fprintf(listing,"!=\n"); break;
    case LCOL: 	fprintf(listing,"[\n"); break;
    case RCOL: 	fprintf(listing,"]\n"); break;
    case LCH: 		fprintf(listing,"{\n"); break;
    case RCH: 		fprintf(listing,"}\n"); break;
    case ENDFILE: 		fprintf(listing,"EOF\n"); break;
    case NUM:

      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "ERROR: %s\n",tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else{
        for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

/* Function newExpressionNode creates another expression
 * node for syntax tree construction
 */
TreeNode * newExpressionNode(ExpressionKind kind)
{
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));

    int i;
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
    {
        for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpressionK;
        t->kind.expression = kind;
        t->lineno = lineno;
        t->tipo = Void;
    }
    return t;
}

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */

TreeNode * newDeclNode(DeclKind kind)
{
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
    {
        for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DeclKi;
        t->kind.decl = kind;
        t->lineno = lineno;
	t->tipo = Void;
    }
    return t;
}



/* Function newParamNode creates a new param
 * node for syntax tree construction
 */
TreeNode * newParamNode(ParamKind kind)
{
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    int i;
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else
    {
        for (i=0; i<MAXCHILDREN; i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ParamK;
        t->kind.param = kind;
        t->lineno = lineno;
        t->tipo = Void;
    }
    return t;
}


/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{
    int n;
    char * t;
    if (s==NULL) return NULL;
    n = strlen(s)+1;
    t = malloc(n);
    if (t==NULL)
        fprintf(listing,"Out of memory error at line %d\n",lineno);
    else strcpy(t,s);
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
    int i;
    for (i=0; i<indentno; i++)
        fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{
    int i;
    INDENT;
    while (tree != NULL){
	printSpaces();
        if (tree->nodekind==StmtK){
            switch (tree->kind.stmt){
            case IfK:
                fprintf(listing,"If\n");
                break;
            case WhileK:
                fprintf(listing,"While\n");
                break;
            case ReturnK:
                fprintf(listing,"Return\n");
                break;
            default:
                fprintf(listing,"Unknown StmtNode kind\n");
                break;
            }
        }
        else if (tree->nodekind==DeclKi)
        {
            switch (tree->kind.decl)
            {
            case VarK:
                fprintf(listing,"Declaracao Variavel: %s\n",tree->attr.name);
                break;
            case VetK:
                fprintf(listing,"Declaracao Vetor: %s\n",tree->attr.name);
                break;
            case FunK:
                fprintf(listing,"Funcao: %s\n",tree->attr.name);
                break;
            default:
                fprintf(listing,"Unknown DeclNode kind\n");
                break;
            }
        }
        else if (tree->nodekind==ParamK)
        {
            switch (tree->kind.param)
            {
            case DeclK:
                fprintf(listing,"Parametro: %s \n",tree->attr.name);
                break;
	    case DeclVK:
                fprintf(listing,"Vetor: %s \n",tree->attr.name);
                break;
            default:
                fprintf(listing,"Unknown ParamNode kind\n");
                break;
            }
        }
        else if (tree->nodekind==ExpressionK)
        {
            switch (tree->kind.expression)
            {
            case EqualK:
                fprintf(listing,"Atribuicao =\n");
                break;
            case ConstK:
                fprintf(listing,"Const\n");
                break;
            case OpK:
                fprintf(listing,"Op: ");
                printToken(tree->attr.op,"\0");
                break;
            case UnaryK:
                fprintf(listing,"Unary: ");
                printToken(tree->attr.op,"\0");
                break;
            case VariavelK:
                fprintf(listing,"Variavel: %s \n",tree->attr.name);
                break;
            case VetorK:
                fprintf(listing,"Vetor: %s \n",tree->attr.name);
                break;
            case CallK:
                fprintf(listing,"Chamada de funcao: %s\n",tree->attr.name);
                break;
            case IdNovoK:
                break;
            default:
                fprintf(listing,"Unknown ExpressionNode kind\n");
                break;
            }
        } else {
	    fprintf(listing,"Unknown node kind\n");
	}
        for (i=0; i<MAXCHILDREN; i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
