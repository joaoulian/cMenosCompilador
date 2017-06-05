typedef struct funCel {
  int hash;
  int posicaoInicio;
  int posicaoFim;
  struct funCel *prox;
} funCel;

typedef struct funFila {
  funCel * inicio;
  funCel * fim;
} funFila;

funFila *fun;

typedef struct paramCel {
  int tipo;
  int valor;
  struct paramCel *prox;
} paramCel;

typedef struct paramFila {
  paramCel * inicio;
  paramCel * fim;
} paramFila;

typedef struct ifCel {
  int linhaChamada;
  int regResult;
  int label;
  struct ifCel *prox;
} ifCel;

typedef struct ifFila {
  ifCel * inicio;
  ifCel * fim;
} ifFila;

funFila *fun;
paramFila *par;
ifFila *filaIf;

void percorreLista();

void converteParaMaquina();
