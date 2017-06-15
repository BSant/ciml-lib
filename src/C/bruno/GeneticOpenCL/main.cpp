#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <omp.h>

//#include "options.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include "genetica.h"
#ifdef __cplusplus
}
#endif

const char *getErrorString(cl_int error) {
    switch(error){
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}

double getTime(){
    struct timeval tv;
    gettimeofday(&tv,0);
    return (double)tv.tv_sec + 1.0e-6*(double)tv.tv_usec;
}

bool IsPowerOf2( int n ){
   return (n & -n) == n;
}

unsigned NextPowerOf2( unsigned n ){
   n--;
   n |= n >> 1;  // handle  2 bit numbers
   n |= n >> 2;  // handle  4 bit numbers
   n |= n >> 4;  // handle  8 bit numbers
   n |= n >> 8;  // handle 16 bit numbers
   n |= n >> 16; // handle 32 bit numbers
   n++;

   return n;
}

std::string ToString( float t ){
      std::stringstream ss; ss << t; return ss.str();
}

void printPlatformsDevices(std::vector<cl::Platform> platforms, std::vector<cl::Device> devices){
    std::cout << "Available Platforms: \n";
    for(cl_uint i = 0; i < platforms.size(); ++i) {
        std::cout <<"\t[" << i << "]"<<platforms[i].getInfo<CL_PLATFORM_NAME>() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Available Devices for Platform " << platforms[0].getInfo<CL_PLATFORM_NAME>()<< ":\n";
    for(cl_uint i = 0; i < devices.size(); ++i) {
        std::cout<<"\t[" << i << "]"<<devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
    }
    std::cout << std::endl;
}

void setupOpenCL(std::vector<cl::Platform> &platforms, std::vector<cl::Device> &devices){
    ///Encontrando as plataformas disponiveis
    int result = cl::Platform::get(&platforms);
    if(result != CL_SUCCESS){
        std::cout << "Erro ao encontrar plataformas." << std::endl;
        exit(1);
    }

    ///Encontrando os dispositivos disponiveis na plataforma 0
    platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if(result != CL_SUCCESS){
        std::cout << "Erro ao encontrar devices." << std::endl;
        exit(1);
    }
}

void setNDRanges(size_t* globalSize, size_t* localSize, std::string* compileFlags, size_t maxLocalSize, size_t numPoints, cl_device_type deviceType){
//FOR GPU
    if(deviceType == CL_DEVICE_TYPE_GPU){
        std::cout << "Definindo NDRanges para GPU..." << std::endl;
        if(numPoints < maxLocalSize)
          *localSize = numPoints;
        else
          *localSize = maxLocalSize;

        // One individual per work-group
        *globalSize = (*localSize) * NUM_INDIV;

//        std::stringstream ss;
//        ss << NextPowerOf2( localSize );
//        std:string str = ss.str();

        (*compileFlags) += " -D LOCAL_SIZE_ROUNDED_UP_TO_POWER_OF_2="
                    + ToString( NextPowerOf2(*localSize) );

        if( MAX_NOS > (*localSize) ) //MaximumTreeSize() > m_local_size )
          (*compileFlags) += " -D PROGRAM_TREE_DOES_NOT_FIT_IN_LOCAL_SIZE";

        if( IsPowerOf2( *localSize ) )
          (*compileFlags) += " -D LOCAL_SIZE_IS_NOT_POWER_OF_2";

        if( numPoints % (*localSize) != 0 )
          (*compileFlags) += " -D NUM_POINTS_IS_NOT_DIVISIBLE_BY_LOCAL_SIZE";
    } else if (deviceType == CL_DEVICE_TYPE_CPU){
        ///FOR CPU
        std::cout << "Definindo NDRanges para CPU..." << std::endl;
        *localSize = 1;//m_num_points;
        *globalSize = NUM_INDIV;
    }
    std::cout << "...fim." << std::endl;
}

std::string setProgramSource(int NUM_OPBIN, int NUM_OPUN, int M, int N, int localSize){
    std::string program_src =
        "#define TIPO "  + ToString( TIPO ) + "\n"
        "#define SEED "  + ToString( SEED ) + "\n"
        "#define VAR  "  + ToString( VAR ) + "\n" +
        "#define CTE  "  + ToString( CTE ) + "\n" +
        "#define FBIN    "+ ToString( FBIN ) + "\n" +
        "#define FUN     "+ ToString( FUN ) + "\n" +
        "#define PLUS    "+ ToString( PLUS ) + "\n" +
        "#define MIN     "+ ToString( MIN ) + "\n" +
        "#define MULT    "+ ToString( MULT ) + "\n" +
        "#define DIV     "+ ToString( DIV ) + "\n" +
        "#define SIN     "+ ToString( SIN ) + "\n" +
        "#define COS     "+ ToString( COS ) + "\n" +
        "#define SQR     "+ ToString( SQR ) + "\n" +
        "#define EXP     "+ ToString( EXP ) + "\n" +
        "#define LOG10   "+ ToString( LOG10 ) + "\n" +
        "#define NUM_OPBIN   "+ ToString( NUM_OPBIN ) + "\n" +
        "#define NUM_OPUN   "+ ToString( NUM_OPUN ) + "\n" +
        "#define M   "+ ToString( M ) + "\n" +
        "#define N   "+ ToString( N ) + "\n" +
        "#define MAX_NOS     "+ ToString( MAX_NOS ) + "\n" +
        "#define MAX_DEPTH   "+ ToString( MAX_DEPTH ) + "\n" +
        "#define MAX_FILHOS  "+ ToString( MAX_FILHOS ) + "\n" +
        "#define NUM_INDIV   "+ ToString( NUM_INDIV ) + "\n" +
        "#define PROB_CROSS  "+ ToString( PROB_CROSS ) + "\n" +
        "#define PROB_MUT    "+ ToString( PROB_MUT ) + "\n" +
        "#define NUM_TORNEIO     "+ ToString( NUM_TORNEIO ) + "\n" +
        "#define ELITISMO        "+ ToString( ELITISMO ) + "\n" +
        "#define NUM_GERACOES    "+ ToString( NUM_GERACOES) + "\n" +
        "#define LOCAL_SIZE " + ToString( localSize ) + "\n";

    return program_src;
}

int main(int argc, char** argv){
    std::cout << std::setprecision(32) << std::fixed;
    //comparar tudo sequencial com openmp+opencl com tudo opencl

    double tempoTotalAvaliacao = 0;
    double tempoTotalEvolucao = 0;
    double tempoTotal = 0;
    double tempoGeracao, start, startIt, endIt, tempoEvolucao, tempoAvaliacao;

    int i, indice1, indice2;
    int iteracoes = 0;

    ///variaveis lidas de arquivo
    int M, N;
    char** LABELS;
    int* conjuntoOpTerm;
    float* ctes; //TODO:ainda nao le constantes...
    int NUM_OPBIN, NUM_OPUN, NUM_CTES;

    ///leituras de dados
    float** dadosTreinamento = readTrainingData(&M, &N, &NUM_CTES, &NUM_OPBIN, &NUM_OPUN, &LABELS, &conjuntoOpTerm, argv[1]);

//    for(int i = 0; i < M; i++){
//        for(int j = 0; j < N; j++){
//            std::cout << dadosTreinamento[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }

    ///transposicao de dados
    float* dadosTranspostos = new float [M*N];
    unsigned pos = 0;
    for( unsigned j = 0; j < N; ++j ){
        for( unsigned i = 0; i < M; ++i ){
            dadosTranspostos[pos++] = dadosTreinamento[i][j];
        }
    }

    imprimeParametros(M, N, NUM_CTES, NUM_OPBIN, NUM_OPUN);
    atribuiSemente(SEED);

    Arvore* popAtual = new Arvore[NUM_INDIV];
    Arvore* popFutura = new Arvore[NUM_INDIV];
    int* seeds = new int [NUM_INDIV*MAX_NOS];
    for(int i = 0; i < NUM_INDIV*MAX_NOS; i++){
        seeds[i] = rand();
    }
    inicializaPopulacao(popAtual, conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, &seeds[0]);
    avaliaIndividuos(popAtual, dadosTreinamento, M, N);

    #if AVALOCL || EVOLOCL
        cl_ulong inicio, fim;
        ///Evento para controlar tempo gasto
        cl::Event e_tempo;


        cl_int result; //Variavel para verificar erros
        ///TODO: Colocar conferencias de erros pelo c�digo
        std::vector<cl::Platform> platforms;
        std::vector<cl::Device> devices;

        setupOpenCL(platforms,devices);
        printPlatformsDevices(platforms, devices);

        ///Estabelecendo o contexto com os devices
        cl::Context contexto(devices, NULL, NULL, NULL, &result);
        if(result != CL_SUCCESS){
            std::cout << "Erro ao criar um contexto OpenCL" << std::endl;
            exit(1);
        }

        ///Criando a fila de comando
        cl_command_queue_properties commandQueueProperties = CL_QUEUE_PROFILING_ENABLE;

        ///4 op��es: GPU-GPU, GPU-CPU, CPU-CPU, CPU-GPU
        ///NUMDEVICES;FIRST

        cl::CommandQueue* cmdQueueEvol;
        cl::CommandQueue* cmdQueueAval;

        #if TWODEVICES
            #if AVALGPU
                cmdQueueEvol = new cl::CommandQueue(contexto, devices[0] /** SELECIONAR CPU */, commandQueueProperties, &result);
                cmdQueueAval = new cl::CommandQueue(contexto, devices[2] /** SELECIONAR GPU*/, commandQueueProperties, &result);
            #else
                cmdQueueEvol = new cl::CommandQueue(contexto, devices[2] /** SELECIONAR GPU*/, commandQueueProperties, &result);
                cmdQueueAval = new cl::CommandQueue(contexto, devices[0] /** SELECIONAR CPU*/, commandQueueProperties, &result);
            #endif // AVALGPU
        #else
            #if AVALGPU
                cmdQueueAval = new cl::CommandQueue(contexto, devices[2] /** SELECIONAR GPU */, commandQueueProperties, &result);
            #else
                cmdQueueAval = new cl::CommandQueue(contexto, devices[0] /** SELECIONAR CPU*/, commandQueueProperties, &result);
            #endif // AVALGPU
            cmdQueueEvol = cmdQueueAval;
        #endif // TWODEVICES

        if(result != CL_SUCCESS){
            std::cout << "Erro ao criar a Command Queue" << std::endl;
            exit(1);
        }


        cl::Buffer bufferPopA(contexto, CL_MEM_READ_WRITE, NUM_INDIV * sizeof(Arvore)/*, popAtual*/);
        cl::Buffer bufferPopF(contexto, CL_MEM_READ_WRITE/*|CL_MEM_USE_HOST_PTR*/, NUM_INDIV * sizeof(Arvore)/*, popFutura*/);
        cl::Buffer bufferOpTerm(contexto, CL_MEM_READ_ONLY, (NUM_OPBIN+NUM_OPUN+NUM_CTES+N-1)*sizeof(int));
        cl::Buffer bufferSeeds(contexto, CL_MEM_READ_WRITE, NUM_INDIV*MAX_NOS*sizeof(int));
        cl::Buffer dados(contexto, CL_MEM_READ_ONLY, M*N * sizeof(float));


        cmdQueueEvol->enqueueWriteBuffer(bufferSeeds, CL_FALSE, 0, NUM_INDIV*MAX_NOS*sizeof(int), seeds);
        cmdQueueAval->enqueueWriteBuffer(dados, CL_FALSE, 0, M*N * sizeof(float), dadosTranspostos);

        cmdQueueEvol->enqueueWriteBuffer(bufferOpTerm, CL_TRUE, 0, (NUM_OPBIN+NUM_OPUN+NUM_CTES+N-1)*sizeof(int), conjuntoOpTerm);
        #if TWODEVICES
            cmdQueueAval->enqueueWriteBuffer(bufferOpTerm, CL_TRUE, 0, (NUM_OPBIN+NUM_OPUN+NUM_CTES+N-1)*sizeof(int), conjuntoOpTerm);
        #endif // TWODEVICES


        size_t globalSize, localSize;
        size_t numPoints = M;
        std::string compileFlags;

        size_t maxLocalSize = cmdQueueAval->getInfo<CL_QUEUE_DEVICE>().getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        setNDRanges(&globalSize, &localSize, &compileFlags, maxLocalSize, numPoints, cmdQueueAval->getInfo<CL_QUEUE_DEVICE>().getInfo<CL_DEVICE_TYPE>());


        ///Leitura do arquivo com o programa em C++
        std::ifstream sourceFileName("kernelAvalia.cl");
        std::string sourceFile(std::istreambuf_iterator<char>(sourceFileName),(std::istreambuf_iterator<char>()));
        std::string program_src = setProgramSource(NUM_OPBIN, NUM_OPUN, M, N, localSize) + sourceFile;
        //std::cout << program_src << std::endl;

        ///Criar programa por Source
        cl::Program::Sources source(1, std::make_pair(program_src.c_str(), program_src.length()+1));
        cl::Program programa(contexto, source);

        //const char options[] = "-cl-opt-disable";
        //int error;
        try {
            programa.build(devices/*, options*/);
        } catch(cl::Error& e){
            std::cerr << getErrorString(e.err()) << std::endl
            << programa.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
            std::cout << e.what() << " : " << e.err() << std::endl;
            exit(1);
        }

        #if AVALGPU
            cl::Kernel krnlAvalia(programa, "avaliaIndividuos");
        #else
            cl::Kernel krnlAvalia(programa, "avaliaIndividuosCPU");
        #endif // AVALGPU

        cl::Kernel krnlEvolucao(programa, "evolucao");

    #endif // OPENCL


    while(criterioDeParada(iteracoes) /*qual o criterio de parada?*/){
        printf("\n-----------\nGERACAO %d: \n", iteracoes);
        startIt = getTime();

        #if EVOLOCL

            int novosIndividuos = selecionaElite(popAtual, popFutura);

            cmdQueueEvol->enqueueWriteBuffer(bufferPopA, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popAtual);
            cmdQueueEvol->enqueueWriteBuffer(bufferPopF, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popFutura);
            cmdQueueEvol->finish();

            //std::cout << "novos = " << novosIndividuos << std::endl;
            ///Evento para controlar tempo gasto
            //cl::Event e_tempo;
            ///Dispondo argumentos para o kernel + executar
            krnlEvolucao.setArg(0, bufferPopA);
            krnlEvolucao.setArg(1, bufferPopF);
            krnlEvolucao.setArg(2, novosIndividuos);
            krnlEvolucao.setArg(3, bufferOpTerm);
            krnlEvolucao.setArg(4, bufferSeeds);


            try {
                result = cmdQueueEvol->enqueueNDRangeKernel(krnlEvolucao, cl::NullRange, cl::NDRange((NUM_INDIV-novosIndividuos)/2), cl::NDRange(1),NULL, &e_tempo);
            } catch(cl::Error& e){
                std::cerr << getErrorString(e.err()) << std::endl;
                exit(1);
            }

            cmdQueueEvol->finish();

            e_tempo.getProfilingInfo(CL_PROFILING_COMMAND_START, &inicio);
            e_tempo.getProfilingInfo(CL_PROFILING_COMMAND_END, &fim);
            tempoEvolucao = (fim-inicio)/1.0E9;

            tempoTotalEvolucao += tempoEvolucao;
            std::cout <<"Tempo evolucao = " << tempoEvolucao << std::endl;

            ///se est� em um s� dispositivo nao precisa disso
            cmdQueueEvol->enqueueReadBuffer(bufferPopA, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popAtual);
            #if TWODEVICES || !AVALOCL
            cmdQueueEvol->enqueueReadBuffer(bufferPopF, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popFutura);
            #endif // TWODEVICES
            cmdQueueEvol->finish();

        #else // EVOLOCL

            start = getTime();
            //Sleep(1000);

            int novosIndividuos = selecionaElite(popAtual, popFutura);
            #if  EVOLOMP
            //omp_set_num_threads((NUM_INDIV)-novosIndividuos/2);
            #pragma omp parallel for num_threads((NUM_INDIV)-novosIndividuos/2) ordered
            for(int j = novosIndividuos; j < NUM_INDIV; j +=2){

            //    int num = omp_get_num_threads();
            //    printf("num threads = %d\n\n", num);
                int id = (j/2)-(novosIndividuos/2);
                //std::cout << novosIndividuos << std::endl;
                //#pragma omp ordered
                //printf("id = %d\n", id);
                //std::cout << "seeds1 = "<< id << seeds[id] << std::endl;
                indice1 = torneio(popAtual, &seeds[id]);
                indice2 = torneio(popAtual, &seeds[id]);
                //std::cout << "seeds2 = " << id<< seeds[id] << std::endl;
                //std::cout << "ind1 = " << indice1 << std::endl;
                //std::cout << "ind2 = " << indice2 << std::endl << std::endl;

                popFutura[novosIndividuos+2*id] = popAtual[indice1];
                popFutura[novosIndividuos+2*id+1] = popAtual[indice2];

                ///testar imprimir o que est� retornando na parte randomica
                float cross = randomProb(&seeds[id]);
                float mut = randomProb(&seeds[id]);
                //std::cout << "cross= " << cross << std::endl;
                //std::cout << "mut = " << mut << std::endl << std::endl;;

                if(cross <= PROB_CROSS){
                    crossOver(&popFutura[novosIndividuos+2*id+1/*-2*/], &popFutura[novosIndividuos+2*id/*-1*/], &seeds[id]);
                }
                if(mut <= PROB_MUT){
                    mutacao(&popFutura[novosIndividuos+2*id+1], conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, &seeds[id]);
                    mutacao(&popFutura[novosIndividuos+2*id], conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, &seeds[id]);
                }
            }
            #else
            for(int j = novosIndividuos; j < NUM_INDIV; j +=2){
                int id = (j/2)-(novosIndividuos/2);

                indice1 = torneio(popAtual, &seeds[id]);
                indice2 = torneio(popAtual, &seeds[id]);

                popFutura[j] = popAtual[indice1];
                popFutura[j+1] = popAtual[indice2];

                ///testar imprimir o que est� retornando na parte randomica
                float cross = randomProb(&seeds[id]);
                float mut = randomProb(&seeds[id]);

                if(cross <= PROB_CROSS){
                    crossOver(&popFutura[j+1], &popFutura[j], &seeds[id]);
                }
                if(mut <= PROB_MUT){
                    mutacao(&popFutura[j+1], conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, &seeds[id]);
                    mutacao(&popFutura[j], conjuntoOpTerm, NUM_OPBIN, NUM_OPUN, N, &seeds[id]);
                }
            }
            #endif // EVOLOMP

            tempoEvolucao = getTime() - start;
            tempoTotalEvolucao += tempoEvolucao;
            std::cout <<"Tempo evolucao = " << tempoEvolucao << std::endl;

        #endif // EVOLOCL

        #if AVALOCL
            #if TWODEVICES || !EVOLOCL
                cmdQueueAval->enqueueWriteBuffer(bufferPopF, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popFutura);
            #endif // TWODEVICES

            ///Dispondo argumentos para o kernel + executar
            krnlAvalia.setArg(0, bufferPopF);
            krnlAvalia.setArg(1, dados);
            krnlAvalia.setArg(2, NUM_INDIV * sizeof(float), NULL);

            try {
                result = cmdQueueAval->enqueueNDRangeKernel(krnlAvalia, cl::NullRange, cl::NDRange(globalSize), cl::NDRange(localSize), NULL, &e_tempo);
            } catch(cl::Error& e){
                std::cerr << getErrorString(e.err()) << std::endl;
                exit(1);
            }
            cmdQueueAval->finish();

            ///medir tempos
            e_tempo.getProfilingInfo(CL_PROFILING_COMMAND_START, &inicio);
            e_tempo.getProfilingInfo(CL_PROFILING_COMMAND_END, &fim);
            tempoAvaliacao = (fim-inicio)/1.0E9;

            tempoTotalAvaliacao += tempoAvaliacao;
            std::cout  << "Tempo de avaliacao = " << tempoAvaliacao << std::endl;

            cmdQueueAval->enqueueReadBuffer(bufferPopF, CL_TRUE, 0, NUM_INDIV * sizeof(Arvore), popFutura);
            //popFutura = (Arvore*)cmdQueueGPU.enqueueMapBuffer(bufferPop,CL_FALSE, CL_MAP_READ, 0, NUM_INDIV * sizeof(Arvore));

            cmdQueueAval->finish();


        #else // AVALOCL
            start = getTime();
            avaliaIndividuos(popFutura, dadosTreinamento, M, N);
            tempoAvaliacao = getTime() - start;
            tempoTotalAvaliacao += tempoAvaliacao;
            std::cout <<"Tempo de avaliacao = " << tempoAvaliacao << std::endl;
        #endif // AVALOCL

        for(i = 0; i< NUM_INDIV; i++){
            popAtual[i] = popFutura[i];
        }

        tempoGeracao = getTime() - startIt;
        tempoTotal += tempoGeracao;
        std::cout <<"Tempo de geracao = " << tempoGeracao << std::endl;

        imprimeMelhor(popAtual, LABELS);
        iteracoes++;
        //std::cout << "seed8 = " << seeds[8] <<std::endl;
    }

    printf("\nPOPULACAO FINAL \n");
    imprimePopulacao(popAtual, LABELS);
    printf("*");
    imprimeMelhor(popAtual, LABELS);
    printf("\n\n");

    std::cout << "Tempo total Evolucao = " << tempoTotalEvolucao << std::endl;
    std::cout << "Tempo total Avaliacao = " << tempoTotalAvaliacao << std::endl;
    std::cout << "Tempo total = " << tempoTotal << std::endl;

    return 0;
}
