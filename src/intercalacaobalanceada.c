#include <stdio.h>
#include <stdlib.h>

#include "constantes.h"
#include "heapsort.h"
#include "quicksort.h"
#include "lista.h"

// retorna 1 se todos os elementos da memoria estao marcados
short todosMarcados(short marcados[], int n) {
	for(int i = 0; i < n; i++) if(!marcados[i]) return 0;
	return 1;
}

// Retorna 1 se tem elementos na memoria (se tem algum elemento que nao está marcado com '-1')
short temElementoNaMemoria(short marcados[], int n) {
	for(int i = 0; i < n; i++) if(marcados[i] != -1) return 1;
	return 0;
}

// Procura por uma unica fita que tenha elementos, dentre as 'n' fitas apartir do indice 'inicio'.
// Retorna -1 se mais de uma ou nenhuma fita esteja preenchida.
int restaUmaFitaPreenchida(int nElementos[], int n, int inicio, int* transf, int* comp) {
	int preenchida = -1;
	for(int i = 0; i < n; i++) {
		(*comp)++;
		if(nElementos[inicio+i] > 0) {
			if(preenchida == -1) preenchida = i;
			else return -1;
		}
	}
	return (preenchida == -1) ? preenchida : inicio + preenchida;
}

// Verifica se todas as fitas foram percorridas
short percorreuTodas(FILE **fitas, int elementos[], int n, int inicio, int *transf, int* comp) {
	for(int i = 0; i < n; i++) {
		(*transf)++;
		if(ftell(fitas[inicio+i])/sizeof(registro) < (unsigned) elementos[inicio+i]) return 0;
	}

	return 1;
}

short percorreuTodas_fm1(FILE **fitas, int elementos[], int saida, int* transf, int* comp) {
	for(int i = 0; i < Fm1; i++) {
		(*transf)++;
		if(ftell(fitas[i+(i>=saida)])/sizeof(registro) < (unsigned) elementos[i+(i>=saida)]) return 0;
	}
	return 1;
}

// retorna 1 se todos os blocos estão esgotados
short todosBlocosEsgotados(short ativas[], int n, int* transf, int* comp) {
	for(int i = 0; i < n; i++) {
		(*comp)++;
		if(ativas[i] != -1) return 0;
	}
	return 1;
}

short todosBlocosEsgotados_fm1(short ativas[FF], int saida, int* transf, int*comp) {
	for(int i = 0; i < Fm1; i++) {
		(*comp)++;
		if(ativas[i+(i>=saida)] != -1) return 0;
	}
	return 1;
}

void gerarblocos_ordenacao (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo, int* transf, int*comp) {

	int TAMMEMORIA = (modo)? Fm1 : F; // Define o tamanho da memoria relativo ao modo utilizado
	registro memoria[TAMMEMORIA]; // memoria interna que sera usada

	for(int i = 0; i < FF; i++) rewind(fitas[i]); // zera os cursores das fitas
	rewind(temp); // zera o cursor da fita onde estao os registros
	for(int i = 0; i < FF; i++) elementos[i] = 0; // zera o numero de elemento das fitas
	int fita = 0; // fita que está sendo usada
	for(int i = 0; i < FF; i++) blocos[i].primeiro = NULL;

	// Criação dos blocos ordenados
	int numblocos = (n%TAMMEMORIA == 0)? n/TAMMEMORIA : n/TAMMEMORIA + 1;
	for(int i = 0; i < numblocos; i++) {
		lista_adicionar(&blocos[fita], elementos[fita]); // adiciona um novo bloco na fita
		// calcula o numero de registros que tera no bloco
		int numregistros = (i == numblocos-1 && n%TAMMEMORIA != 0)? n%TAMMEMORIA : TAMMEMORIA;
		(*comp)++;
		fread(memoria, sizeof(registro), numregistros, temp); // faz a leitura dos elementos da fita
		(*transf)++;

		sort(memoria, numregistros, transf, comp); // ordena a memoria
		fwrite(memoria, sizeof(registro), numregistros, fitas[fita]);
		(*transf)++;

		elementos[fita] += numregistros;

		if(modo == 0)
			fita = (fita+1)%(FF/2); // Percorre somente entre as fitas de entrada (as F primeiras do vetor de fitas)
		else if(modo == 1)
			fita = (fita+1)%(FF-1);
	}
}

