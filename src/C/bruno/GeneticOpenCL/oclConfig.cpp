#include "oclConfig.h"

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

    ///Encontrando os dispositivos disponiveis na plataforma 0.
    #if AVALGPU
        platforms[2].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    #else
        platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    #endif

    if(result != CL_SUCCESS){
        std::cout << "Erro ao encontrar devices." << std::endl;
        exit(1);
    }
}

void setupOpenCL2(std::vector<cl::Platform> &platforms, std::vector<cl::Device> &devicesCPU, std::vector<cl::Device> &devicesGPU){
    ///Encontrando as plataformas disponiveis
    int result = cl::Platform::get(&platforms);
    if(result != CL_SUCCESS){
        std::cout << "Erro ao encontrar plataformas." << std::endl;
        exit(1);
    }

    ///Encontrando os dispositivos disponiveis na plataforma 0
    platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devicesCPU);
    platforms[2].getDevices(CL_DEVICE_TYPE_ALL, &devicesGPU);
    ///AQUI
    if(result != CL_SUCCESS){
        std::cout << "Erro ao encontrar devices." << std::endl;
        exit(1);
    }

}

void setNDRanges(size_t* globalSize, size_t* localSize, std::string* compileFlags, size_t maxLocalSize, size_t numPoints, cl_device_type deviceType){
//FOR GPU
    if(deviceType == CL_DEVICE_TYPE_GPU){
        std::cout << "Definindo NDRanges para GPU..." << std::endl;
        if(numPoints < maxLocalSize){
                //std::cout << "AQUI??????" <<std::endl;
          *localSize = numPoints;
        }else{
            //std::cout << "AQUI!!!!!!" <<std::endl;
          *localSize = maxLocalSize;
        }

        // One individual per work-group
        *globalSize = (*localSize) * NUM_INDIV;

//        std::stringstream ss;
//        ss << NextPowerOf2( localSize );
//        std:string str = ss.str();

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
        (*compileFlags) += " -D LOCAL_SIZE_ROUNDED_UP_TO_POWER_OF_2="
                        + ToString( NextPowerOf2(*localSize) );
        std::cout << "...fim." << std::endl;
}

std::string setProgramSource(int NUM_OPBIN, int NUM_OPUN, int M, int N, int localSize, float maxDados, float minDados){
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
        "#define maxDados    "+ ToString( maxDados ) + "\n" +
        "#define minDados    "+ ToString( minDados) + "\n" +
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
        //std::cout <<ToString( maxDados ) << std::endl;
        //std::cout <<maxDados<< std::endl;
    return program_src;
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
      std::stringstream ss; ss << std::setprecision(32) << t; return ss.str();
}

