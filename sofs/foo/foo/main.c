#include <stdio.h>
#include <stdlib.h>

int vetor[2];
int foo(int x, int y, int z, int w){
    int result;
    int retorno;
    int aux;
    result = 0;
    while(result >= 0){
        if(x < 2)
            result = x + y;
        if(y > 2)
            result = result + y + z;
        else
            result = result * 3;
        if(w <= 4){
            if(z == 0)
                return 0;
            if(result != 0){
  		aux = w - 1;
		retorno = result/aux;
		return retorno;
	    }
        }
    }
}
int main(){
    int display;
    int par;
    int i;
    int j;
    int k;
    int chamada;
    scanf("%d", &par);
    i = 1;
    j = 3;
    k = 4;
    chamada = foo(i, par, j, k);
    printf("\n%d", chamada);
}