void gerarblocos_selecao (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo, int* transf, int*comp) {

	int TAMMEMORIA = (modo)? Fm1 : F; // define o tamanho da memoria relativo ao modo utilizado
	registro memoria[TAMMEMORIA];
	short marcados[TAMMEMORIA];
	for(int i = 0; i < FF; i++) {
		elementos[i] = 0;
		lista_limpar(&blocos[i]);
	}
	rewind(temp);

	int fita = 0; // pega a primeira fita de entrada como fita alvo
	registro proximo;

	//pega os 'TAMMEMORIA' primeiros elementos do arquivo
	fread(memoria, sizeof(registro), TAMMEMORIA, temp);
	(*transf)++;
	for(int i = 0; i < TAMMEMORIA; i++) {
		marcados[i] = 0;
		(*comp)++;
	}
	// Enquanto nao percorrer todo o arquivo ou houver elementos na memoria
	while (ftell(temp)/sizeof(registro) < (unsigned) n || temElementoNaMemoria(marcados, TAMMEMORIA)) {
		(*transf)++;
		for(int i = 0; i < TAMMEMORIA; i++) {
			if(marcados[i] == 1) marcados[i] = 0; // desmarca todos que estavam na memoria
			(*comp)++;
		}

		lista_adicionar(&blocos[fita], elementos[fita]); // adiciona um novo bloco para a fita alvo
		while (!todosMarcados(marcados, TAMMEMORIA)) { // enquanto houver um elemento na memoria não-marcado

			heapsort_marcados(memoria, marcados, TAMMEMORIA, modo, transf, comp);
			fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor elemento da memoria na fita alvo
			(*transf)++;
			elementos[fita]++;

			if(ftell(temp)/sizeof(registro) < (unsigned) n) { // se o arquivo nao acabou
				fread(&proximo, sizeof(registro), 1, temp); // lê o proximo elemento do arquivo
				(*transf)++;
				if(proximo.nota > memoria[0].nota)  // se o proximo elemento for menor que o primeiro elemento da memoria
					marcados[0] = 1; // marca o primeiro elemento
				memoria[0] = proximo; // joga o proximo elemento na primeira posição da memoria
				(*comp)++;
			} else {
				marcados[0] = -1; // marca o primeiro elemento (nao tem elemento na memoria)
			}
		}

		if(modo == 0)
			fita = (fita+1)%(FF/2); // Percorre somente entre as fitas de entrada (as F primeiras do vetor de fitas)
		else if(modo == 1)
			fita = (fita+1)%(FF-1);
	}
}

