#include "./src/parameters.h"
#include "./src/representation.h"
#include "./src/kernels/utils.cpp"

//#pragma OPENCL EXTENSION cl_khr_fp64: enable

__kernel void initPopulation(__global unsigned *pop, 
						     __global int * D_seeds, 
					         __constant struct KernelParameters* parameters){
	
	int tid = get_global_id(0);	
	
	if(tid < parameters->POP_SIZE * parameters->REAL_LEN){
    
	    int seed = D_seeds[tid];	        
	    unsigned word = 0;    
	    
	    for(uint j=0; j < BITS_PER_WORD; j++){
	        
        	word = word << 1;     	
  			
  			if(u_rand(&seed) <= 0.5){
  				word = word | 1;
  			}
	    }	    
	    pop[tid] = word;    
	    D_seeds[tid] = seed;
    }
}


int BinaryToDecimal(__local int *binary, int begin, int end, __local float *partialSum, int localID, int localSize){

    int i, n=1, sum=0;
    
    for(i=end-1; i>=begin; i--, n=n<<1){
    	sum += n*binary[i];
    }
    
	return sum;
}

float EvaluateIndividual_local(__local unsigned *individual, int lid, int localSize, __constant struct KernelParameters* parameters)
{
    __local float partial[128];
    partial[lid] = 0;

    int i,j;
	
	//Genotype decoding....

    #ifdef ONEMAX
        int idx, m;
	    unsigned aux, shift;
	    int value;			
	
	    //if(localSize <= parameters->REAL_LEN){

		    for(int idx=lid; idx < parameters->REAL_LEN; idx+=localSize) {
		     	aux = individual[idx];
			    shift = MASK;
		
			    for(int j=0;j<BITS_PER_WORD; j+=1) {

			       value = (aux & shift) == 0 ? 0 : 1;
			       shift = shift >> 1;
			       partial[lid] += value;		   
			    }	
		    }

	    barrier(CLK_LOCAL_MEM_FENCE); 

	    idx = get_local_size(0)/2;
	    while (idx!=0) {
	       	if (lid < idx)
	      		partial[lid] += partial[lid + idx]; 
	       	barrier(CLK_LOCAL_MEM_FENCE);
		    idx = idx/2;
	    }
    #endif

    #ifndef ONEMAX	
    
        __local float phenotype[1024];
    
	    for(int k = lid; k < parameters->DIMENSIONS; k+=localSize){

		    phenotype[k] = 0;

		    int index = (k * parameters->BITS_PER_DIMENSION) / BITS_PER_WORD;
		    int val = individual[index];		  
		    int start = k * parameters->BITS_PER_DIMENSION;
		    int stop = start + parameters->BITS_PER_DIMENSION;
		    float bitpow;

		    int tp = val >> start;
		    int count = 0, mask = 1;

	        for(int j = start; j < stop; j++) {

	      		// test for current bit 0 or 1
		        if((tp & mask) == 1) {
		            bitpow = powr(2.0, count);
		            phenotype[k] += bitpow;
		        }
		        tp = tp >> 1;
		        count++;
		    }

            phenotype[k] = parameters->LOWER_LIM + (((parameters->UPPER_LIM - parameters->LOWER_LIM)/(native_powr(2.0f, parameters->BITS_PER_DIMENSION)-1)*phenotype[k]));
	    }
	
	    barrier(CLK_LOCAL_MEM_FENCE); 	
	
	    partial[lid] = 0;
	    //Objective function!
	    for(int idx=lid; idx < SUM_LIMIT(parameters->DIMENSIONS); idx+= localSize){		
            partial[lid] += OBJECTIVE_FUNCTION(idx, phenotype);
	    }
		
	    //Local parallel reduction
		
	    barrier(CLK_LOCAL_MEM_FENCE); 

        i = localSize/2;
        while (i!=0) {
           	if (lid < i)
	      		partial[lid] += partial[lid + i]; 
           	barrier(CLK_LOCAL_MEM_FENCE);
		    i = i/2;
        }
    #endif

    barrier(CLK_LOCAL_MEM_FENCE); 
       
    return -partial[0];	
}


