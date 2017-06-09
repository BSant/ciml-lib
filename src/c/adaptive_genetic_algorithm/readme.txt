O código possui diferentes versões de Algoritmo Genético, verificar o Makefile antes de compilar.

O padrão da representação é Job Based Representation
-OBR para Operation Based Representation


Definir:
-GA para algoritmo genético padrão
-PPC para algoritmo genético adaptativo utilizando Predictive Parameter Control (discreto)
-PPCR para algoritmo genético adaptativo utilizando Predictive Parameter Control (contínuo)
-AP para algoritmo genético adaptativo utilizando Adaptive Pursuit
-NEH para inicializar parte da população com a heurística construtiva NEH(Nawaz, Enscore, Ham)

Algoritmo Genético:
-Crossover: PMX crossover
-Mutação: Shift mutation

As técnicas Adaptativas atuam nos seguintes parâmetros:
-Taxa de crossover
-Taxa de mutação
-Operador de crossover empregado
-Operador de mutação empregado

As técnicas implementadas de crossover são:
-PMX crossover
-LOX crossover
-op crossover

As técnicas de mutação impregadas são:
-Shift mutation
-Interchange mutation

Para executar o AG "default" utilize:
 ./nome_do_executável nome_do_arquivo_teste numero_da_repeticao
Para selecionar os parâmetros do AG utilize:
 ./nome_do_executável nome_do_arquivo_teste numero_da_repeticao tamanho_da_população taxa_de_crossover taxa_de_mutação

Para executar o AG adaptativo "default" utilize:
 ./nome_do_executável nome_do_arquivo_teste numero_da_repeticao
Para selecionar os parâmetros do AG adaptativo utilize:
 ./nome_do_executável nome_do_arquivo_teste numero_da_repeticao tamanho_da_população

Versão 3.1 - Alteração na forma de recompensar os operadores e correção de erros menores:
A recompensa do crossover é calculada por:
recompensa = fitness_melhor_pai/fitness_melhor_filho

A recompensa da mutação é calculada por:
recompensa = fitness_médio_da_população/fitness_alcançado

Versão 3.0
A recompensa do crossover é calculada por:
recompensa = (fitness_pai1 + fitness_pai2)/(fitness_filho1 + fitness_filho2)

A recompensa da mutação é calculada por:
recompensa = (fitness_pai1 + fitness_pai2)/(fitness_offspring * 2)