void intercalablocos (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], int *transf, int *comp) {

	registro memoria[F];
	int from[F];
	short ativas[F];
	short saltofitas = 1;
	int fita;
	for(int i = 0; i<FF; i++) {
		(*transf)++;
		rewind(fitas[i]);
	}
	// f armazena o indice da unica fita preenchida (-1 se nao houver uma unica fita preenchida)
	int f = restaUmaFitaPreenchida(elementos, F, F*saltofitas, transf, comp);
	while (f == -1) {

		fita = F*saltofitas; // alveja a primeira fita de saida
		int bloco = 0;
		while (!percorreuTodas(fitas, elementos, F, F*(!saltofitas), transf, comp)) { // enquanto nao percorrer todas as fitas de entrada

			lista_adicionar(&blocos[fita], ftell(fitas[fita])/sizeof(registro)); // adiciona um novo bloco na fita alvo
			int limiteblocos[F]; // indice onde termina o bloco de cada fita
			for(int i = 0; i<F; i++) {
				if (lista_tamanho(&blocos[(F*!saltofitas)+i]) > bloco+1)
					limiteblocos[i] = lista_posicao(&blocos[(F*!saltofitas)+i], bloco+1);
				else
					limiteblocos[i] = elementos[(F*!saltofitas)+i];
			}

			// Pega o primeiro elemento de cada fita de entrada
			for (int i = 0; i < F; i++) {
				if(ftell(fitas[(F*!saltofitas)+i])/sizeof(registro) < (unsigned) limiteblocos[i]) { // tem elemento no bloco?
					fread(&memoria[i], sizeof(registro), 1, fitas[(F*!saltofitas)+i]); // pega o primeiro elemento
					if (ftell(fitas[(F*!saltofitas)+i])/sizeof(registro) >= (unsigned) limiteblocos[i]) // foi o ultimo elemento?
						ativas[i] = 0;
					else
						ativas[i] = 1;
					(*transf)++;
				}
				else ativas[i] = -1;
				from[i] = i+(!saltofitas*F);
			}

			while (!todosBlocosEsgotados(ativas, F, transf, comp)) { // enquanto um bloco nao estiver esgotado (-1)
				heapsort(memoria, ativas, from, F, 0, transf, comp); // ordena a memoria
				fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor elemento na fita alvo
				(*transf)++;
				elementos[fita]++; // aumenta o tamanho da fita
				if(ativas[from[0]%F]) { // se a fita do menor está ativa:
					fread(&memoria[0], sizeof(registro), 1, fitas[from[0]]); // pega o proximo elemento
					if(ftell(fitas[from[0]])/sizeof(registro) == (unsigned) limiteblocos[from[0]%F]) // se tiver sido o ultimo elemento:
						ativas[from[0]%F] = 0; // a fita se torna inativa
					(*transf)++;
				} else {
					ativas[from[0]%F] = -1; // bloco esgotado
				}
			}
			fita = (fita%F+1)%F + F*saltofitas; // proxima fita alvo
			(*comp)++;
			bloco++;
		}

		for(int i = 0; i < F; i++) {
			rewind(fitas[F*saltofitas + i]); // retorna os cursores das fitas de saida
			rewind(fitas[F*(!saltofitas) + i]); // retorna os cursores das fitas de entrada
			elementos[F*(!saltofitas) + i] = 0; // zera os elementos das fitas de entrada
			lista_limpar(&blocos[F*(!saltofitas) + i]); // limpa os blocos das fitas de entrada
			(*transf)++;
		}

		saltofitas = !saltofitas; // troca as fitas de entrada com as fitas de saida
		f = restaUmaFitaPreenchida(elementos, F, F*(!saltofitas), transf, comp); // faz a verificação (se resta apenas uma fita com elementos)
	}

	rewind(fitas[f]);
	rewind(temp);
	(*transf)++;
	registro r;
	for(int i = 0; i < n; i++) {
		// joga todos os elementos da fita f na fita principal
		fread(&r, sizeof(registro), 1, fitas[f]);
		fwrite(&r, sizeof(registro), 1, temp);
		(*transf) += 2;
	}
}

