#include <stdio.h>
#include <stdlib.h>
#include "genetica.h"

void inicializaCheia(Arvore pop[], int num, int indice, int* conjuntoOpTerm,int NUM_OPBIN, int NUM_OPUN, int N, int* seed){
    int i;

    for(i = indice; i < num/3; i++){
        criaCheia(&pop[i], MAX_DEPTH/4, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
    }
    for(i = indice; i < 2*num/3; i++){
        criaCheia(&pop[i], MAX_DEPTH/2, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
    }

    for( ; i < num; i++){
        criaCheia(&pop[i], MAX_DEPTH, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
    }

}

void inicializaAleatorio(Arvore pop[], int num, int indice, int* conjuntoOpTerm,int NUM_OPBIN, int NUM_OPUN, int N, int* seed){
    int i;
    for(i = indice; i < num; i++){
        geradorArvore(&pop[i], MAX_NOS, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
    }

}

void inicializaPopulacao(Arvore pop[], int* conjuntoOpTerm, int NUM_OPBIN, int NUM_OPUN, int N, int* seed){

    inicializaAleatorio(pop, NUM_INDIV/2, 0, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
    inicializaCheia(pop, NUM_INDIV, NUM_INDIV/2, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, seed);
//    int i;
//    for(i = 0; i < NUM_INDIV/2; i++){
//        geradorArvore(&pop[i], MAX_NOS); //sortear a quantidade de n�s em determinado intervalo
//    }
//    for( ; i<NUM_INDIV; i++){
//        criaCheia(&pop[i], MAX_DEPTH); // sortear a profundidade m�xima em determinado intervalo
//    }
}


void avaliaIndividuos(Arvore pop[], float* dados[], int M, int N){

    int i, j = 0;
    float erro = 0;
    for(i = 0; i < NUM_INDIV; i++){
        //printf("i = %d \n", i);
        for(j = 0; j < M; j++){
            erro = erro + executa(&pop[i], dados[j], N);
        }
        //int k; scanf("%d", &k);
        pop[i].aptidao = erro; // + 2 * pop[i].numNos;
        erro = 0;
    }
}

//TODO: imprimir profundidade tambem
void imprimePopulacao(Arvore pop[], char** LABELS){
    int i;
    for(i = 0; i < NUM_INDIV; i++){
        imprimeArvorePre(&pop[i], LABELS);
        //imprimeArvoreNivel(&pop[i]);
        printf(" | Numero de nos: %d | Erro: %.25f |\n",pop[i].numNos, pop[i].aptidao);
    }
    printf("\n");
}

void imprimeMelhor(Arvore pop[], char** LABELS){
    int i;
    int indiceMelhor = 0;
    float melhor = pop[0].aptidao;
    for(i = 1; i < NUM_INDIV; i++){
        if(pop[i].aptidao < melhor){
            indiceMelhor = i;
            melhor = pop[i].aptidao;
        }
    }
    printf("MELHOR: ");
    imprimeArvorePre(&pop[indiceMelhor], LABELS);
    printf(" | Numero de nos: %d | Erro: %.25f |\n",pop[indiceMelhor].numNos, pop[indiceMelhor].aptidao);
    //return pop[indiceMelhor];
}

void selection(Arvore pop[], int k){
    int inicio = 0, fim = NUM_INDIV-1;

    while(inicio < fim){
        int r = inicio, w = fim;
        float meio = pop[(r+w)/2].aptidao;

        while(r < w){
            if(pop[r].aptidao <= meio){
                Arvore tmp = pop[w];
                pop[w] = pop[r];
                pop[r] = tmp;
                w--;
            } else {
                r++;
            }
        }
            if(pop[r].aptidao < meio)
                r--;

            if(k<=r){
                fim = r;
            }else{
                inicio = r+1;
            }
    }
    return;
}

void ordenaElite(Arvore pop[], int k){
    Arvore aux;
    int i, j;
    int indiceMelhor;

    for(i = 0; i < k; i++){
        indiceMelhor = i;
        for(j = i+1; j < NUM_INDIV; j++){
            if(pop[j].aptidao < pop[indiceMelhor].aptidao){
                indiceMelhor = j;
            }
        }
        aux = pop[i];
        pop[i] = pop[indiceMelhor];
        pop[indiceMelhor] = aux;
    }
}

int selecionaElite(Arvore popAtual[], Arvore popFutura[]){
    int i;
    int cont = (int)(ELITISMO*NUM_INDIV);

    if(cont%2 != 0)
        cont++;
    ordenaElite(popAtual, cont);
    for(i = 0; i < cont; i++){
        popFutura[i] = popAtual[i];
    }

    return cont;
}

void testaSelection(Arvore pop[], int k){
    //imprimePopulacao(pop);
//    ordenaElite(pop, k);
//    imprimePopulacao(pop);
}

int torneio(Arvore pop[], int* seed){
    int indiceMelhor = rand2(seed) % NUM_INDIV;
    int indice;
    int i;
    for(i = 0; i < NUM_TORNEIO-1; i++){
        indice = rand2(seed)%NUM_INDIV;
        if(pop[indice].aptidao < pop[indiceMelhor].aptidao)
            indiceMelhor = indice;
    }
    return indiceMelhor;
}

int criterioDeParada(int iteracoes){
    return iteracoes < NUM_GERACOES;
}
