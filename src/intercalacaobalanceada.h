#ifndef INTERCALACAOBALANCEADA_H
#define INTERCALACAOBALANCEADA_H

#include "lista.h"
#include "constantes.h"
#include <stdio.h> 
void gerarblocos_ordenacao (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo, int*, int*);
void gerarblocos_selecao   (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo, int*, int*);
void intercalablocos       (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], int *, int *);
void intercalablocos_f1    (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], int *, int *);

#endif