__kernel void Evaluation(__global unsigned *pop,
					  __global float *fitness, 
					  __constant struct KernelParameters *parameters){
	
	__local float partial[128];	
	
	int gid = get_group_id(0); 
	int localID = get_local_id(0);
	int localSize = get_local_size(0);
	partial[localID] = 0;
	
	int i,j;

    //Genotype decoding....	

    #ifdef ONEMAX
        int idx, m;
	    unsigned aux, shift;
	    int value;			
	
	    //if(localSize <= parameters->REAL_LEN){

		    for(int idx=localID; idx < parameters->REAL_LEN; idx+=localSize) {
		     	aux = pop[gid*parameters->REAL_LEN + idx];
			    shift = MASK;
		
			    for(int j=0;j<BITS_PER_WORD; j+=1) {

			       value = (aux & shift) == 0 ? 0 : 1;
			       shift = shift >> 1;
			       partial[localID] += value;		   
			    }	
		    }
    #endif

    #ifndef ONEMAX

        __local float phenotype[1024];

        for(int k = localID; k < parameters->DIMENSIONS; k+=localSize){

		    phenotype[k] = 0;

		    int index = (k * parameters->BITS_PER_DIMENSION) / BITS_PER_WORD;
		    int val   = pop[gid*parameters->REAL_LEN + index];		  
		    int start = k * parameters->BITS_PER_DIMENSION;
		    int stop  = start + parameters->BITS_PER_DIMENSION;
		    float bitpow;

		    int tp = val >> start;
		    int count = 0, mask = 1;

	        for(int j = start; j < stop; j++) {

	      		// test for current bit 0 or 1
		        if((tp & mask) == 1) {
		            bitpow = powr(2.0, count);
		            phenotype[k] += bitpow;
		        }
		        tp = tp >> 1;
		        count++;
		    }

            phenotype[k] = parameters->LOWER_LIM + (((parameters->UPPER_LIM - parameters->LOWER_LIM)/(native_powr(2.0f, parameters->BITS_PER_DIMENSION)-1)*phenotype[k]));
	    }
	
	    barrier(CLK_LOCAL_MEM_FENCE); 	
	
	    //Objective function!

	    partial[localID] = 0;
	    //Objective function!
	    for(int idx=localID; idx < SUM_LIMIT(parameters->DIMENSIONS); idx+= localSize){		
            partial[localID] += OBJECTIVE_FUNCTION(idx, phenotype);
	    }
    #endif
		
	//Local parallel reduction
		
	barrier(CLK_LOCAL_MEM_FENCE); 

    i = get_local_size(0)/2;
    while (i!=0) {
       	if (localID < i)
	  		partial[localID] += partial[localID + i]; 
       	barrier(CLK_LOCAL_MEM_FENCE);
		i = i/2;
    }
       
    if (localID == 0)
		fitness[gid] = -partial[0];	
}

__kernel void NormalizeAffinity(__global float * fitness,
								__global float * fitnessNorm,
								__global t_stats *est,
								__constant struct KernelParameters *parameters){
								
	int tid = get_global_id(0);
	
	if(tid < parameters->POP_SIZE){	
	
		float max = est->affinityBest,
			  min = est->affinityWorst;
			  
	  	if(max-min == 0.0){
			fitnessNorm[tid] = 0.0f;
		}
		else{
			float n = (fitness[tid]- min) / (max-min);
			n = 1.0f-n;
			fitnessNorm[tid] = n;
		}	
	}
}


