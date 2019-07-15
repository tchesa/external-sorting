#include <stdio.h>
#include "constantes.h"

int partition(registro a[], int l, int r, int* transf, int* comp) {
	int i, j;
	registro pivot, t;
	pivot = a[l];
	i = l;
	j = r+1;

	while(1) {
		do ++i; while(a[i].nota >= pivot.nota && i <= r);
		do --j; while(a[j].nota < pivot.nota);
		if(i >= j) break;

		t = a[i];
		a[i] = a[j];
		a[j] = t;

		(*comp) += 2;
	}
	t = a[l];
	a[l] = a[j];
	a[j] = t;

	return j;
}

void quickSort(registro a[], int l, int r, int* transf, int* comp) {
	int j;
	if(l < r) {
		(*comp)++;
		// divide and conquer
		j = partition(a, l, r, transf, comp);
		quickSort(a, l, j-1, transf, comp);
		quickSort(a, j+1, r, transf, comp);
	}
}

void sort(registro v[], int n, int* transf, int* comp) {
	quickSort(v, 0, n-1, transf, comp);
}
