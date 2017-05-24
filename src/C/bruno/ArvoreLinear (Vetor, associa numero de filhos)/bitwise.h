#ifndef BITWISE_H_INCLUDED
#define BITWISE_H_INCLUDED

unsigned int floatToInt(float floatVal);
float intToFloat(unsigned int intVal);
unsigned int packInfo(int tipo, int valor);
unsigned int packInt(int tipo, int valor);
unsigned int packFloat(int tipo, float valor);
int unpackTipo(unsigned int info);
int unpackInt(unsigned int info);
float unpackFloat(unsigned int info);



#endif // BITWISE_H_INCLUDED