__kernel void cloneAndHypermutation(__global unsigned *pop, 
							  __global float * fitness,
  							  __global float * fitnessNorm,
                              __global int * D_seeds,
                              __global t_stats *est,
                              __constant struct KernelParameters* parameters){

    int tid = get_global_id(0),
		lid = get_local_id(0),
        gid = get_group_id(0),
        localSize = get_local_size(0);      
        
    int DIMENSIONS = parameters->DIMENSIONS,
        REAL_LEN = parameters->REAL_LEN,
        BITS_PER_DIMENSION = parameters->BITS_PER_DIMENSION;

		int seed = D_seeds[tid];
    
    	__local unsigned clone[512];
    	__local unsigned bestClone[512];
    	__local float cloneFitness, bestCloneFitness;
		    
		float taxaMutacao = exp(-parameters->MUTATION_FACTOR * fitnessNorm[gid]);
		    
		int j,k;
		unsigned word;
		
		for(uint i = lid; i < REAL_LEN; i+=localSize){
		 	bestClone[i] = pop[gid*REAL_LEN + i]; 
		}
				 
		bestCloneFitness = fitness[gid];	 		
		
		barrier(CLK_LOCAL_MEM_FENCE);
		
		for(uint i = 0; i < parameters->NCLON; i++) {

			//Clone
			for(uint k = lid; k < REAL_LEN; k+=localSize){
			 	clone[k] = pop[gid*REAL_LEN + k];
			}	
			
			barrier(CLK_LOCAL_MEM_FENCE);
			
		    //Mutation
		    for(j = lid; j < REAL_LEN; j+=localSize){
		          
		        word = clone[j];
		        
		        for(uint l = 0; l < BITS_PER_WORD; l++){
		        
		            //Mutação            
		            if(u_rand(&seed) <= taxaMutacao){
						BIT_FLIP(word, l);
		            }
		        }		                                            
		        clone[j] = word;                                
		    }		    
		    barrier(CLK_LOCAL_MEM_FENCE);
		    
		    //------------------------------------------------------------------
		    //Clone evaluation		    
		    
            #ifdef ONEMAX

		        __local int partial[128];
			    partial[lid] = 0;
			    int idx, m;
			    unsigned aux, shift;
			    int value;			
			
			    //if(localSize <= parameters->REAL_LEN){
	
				    for(int idx=lid; idx < parameters->REAL_LEN; idx+=localSize) {
				     	aux = clone[idx];
					    shift = MASK;
				
					    for(int j=0;j<BITS_PER_WORD; j+=1) {
		
					       value = (aux & shift) == 0 ? 0 : 1;
					       shift = shift >> 1;
					       partial[lid] += value;		   
					    }	
				    }
	
			    barrier(CLK_LOCAL_MEM_FENCE); 

			    idx = get_local_size(0)/2;
			    while (idx!=0) {
			       	if (lid < idx)
			      		partial[lid] += partial[lid + idx]; 
			       	barrier(CLK_LOCAL_MEM_FENCE);
				    idx = idx/2;
			    }
            #endif

            #ifndef ONEMAX
				
			    __local float partial[128];
			    __local float phenotype[1024];
			
			    partial[lid] = 0;		
		        //Genotype decoding....	

                int index, val, start, stop, tp;
                float bitpow;
                int mask = 1, count;
                float localPhenotype;
	
	            for(int k = lid; k < DIMENSIONS; k+=localSize){

                    localPhenotype = 0.0f;

		            index = (k * BITS_PER_DIMENSION) / BITS_PER_WORD;
		            val = clone[index];		  
		            start = k * BITS_PER_DIMENSION;
		            stop = start + BITS_PER_DIMENSION;

		            tp = val >> start;
		            count = 0;

	                for(int j = start; j < stop; j++) {

	              		// test for current bit 0 or 1
		                if((tp & mask) == 1) {
		                    bitpow = native_powr(2.0f, count);
                            localPhenotype += bitpow;
		                }
		                tp = tp >> 1;
		                count++;
		            }

                    phenotype[k] = parameters->LOWER_LIM + (((parameters->UPPER_LIM - parameters->LOWER_LIM)/(native_powr(2.0f, BITS_PER_DIMENSION)-1)*localPhenotype));
	            }
	
			    //Objective function!
			    for(int idx=lid; idx < SUM_LIMIT(DIMENSIONS); idx+= localSize){
				    partial[lid] += OBJECTIVE_FUNCTION(idx, phenotype);
			    }
		
			    //Local parallel reduction
		
			    barrier(CLK_LOCAL_MEM_FENCE); 
			
			    int idx = get_local_size(0)/2;
			    while (idx!=0) {
			       	if (lid < idx)
			      		partial[lid] += partial[lid + idx]; 
			       	barrier(CLK_LOCAL_MEM_FENCE);
				    idx = idx/2;
			    }
 
			#endif    

			barrier(CLK_LOCAL_MEM_FENCE); 			
			
            if(lid==0)
                cloneFitness = -partial[0];
			
			barrier(CLK_LOCAL_MEM_FENCE); 			
			
		    
		    //------------------------------------------------------------------		    
		    
		    //barrier(CLK_LOCAL_MEM_FENCE);
		    
		    if(cloneFitness > bestCloneFitness){
		    	for(int k = lid; k < REAL_LEN; k+=localSize){
			 		bestClone[k] = clone[k];		 
				}
				bestCloneFitness = cloneFitness;
		    }
		    barrier(CLK_LOCAL_MEM_FENCE);		    
		}    
		
		//--------------------------------------------------------------------
		//Replacement
		
		barrier(CLK_LOCAL_MEM_FENCE);
	    if(bestCloneFitness > fitness[gid]){
	    	for(uint k = lid; k < REAL_LEN; k+=localSize){
		 		pop[gid*REAL_LEN + k] = bestClone[k];		 
			}		    	
			if(lid==0)
				fitness[gid] = bestCloneFitness;
	    }		    
		    
		D_seeds[tid] = seed; 	
}

