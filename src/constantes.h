#ifndef CONSTANTES_H
#define CONSTANTES_H

#define FF 20 // Numero de fitas magneticas
#define F (FF/2)
#define Fm1 (FF-1)

typedef struct registro {
	int matricula;
	float nota;
	char estado[3];
	char cidade[51];
	char curso[31];
} registro;

#endif
