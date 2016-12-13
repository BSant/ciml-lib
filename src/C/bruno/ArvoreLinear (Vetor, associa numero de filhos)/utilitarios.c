#include "utilitarios.h"
#include "constantes.h"


void atribuiSemente(int i){
    srand(i);
}

float randomProb(){
    return rand()/(float)(RAND_MAX+1);
}

int randomInterval(int inferior, int superior){
    return rand() % (superior - inferior + 1) + inferior;
}

int randomSuperior(int limiteSuperior){
    if(limiteSuperior == 0) return 0;
    return rand() % (limiteSuperior + 1);
}

int randomTerminal(){
    int valor = rand() % (2*(N-1));
    return valor;
}


int randomFunctionBin(){
    return rand() % NUM_OPBIN;
}

int randomFunctionUn(){
    return rand() % NUM_OPUN;
}

//TODO(-): selecionar constante aleatoria de acordo com o maior e menor valor dos dados

float randomConst(){
    return (rand()/(float)(RAND_MAX));
}

int randomType(){
    int tipoAleatorio = rand()%(NUM_OPBIN+NUM_OPUN+(1)+N-1);
    return tipoAleatorio;
}


int randomLeafType(){
    int tipoAleatorio = (rand()%N) + NUM_OPBIN + NUM_OPUN;
    return tipoAleatorio;
}

int randomNodeType(){
    int tipoAleatorio = (rand()%(NUM_OPBIN+NUM_OPUN));
    return tipoAleatorio;
}


void fatal(char *msg) {
    printf("%s\n", msg);
    exit (1);
}


float** readTrainingData(){
    FILE *arq;
    float** dadosTreinamento;
    int i = 0, j;
    int k;

//    char nome[100];
//    printf("\nDigite nome do arquivo:");
//    gets(nome);
//    arq = fopen(nome, "r");

    arq = fopen("dadosTreinamento4.txt", "r");
    int numLinhas, numColunas, boolLabel;
    fscanf(arq, "%d %d %d", &numLinhas, &numColunas, &boolLabel);//, &numVariaveis);

    M = numLinhas;
    N = numColunas;
//    printf("\nN = %d\n\n", N);
//    printf("\nM = %d\n\n", M);

    dadosTreinamento = malloc(M* sizeof(float*));
    for(k = 0; k < M; k++){
        (dadosTreinamento)[k] = malloc(N*sizeof(float));
    }

    //float dadosTreinamento[M][N];

    LABELS = malloc(numColunas*sizeof(char*));

    if(boolLabel){
        char *word = NULL;
        size_t size = 2;
        long fpos;
        fpos = ftell(arq);
        //printf("pos = %d\n", fpos);
        char format [32];
        //char* labels[n];

        if ((word = malloc(size)) == NULL)                  // word memory
            fatal("Failed to allocate memory");
        sprintf (format, "%%%us", (unsigned)size-1);        // format for fscanf''

        while((fscanf(arq, format, word) == 1) && (i < N)) {
            while (strlen(word) >= size-1) {                // is buffer full?
                size *= 2;                                  // double buff size
                //printf ("** doubling to %u **\n", (unsigned)size);
                if ((word = realloc(word, size)) == NULL)
                    fatal("Failed to reallocate memory");
                sprintf (format, "%%%us", (unsigned)size-1);// new format spec
                fseek(arq, fpos, SEEK_SET);                  // re-read the line
                if (fscanf(arq, format, word) == 0)
                    fatal("Failed to re-read file");

       // printf ("lido2= %s\n", word);
            }
            //printf("pos = %d\n", fpos);
            LABELS[i] = (char*)malloc((size));

            strcpy(LABELS[i], word);
            i = i + 1;

           // printf ("%s\n", word);
            //printf ("%s\n", LABELS[i]);
            fpos = ftell(arq);                               // mark file pos
        }
        fseek(arq, fpos, SEEK_SET);
        //printf("pos = %d\n", fpos);
        free(word);
    } else {
//        for(k = 0; k < N; k++){
//            (LABELS)[k] = (char*)malloc(10*sizeof(char));
//            (LABELS)[k][0] = 'X';
//            (LABELS)[k][1] = '\0';
//
//            strcat(LABELS[k], "k");
        //}
    }

//    for(i = 0; i < numColunas; i++){
//        printf ("%s\n", LABELS[i]);
//    }

    for(i = 0; i < numLinhas; i++){
        for(j = 0; j < numColunas; j++){
                //printf("a\n");
            if(!fscanf(arq, "%f", &dadosTreinamento[i][j]))
                break;
            //printf("%f ",(dadosTreinamento)[i][j]);
        }
    }

    int boolOps, numOpBin, numOpUn;
    int tipo;
    int info;
    //int j;

    fscanf(arq, "%d", &boolOps);
    if(boolOps){
        fscanf(arq, "%d %d", &numOpBin, &numOpUn);
        NUM_OPBIN = numOpBin;
        NUM_OPUN = numOpUn;

        //operacoesBin = malloc(numOpBin * sizeof(int));
        conjuntoOpTerm = malloc((NUM_OPBIN+NUM_OPUN+(1)+N-1) * sizeof(int));

        for (i = 0; i < NUM_OPBIN; i++){
            if(!fscanf(arq, "%d", &info))
                break;
            conjuntoOpTerm[i] = packInt(FBIN, info);
        }

//        if(numOpUn!= 0){
            for (j = 0 ; j < NUM_OPUN; j++, i++){
                if(!fscanf(arq, "%d", &info))
                    break;
                conjuntoOpTerm[i] = packInt(FUN, info);
            }
//        }

        for(j = 0; j < N-1; j++, i++){
            conjuntoOpTerm[i] = packInt(VAR, j);
        }

        conjuntoOpTerm[i] = packFloat(CONST, 1.0);

    } else {
        NUM_OPBIN = 4;
        NUM_OPUN = 3;
        conjuntoOpTerm = malloc((NUM_OPBIN+NUM_OPUN+(1)+N-1) * sizeof(int));

        for (i = 0; i < NUM_OPBIN; i++){
            conjuntoOpTerm[i] = packInt(FBIN, i);
        }
        for (j = 0 ; j < NUM_OPUN; j++, i++){
            conjuntoOpTerm[i] = packInt(FUN, i);
        }
        for(j = 0; j < N-1; j++, i++){
            conjuntoOpTerm[i] = packInt(VAR, j);
        }
        conjuntoOpTerm[i] = packFloat(CONST, 1.0);
    }
    fclose(arq);

    return dadosTreinamento;

}


float proDiv(float num, float div){
    if(div == 0){
        return 1;
    } else {
        return (num/div);
    }
}

float infDiv(float num, float div){
    if(div == 0){
        return INFINITY;
    } else {
        return (num/div);
    }
}

float proSqrt(float num){
    if(num < 0){
        return 1;
    } else {
        return sqrt(num);
    }
}

float infSqrt(float num){
    if(num < 0){
        return INFINITY;
    } else {
        return sqrt(num);
    }
}