void intercalablocos_f1 (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], int *transf, int *comp) {

	registro memoria[Fm1];
	int from[Fm1];
	short ativas[Fm1];
	int fita = Fm1;
	short finalizado = 0;
	registro r;

	do {
		for(int i = 0; i < FF; i++) {
			(*transf)++;
			rewind(fitas[i]);
		}
		int bloco = 0;
		while (!percorreuTodas_fm1(fitas, elementos, fita, transf, comp)) { // enquanto nao percorrer todas as fitas de entrada

			lista_adicionar(&blocos[fita], ftell(fitas[fita])/sizeof(registro)); // adiciona um novo bloco na fita alvo
			int limiteblocos[Fm1]; // indice onde termina o bloco de cada fita
			for(int i = 0; i<Fm1; i++) {

				if (lista_tamanho(&blocos[i]) > bloco+1) {
					limiteblocos[i] = lista_posicao(&blocos[i], bloco+1);
				}

				else
					limiteblocos[i] = elementos[i];

				(*comp)++;
			}

			// Pega o primeiro elemento de cada fita de entrada
			for (int i = 0; i<Fm1; i++) {
				(*transf)++;
				if(ftell(fitas[i])/sizeof(registro) < (unsigned) limiteblocos[i]) { // tem elemento no bloco?
					fread(&memoria[i], sizeof(registro), 1, fitas[i]); // pega o primeiro elemento
					if (ftell(fitas[i])/sizeof(registro) >= (unsigned)limiteblocos[i]) // foi o ultimo elemento?
						ativas[i] = 0;
					else
						ativas[i] = 1;

					(*transf)++;
				}
				else ativas[i] = -1;
				from[i] = i;
			}

			while (!todosBlocosEsgotados_fm1(ativas, fita, transf, comp)) { // enquanto um bloco nao estiver esgotado (-1)

				heapsort(memoria, ativas, from, Fm1, 1, transf, comp); // ordena a memoria
				(*transf)++;
				fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor elemento na fita alvo
				elementos[fita]++; // atualiza o tamanho da fita de saida
				if(ativas[from[0]]) { // se a fita do menor está ativa:
					//memoria[0] = fitas[from[0]][cursores[from[0]]++];
					fread(&memoria[0], sizeof(registro), 1, fitas[from[0]]); // pega o proximo elemento
					// se tiver sido o ultimo elemento:
					if(ftell(fitas[from[0]])/sizeof(registro) == (unsigned) limiteblocos[from[0]])
						ativas[from[0]] = 0; // a fita se torna inativa
					(*transf)+=2;
				} else {
					ativas[from[0]] = -1; // bloco esgotado
				}
			}
			//elementos[fita] = cursores[fita]; // atualiza o tamanho da fita de saida
			bloco++; // fazer a intercalação dos proximos blocos
		} // blocos intercalados gerados

		if(lista_tamanho(&blocos[fita])>1) { // se a fita de saida tem mais de um bloco
			// distribui os blocos entre as fitas de entrada
			for(int i = 0; i < Fm1; i++) {
				rewind(fitas[i]); // volta o cursor da fita 'i' para o inicio
				elementos[i] = 0; // limpa a fita 'i'
				lista_limpar(&blocos[i]);
				(*transf)++;
			}
			rewind(fitas[fita]); // volta o cursor da fita de saida para o inicio

			for(int i = 0; i < lista_tamanho(&blocos[fita]); i++) {
				int j = i%Fm1;
				// adiciona um novo bloco na fita de entrada
				lista_adicionar(&blocos[j], ftell(fitas[j])/sizeof(registro));
				int limitebloco; // onde termina o bloco a ser lido
				if(lista_tamanho(&blocos[fita]) > i+1) {
					(*comp)++;
					(*transf)++;
					limitebloco = lista_posicao(&blocos[fita], i+1);
				}

				else
					limitebloco = elementos[fita];

				// joga os elementos do bloco da fita de saida para a fita de entrada 'j'
				while(ftell(fitas[fita])/sizeof(registro) < (unsigned) limitebloco) {
					fread(&r, sizeof(registro), 1, fitas[fita]);
					fwrite(&r, sizeof(registro), 1, fitas[j]);
					elementos[j]++; // atualiza o tamanho da fita 'j'
					(*transf)+=2;
				}
			}

			// limpa a fita de saida
			lista_limpar(&blocos[fita]);
			rewind(fitas[fita]);
			elementos[fita] = 0;
		} else {
			finalizado = 1; // finaliza quando a fita de saida possui apenas um bloco ordenado
		}
	} while (!finalizado);

	rewind(temp);
	rewind(fitas[fita]);
	(*transf)++;
	for(int i = 0; i < n; i++) {
		// joga todos os elementos da fita f na fita principal
		fread(&r, sizeof(registro), 1, fitas[fita]);
		fwrite(&r, sizeof(registro), 1, temp);
		(*transf)+=2;
	}
}
