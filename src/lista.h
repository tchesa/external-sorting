#ifndef LISTA_H
#define LISTA_H

typedef struct elemento* celula;

typedef struct elemento {
	int valor;
	celula anterior;
	celula proximo;
} elemento;

typedef struct lista {
	celula primeiro;
} lista;

void lista_adicionar(lista *l, int valor);

int lista_removeprimeiro(lista *l);

int lista_removerelemento(lista *l, int valor);

int lista_tamanho(lista *l);

int lista_posicao(lista *l, int indice);

void lista_imprime(lista *l);

void lista_limpar(lista *l);

#endif