__kernel void StatisticsReduction1(__global unsigned *pop, 
								   __global float * fitness,
								  __global t_stats *est,
		                          __global KernelParameters* parameters){

	//TODO: Two-stages reduction
	
	int tid = get_global_id(0),
        lid = get_local_id(0),
        gid = get_group_id(0),
        localSize = get_local_size(0);
        
	int P_POP_SIZE = parameters->POP_SIZE;
     
	__local float affinities[64];
	__local int index[64];
			
	affinities[lid] = fitness[lid];
	index[lid] = lid;
	   
	 //Looks for the best antibody
	 if(gid==1){      
		
		for (int i=localSize+lid; i < P_POP_SIZE; i+=localSize)
		{
			if(fitness[i] > affinities[lid]){
				affinities[lid] = fitness[i];
				index[lid] = i;
			}
		}
		
		//Redução 
		int next_power_of_2 = localSize; //(int)native_powr( (float)2.0, (int) ceil( log2( (float) localSize ) ) );
	
		for(int s = next_power_of_2/2; s>0; s=s>>1){
			barrier(CLK_LOCAL_MEM_FENCE);
			if(lid < s && lid+s < localSize){	 
				//int value = posicao[lid+s];
				if(affinities[lid+s]>affinities[lid]){
					affinities[lid] = affinities[lid+s];
					index[lid] = index[lid+s];        	    
				}
			}
		}    
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(lid==0){
			est[0].indexBest = index[0];
			est[0].affinityBest = affinities[0];
		}
		
	 }
	 //Looks for the worst antibody
	 else{
		for (int i=localSize+lid; i< P_POP_SIZE; i+=localSize)
		{
			if(fitness[i] < affinities[lid]){
				affinities[lid] = fitness[i];
				index[lid] = i;
			}
		}     
		 
		//Local parallel reduction 
		int next_power_of_2 = localSize; //(int)native_powr( (float)2.0, (int) ceil( log2( (float) localSize ) ) );
	
		for(int s = next_power_of_2/2; s>0; s=s>>1){
			barrier(CLK_LOCAL_MEM_FENCE);
			if(lid < s && lid+s < localSize){	 
				//int value = posicao[lid+s];
				if(affinities[lid+s] < affinities[lid]){
					affinities[lid] = affinities[lid+s];
					index[lid] = index[lid+s];        	    
				}
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
		
		if(lid==0){
			est[0].indexWorst = index[0];
			est[0].affinityWorst = affinities[0];
		}
	 }
}

__kernel void randomInsertion(__global unsigned int *pop, 
                              __global float * fitness,
                              __global t_stats *stats,
                              __global int * D_seeds,
                              __constant KernelParameters* p){
                              
    int tid = get_global_id(0),
        lid = get_local_id(0),
        gid = get_group_id(0),
        localSize = get_local_size(0);
        
    int seed = D_seeds[tid];
    
    __local float affinities[256];
    __local int index[256];
            
    affinities[lid] = fitness[lid];
    index[lid] = lid;
       
    for (int i=localSize+lid;i<p->POP_SIZE;i+=localSize)
    {
        if(fitness[i] < affinities[lid]){
            affinities[lid] = fitness[i];
            index[lid] = i;
        }
    }     

	//local parallel reduction
     
    int next_power_of_2 = localSize; //(int)native_powr( (float) 2.0, (int) ceil( log2( (float) localSize ) ) );

    for(int s = next_power_of_2/2; s>0; s=s>>1){
        barrier(CLK_LOCAL_MEM_FENCE);
        if(lid < s && lid+s < localSize){	 
    	    if(affinities[lid+s] < affinities[lid]){
       	        affinities[lid] = affinities[lid+s];
                index[lid] = index[lid+s];        	    
    	    }
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    
    __local unsigned int new[512];
    
    unsigned word = 0;    

    for(int i = lid; i < p->REAL_LEN; i+=localSize)
    {
        for(uint j=0; j < BITS_PER_WORD; j++){
        
        	word = word << 1;      	
		
		    if(u_rand(&seed) <= 0.5){
			    word = word | 1;
		    }
        }

        new[i] = word;
    }
    
    D_seeds[tid] = seed;

    barrier(CLK_LOCAL_MEM_FENCE);    
    fitness[index[0]] = EvaluateIndividual_local(new, lid, localSize, p);    
    if(lid==0){
	    for( uint i = 0; i < p->REAL_LEN; i+=1 )
    	{   
    	    pop[index[0] * p->REAL_LEN + i] = new[i];    
    	}
	}
}
