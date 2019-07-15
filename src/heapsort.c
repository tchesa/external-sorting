#include <stdio.h>
#include "constantes.h"

void swapregistro (registro v[], int a, int b) {
	registro aux = v[a];
	v[a] = v[b];
	v[b] = aux;
}

void swapInt (int v[], int a, int b) {
	int aux = v[a];
	v[a] = v[b];
	v[b] = aux;
}

void swapShort (short v[], int a, int b) {
	short aux = v[a];
	v[a] = v[b];
	v[b] = aux;
}

short ehMenor(registro v[], int from[], short ativas[], int a, int b, short modo) {
	if(modo == 0) {
		if     (ativas[from[a]%F] == -1) return 0;
		else if(ativas[from[b]%F] == -1) return 1;
		else                             return (v[b].nota < v[a].nota);
	} else {
		if     (ativas[from[a]] == -1) return 0;
		else if(ativas[from[b]] == -1) return 1;
		else                           return (v[b].nota < v[a].nota);
	}
}

short ehMenor_m(registro v[], short m[], int a, int b, short modo) {
	if(modo == 0) {
		if     (m[a%F] != 0) return 0;
		else if(m[b%F] != 0) return 1;
		else                 return (v[b].nota < v[a].nota);
	} else {
		if     (m[a] != 0) return 0;
		else if(m[b] != 0) return 1;
		else               return (v[b].nota < v[a].nota);
	}
}

// modo = 0 (2F fitas)
// modo = 1 (F+1 fitas)
void heapsort(registro v[], short ativas[], int from[], int n, short modo, int* transf, int* comp) {
	short heap; // é heap?
	do {
		heap = 1;
		for(int i = 0; i < n/2; i++) {
			(*comp)+= 4;
			if(2*i+2 < n && ehMenor(v, from, ativas, 2*i+2, 2*i+1, modo)) {
				if(ehMenor(v, from, ativas, 2*i+2, i, modo)) {
					swapregistro(v, i, 2*i+2);
					swapInt(from, i, 2*i+2);
					heap = 0;
				}
			} else {
				if(ehMenor(v, from, ativas, 2*i+1, i, modo)) {
					swapregistro(v, i, 2*i+1);
					swapInt(from, i, 2*i+1);

					heap = 0;
				}
			}
		}
	} while (!heap);
}

void heapsort_marcados(registro v[], short m[], int n, short modo, int*transf, int *comp) {
	short heap; // é heap?
	do {
		heap = 1;
		for(int i = 0; i < n/2; i++) {
			(*comp) += 4;
			if(2*i+2 < n && ehMenor_m(v, m, 2*i+2, 2*i+1, modo)) {
				if(ehMenor_m(v, m, 2*i+2, i, modo)) {
					swapregistro(v, i, 2*i+2);
					swapShort(m, i, 2*i+2);
					heap = 0;
				}
			} else {
				if(ehMenor_m(v, m, 2*i+1, i, modo)) {
					swapregistro(v, i, 2*i+1);
					swapShort(m, i, 2*i+1);
					heap = 0;
				}
			}
		}
	} while (!heap);
}
