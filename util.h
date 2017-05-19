#ifndef _UTIL_H_
#define _UTIL_H_

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken( TokenType, const char* );

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind);

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind);

/* Function newParamNode creates a new param
 * node for syntax tree construction
 */
TreeNode * newParamNode(ParamKind);

/* Function newExpressionNode creates a new expression
 * node for syntax tree construction
 */
TreeNode * newExpressionNode(ExpressionKind);

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString( char * );

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * );

#endif
