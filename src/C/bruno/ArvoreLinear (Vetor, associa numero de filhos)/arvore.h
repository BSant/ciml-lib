#ifndef ARVORE_H_INCLUDED
#define ARVORE_H_INCLUDED

#include "pilha.h"
#include "utilitarios.h"
#include "bitwise.h"

typedef struct{
    int numeroFilhos[MAX_NOS];
    int informacao[MAX_NOS];
    int numNos;
    float aptidao;
}Arvore;



void inicializaArvore(Arvore* arv);//Inicializar todos os n�s da arvore com um valor flag (verificar necessidade)
int calculaTamanhoSubArvore(Arvore* arv, int indice);//dado o indice de um n�, retorna o tamanho da sub�rvore a partir dele

void geradorArvore(Arvore* arv, int maxTam);//preenche uma �rvore com um numero m�ximo de n�s 'maxTam'
void criaCheia(Arvore* arv, int maxDepth);//dada uma profundidade maxima, preenche a arvore ate essa profundidade

void imprimeArvoreNivel(Arvore* arv);//Metodo para imprimir a �rvore na estrutura de n�veis
void imprimeArvorePre(Arvore* arv);//Imprime a �rvore em pr� ordem no modelo: 1filho= (1(0)), 2 filhos=(2(0)(0))
void imprimeArvorePos(Arvore* arv);//Imprime a �rvore em p�s ordem no modelo: 1filho= ((0)1), 2 filhos=((0)(0)2)


void shift(Arvore* arv, int tam, int indice); //Desloca os elementos da �rvore a partir de um 'indice' em determinado tamanho
void mutacao(Arvore* arvore); //Sorteia uma sub-arvore aleat�ria e troca por outra
void crossOver(Arvore* arvore1, Arvore* arvore2); //Troca informa��es entre duas arvores

//TODO: conferir a melhor forma de fazer
void trocaSubArv(Arvore* arvMaior,Arvore* arvMenor,int ind1,int ind2,int tamanhoSubMenor, int tamanhoSubMaior);

void imprimeSinxate(int info);

float executa(Arvore* individuo, float dados[]);

float fazConta(int k, float vet[]);
float opBinaria(int operador, float valor1, float valor2);
float opUnaria(int k, float valor1);

void generate(Arvore* arv,int min, int max);

void criaArvTeste(Arvore* arvore1);//Cria uma �rvore de forma conhecida para testes.
void testaPrint(Arvore* arvore); //Testa as tr�s fun��es de impress�o
void realizaTestes();//testa algumas fun��es da �rvore
void testaExecuta(float* dados[]);

#endif // ARVORE_H_INCLUDED
