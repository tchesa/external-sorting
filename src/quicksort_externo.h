#ifndef QUICKSORTEXTERNO_H
#define	QUICKSORTEXTERNO_H

#define TAMAREA 100
#include "constantes.h"
#include "assert.h"
#include "stdio.h"
#include "limits.h"

typedef int Apontador;
typedef int Chave;

typedef struct
{
    registro Item;
    Apontador Prox, Ant;
} Celula;

typedef struct
{
  Celula Itens[TAMAREA];
  Apontador CelulasDisp, Primeiro, Ultimo;
  int NumCelOcupadas;
} TArea;

typedef registro TReg;

/*Declaracao dos tipos utilizados pelo quicksort externo*/
void FAVazia(TArea *Area);
int ObterNumCelOcupadas(TArea* Area);
void InsereItem(registro Item, TArea *Area, int&);
void RetiraPrimeiro(TArea *Area, registro* Item, int&);
void RetiraUltimo(TArea* Area, registro *Item, int&);
void ImprimeArea(TArea* Area);

/*Procedimentos utilizados pelo Particao do quicksort externo*/
void LeSup(FILE **ArqLEs, TReg* UltLido, int* Ls, short *OndeLer, int*);
void LeInf(FILE **ArqLi, TReg* UltLido, int* Li, short* OndeLer, int*);
void InserirArea(TArea* Area, TReg* UltLido, int* NRArea, int*);
void EscreveMax(FILE **ArqLEs, TReg R, int *Es, int*);
void EscreveMin(FILE **ArqEi, TReg R, int *Ei, int*);
void RetiraMax(TArea** Area, TReg *R, int *NRArea);
void RetiraMin(TArea** Area, TReg* R, int *NRArea);
void Particao(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, TArea Area, int Esq, int Dir, int*i, int*j, int& transf, int* comp);
void QuickSortExterno(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, int Esq, int Dir, int*, int*);

#endif	/* QUICKSORTEXTERNO_H */
