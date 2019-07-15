#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "intercalacaobalanceada.h"
#include "lista.h"
#include "constantes.h"
#include "quicksort_externo.h"

#define ARQUIVO "PROVAO.TXT"
#define TEMP "temp.bin"

// gera um arquivo binario temporario com os primeiros 'n' registros do arquivo original
void gera_arquivo(FILE *arq, FILE *temp, int n) {

	registro r;
	for(int i = 0; i < n; i++) {
//		fscanf(arq, "%d %f %s %[^  ] %[^\n]", &r.matricula, &r.nota, r.estado, r.cidade, r.curso);
		fscanf(arq, "%d %f ", &r.matricula, &r.nota);
		fgets(r.estado, 3, arq);
		fseek(arq, 1, 1);
		fgets(r.cidade, 50, arq);
		//fscanf(arq, "[A-Z ] %[ ]", &r.cidade);
		fseek(arq, 2, 1);
		//fgets(r.curso, 29, arq);
		fscanf(arq, "%[A-Z] %[\n]", &r.curso);
//		printf("%i|%g|%s|%s|%s\n", r.matricula, r.nota, r.estado, r.cidade, r.curso);
		fwrite(&r, sizeof(registro), 1, temp);
	}
}

int main(int argc, char **argv) {

	int metodo, quantidade, situacao, p;

	int transf, comp;  //dados para comparaçao de metodos
	transf = comp = 0;
	// Argumentos passados na linha de comando
	metodo = atoi(argv[1]);
    quantidade = atoi(argv[2]);
    situacao = atoi(argv[3]);
	if (argc == 5) { //Verificação de argumento opcional -P
		p = !strcmp(argv[4], "-P");
	} else {
		p = 0;
	}

	// Geração de arquivo temporário com n primeiros elementos do arquivo
	FILE *arquivo, *temp;

	if((arquivo = fopen(ARQUIVO, "r")) == NULL) return 1;
	temp = fopen(TEMP, "w+b");

	gera_arquivo(arquivo, temp, quantidade);
	rewind(temp);

	FILE** fitas = (FILE**) malloc(sizeof(FILE*)*FF); // vetor de arquivos/fitas
	for(int i = 0; i < FF; i++) {
		char nome[11];
		sprintf(nome, "fita%.2i.bin", i);
		fitas[i] = fopen(nome, "w+b");
	}

	int elementos[FF];
	for(int i = 0; i < FF; i++) elementos[i] = 0;
	lista blocos[FF];
	for(int i = 0; i < FF; i++) blocos[i].primeiro = NULL;

	// coloca o arquivo na situação escolhida
	if(situacao == 1) {
		gerarblocos_selecao(quantidade, temp, fitas, elementos, blocos, 1, &transf, &comp);
		intercalablocos_f1(quantidade, temp, fitas, elementos, blocos, &transf, &comp);

		registro r1, r2;
		for(int i = 0; i < quantidade/2; i++) {
			fseek(temp, sizeof(registro)*i, 0);
			fread(&r1, sizeof(registro), 1, temp);
			fseek(temp, -sizeof(registro)*(i+1), 2);
			fread(&r2, sizeof(registro), 1, temp);

			fseek(temp, sizeof(registro)*i, 0);
			fwrite(&r2, sizeof(registro), 1, temp);
			fseek(temp, -sizeof(registro)*(i+1), 2);
			fwrite(&r1, sizeof(registro), 1, temp);
		}
	} else if(situacao == 2) {
		gerarblocos_selecao(quantidade, temp, fitas, elementos, blocos, 1, &transf, &comp);
		intercalablocos_f1(quantidade, temp, fitas, elementos, blocos, &transf, &comp);
	}

	rewind(temp);
	if (p) {  //Imprime dados iniciais do arquivo a ser ordenado
		for(int i = 0; i < quantidade; i++) {
			registro r;
			fread(&r, sizeof(registro), 1, temp);
			printf("%i\t%.2f\t%s\t%s\t%s\n", r.matricula, r.nota, r.estado, r.cidade, r.curso);
		}
	}

	//Medição do tempo
	struct timeval comecoExecucao, fimExecucao;
	comecoExecucao.tv_usec = 0;
	fimExecucao.tv_usec = 0;

	gettimeofday(&comecoExecucao, NULL);

	if (metodo == 1) {
		gerarblocos_ordenacao(quantidade, temp, fitas, elementos, blocos, 0, &transf, &comp);
		intercalablocos(quantidade, temp, fitas, elementos, blocos, &transf, &comp);
	} else if (metodo == 2) {
		gerarblocos_selecao(quantidade, temp, fitas, elementos, blocos, 0, &transf, &comp);
		intercalablocos(quantidade, temp, fitas, elementos, blocos, &transf, &comp);
	} else if (metodo == 3) {
		gerarblocos_selecao(quantidade, temp, fitas, elementos, blocos, 1, &transf, &comp);
		intercalablocos_f1(quantidade, temp, fitas, elementos, blocos, &transf, &comp);
	} else if (metodo == 4) {  //QuickSort Externo
	    FILE *ArqLEs = fopen(TEMP, "r+b");
	    FILE *ArqEi  = fopen(TEMP, "r+b");

		QuickSortExterno(&temp, &ArqEi, &ArqLEs, 1, quantidade, &transf, &comp);
		fflush(temp);
		fclose(ArqEi); fclose(ArqLEs);
	}

	if (p) {  //Imprime dados iniciais do arquivo a ser ordenado
		printf("===============================================\n");
		printf("============ Arquivos Após Ordenação ==========\n");
		printf("===============================================\n");
		rewind(temp);
		for(int i = 0; i < quantidade; i++) {
			registro r;
			fread(&r, sizeof(registro), 1, temp);
			printf("%i\t%.2f\t%s\t%s\t%s\n", r.matricula, r.nota, r.estado, r.cidade, r.curso);
		}
	}

	//Impressão de dados de tempo e comparações
	// printf("Quant\tComp\tTransf\tTempo\n");
	// printf("%d\t%d\t%d\t%.2f\n", quantidade, comp, transf, time);

	// printf("\n\nNumero de comparacoes: %d\n", comp);
	// printf("Numero de transferencias: %d\n", transf);

	gettimeofday(&fimExecucao, NULL);
	float time = (float)((fimExecucao.tv_sec - comecoExecucao.tv_sec) + (fimExecucao.tv_usec - comecoExecucao.tv_usec)/1000000.0);
	// printf("Tempo de execucao: %f\n", time);

	printf("Quant\t\tComp\t\tTransf\t\tTempo\n");
	printf("%d\t\t%d\t\t%d\t\t%.8f\n", quantidade, comp, transf, time);

	// fecha os arquivos abertos
	for(int i = 0; i < FF; i++) fclose(fitas[i]);
	fclose(arquivo);
	fclose(temp);
	// exclui os arquivos temporarios criados
	remove(TEMP);
	for(int i = 0; i < FF; i++) {
		char nome[11];
		sprintf(nome, "fita%.2i.bin", i);
		remove(nome);
	}

	return 0;
}
