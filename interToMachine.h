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

funFila *fun;
paramFila *par;

void percorreLista();

void converteParaMaquina();
