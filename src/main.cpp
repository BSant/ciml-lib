#include <iostream>
#include <ctime>
#include "Database.h"
#include "Grammar.h"
#include "No.h"
#include "Configures.h"
#include "Search.h"
#include "Configures.h"
#include "SimpleParser.h"
#include "../inc/ParserDerivadas.h"
#include "../inc/LeastSquareParser.h"
#include "../inc/LSDerivadasParser.h"
#include "IndividuoBuilder.h"
#include "LeastSquareIndividuoBuilder.h"
#include "../inc/RK4LSDerivadasParser.h"
#include "../inc/RK4LSIndividuoBuilder.h"
#include "Subject.h"
#include "RK4sParser.h"
#include "RK4LSDParser.h"

using namespace std;

//#define parserLeastSquare
//#define parserLSDerivadas
//#define parserDerivadas
//#define simpleParser
//#define RK4LSDerivadas
#define RK4Parser
//#define RK4LDSPaser
int main(){

#ifdef MAKEDATA
    double a, e = 0;
    int size = 200;
    for(int i = 0; i < size; i++){
        a = (double)i/100;

        cout << a << " ";

//        cout << a + b + c - d - e << endl;
        cout << cos(a) << endl;
    }

    for(int i = 0; i < size; i++)
        cout << i << " ";

    cin.get();
#endif


    srand(7566742/*time(NULL)*/);
    cout << "Hello GP-ufjf!" << endl;
    conf = new Configures();

    //set parametros
    conf->MAXDEEP = 6;

    conf->generations = 1000;
    conf->popSize = 500;
    conf->elitism = 0.1;
    conf->crossoverRate = 0.9;
    conf->mutationRate = 0.05;

    conf->NUM_THREADS = 4;

    /// Loading database and grammar

//    data = new Database("input/data/edo.dat");
//    data->loadGroup("input/data/edo1.dat");
//    grammar = new Grammar("input/grammar/edogram.txt");

//    data = new Database("input/data/rk4.dat");
//    data->loadGroup("input/data/rk4.grp");
//    grammar = new Grammar("input/grammar/rk4.dat");

//    data = new Database("input/data/ita.dat");
//    data->loadGroup("input/data/ita.grp");
//    grammar = new Grammar("input/grammar/ita.dat");

//    data = new Database("input/data/teste.dat");
//    data->loadGroup("input/data/teste.grp");
//    grammar = new Grammar("input/grammar/teste.dat");

    data = new Database("input/data/ita.dat");
    data->loadGroup("input/data/ita.grp");
    grammar = new Grammar("input/grammar/ita.dat");

//    data = new Database("input/data/cos.dat");
//    data->loadGroup("input/data/cos.grp");
//    grammar = new Grammar("input/grammar/senx.dat");

//    data = new Database("input/data/ls.dat");
//    data->loadGroup("input/data/ls.grp");
//    grammar = new Grammar("input/grammar/ls.dat");



#ifdef testLSIndividuo
 conf->numTree = data->prediction;

 Subject * s = new LeastSquareIndividuo(true);

int tam = 10;
Subject ** pop = new Subject*[tam];
    while(1){
        for(int i = 0 ; i < tam; i++)
        {
           pop[i] = s->clone();
//           LeastSquareIndividuo * l = dynamic_cast<LeastSquareIndividuo*>(s1);
//           //LeastSquareIndividuo * l = new LeastSquareIndividuo(true);
//            cout << "LeastSquareSize[0] : " << l->leastSquareSize[0] << endl;
//            pop[i] = l;
//            for(int j = 0 ; j < conf->numTree; j ++){
//                pop[i]->trees[j]->print();
//                cout << endl;
//            }
//            cout << endl;
        }
        for(int i = 0 ; i < tam; i++){
            delete pop[i];
        }
    }

#endif


#define normal


#ifdef normal

    conf->numTree = data->prediction; // seta o numero de variaveis a serem preditas. dependente do problema a ser tratado

    grammar->print();
    cin.get();
   // data->print();

    double** dados_treino = data->values;
    cout << "total training " << data->totalTraining << " total test " << data->totalTest << " total validation "<< data->totalValidation << endl;

     IndividuoBuilder * individuoBuilder = NULL;
    /// Setting parser

    #ifdef simpleParser
      SimpleParser * parser = new SimpleParser();
    #endif
    #ifdef parserGenericParser
        parser = new GenericParser();
    #endif
    #ifdef parserLeastSquare
        parser = new LeastSquareParser();
        individuoBuilder = new LeastSquareIndividuoBuilder();
    #endif
    #ifdef parserDerivadas
        parser = new ParserDerivadas();
    #endif
    #ifdef parserLSDerivadas
        parser = new LSDerivadasParser();
        individuoBuilder = new LeastSquareIndividuoBuilder();
    #endif
    #ifdef RK4LSDerivadas
        parser = new RK4LSDerivadasParser();
        individuoBuilder = new RK4LSIndividuoBuilder();
    #endif // RK4LSDerivadas
    #ifdef RK4Parser
        parser = new RK4sParser();
        individuoBuilder = new RK4LSIndividuoBuilder();
    #endif
    #ifdef RK4LDSPaser
        parser = new RK4LSDParser();
        individuoBuilder = new RK4LSIndividuoBuilder();
    #endif // RK4LDSPaser



    parser->setDataSet(data->training,data->totalTraining);




    Search* s = new Search(parser,NULL,individuoBuilder);
    s->stepByStep = false;
    s->evolve();
#endif

    return 0;
}
