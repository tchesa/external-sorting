#include <stdlib.h>
#include <stdio.h>

typedef struct elemento* celula;

typedef struct elemento {
	int valor;
	celula anterior;
	celula proximo;
} elemento;

typedef struct lista {
	celula primeiro;
} lista;

void lista_adicionar(lista *l, int valor) {
	
	celula n = (celula)malloc(sizeof(elemento));
	n->valor = valor;
	n->anterior = NULL;
	n->proximo = NULL;

	if(l->primeiro == NULL) l->primeiro = n;
	else {
		celula aux = l->primeiro;
		while(aux->proximo != NULL) aux = aux->proximo;

		aux->proximo = n;
		n->anterior = aux;
	}
}

int lista_removeprimeiro(lista *l) {

	if(l->primeiro == NULL) return -1;

	celula aux = l->primeiro;
	int r = aux->valor;
	l->primeiro = aux->proximo;

	//free(aux);
	aux = NULL;
	return r;
}

 int lista_removerelemento(lista *l, int valor) {
	
 	if(l->primeiro == NULL) return -1;
	
 	celula aux = l->primeiro;
 	while(aux != NULL && aux->valor != valor) aux = aux->proximo;
 	if(aux == NULL) return -1;

 	if(aux == l->primeiro) l->primeiro = l->primeiro->proximo;

 	if(aux->anterior != NULL) aux->anterior->proximo = aux->proximo;
 	if(aux->proximo != NULL) aux->proximo->anterior = aux->anterior;

 	int r = aux->valor;
 	free(aux);
 	return r;
 }

int lista_tamanho(lista *l) {
	if(l->primeiro == NULL) return 0;
	int n;
	celula aux = l->primeiro;

	for(n = 0; aux != NULL; n++) aux = aux->proximo;
	return n;
}

int lista_posicao(lista *l, int indice) {
	
	if(l->primeiro == NULL || indice >= lista_tamanho(l) || indice < 0) return -1;

	celula aux = l->primeiro;
	for(int i = 0; i < indice; i++) aux = aux->proximo;

	return aux->valor;
}

void lista_imprime(lista *l) {
	printf("[ ");
	if(l->primeiro != NULL) {
		for(celula i = l->primeiro; i != NULL; i = i->proximo) {
			printf("%i ", i->valor);
		}
	}
	printf("]");
}

void lista_limpar(lista *l) {
	if(l->primeiro == NULL) return;
	
	while(l->primeiro != NULL) lista_removeprimeiro(l);
}