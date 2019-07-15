#include "quicksort_externo.h"

void FAVazia(TArea *Area) {
    int i;
    Area->NumCelOcupadas = 0;
    Area->Primeiro = -1;
    Area->Ultimo = -1;
    Area->CelulasDisp = 0;
    for (i = 0; i < TAMAREA; i++) {
        Area->Itens[i].Ant = -1;
        Area->Itens[i].Prox = i + 1;
    }
}

int ObterNumCelOcupadas(TArea* Area) {
    return (Area->NumCelOcupadas);
}

void InsereItem(registro Item, TArea *Area, int* comp) {
    int pos, disp, IndiceInsercao;
    (*comp)++;
    if (Area->NumCelOcupadas == TAMAREA) {
        printf("Tentativa de inserção em memoria cheia.\n");
        return;
    }
    disp = Area->CelulasDisp;
    Area->CelulasDisp = Area->Itens[Area->CelulasDisp].Prox;
    Area->Itens[disp].Item = Item;
    Area->NumCelOcupadas++;
    (*comp)++;
    if (Area->NumCelOcupadas == 1) {
        /*Inserção do primeiro item*/
        Area->Primeiro = disp;
        Area->Ultimo = Area->Primeiro;
        Area->Itens[Area->Primeiro].Prox = -1;
        Area->Itens[Area->Primeiro].Ant = -1;
        return;
    }
    pos = Area->Primeiro;
    (*comp)++;
    if (Item.nota < Area->Itens[pos].Item.nota) {
        /*Inserção na primeira posição*/
        Area->Itens[disp].Ant = -1;
        Area->Itens[disp].Prox = pos;
        Area->Itens[pos].Ant = disp;
        Area->Primeiro = disp;
        return;
    }
    IndiceInsercao = Area->Itens[pos].Prox;
    while (IndiceInsercao != -1 && Area->Itens[IndiceInsercao].Item.nota < Item.nota) {
        (*comp)++;
        pos = IndiceInsercao;
        IndiceInsercao = Area->Itens[pos].Prox;
    }
    (*comp)++;
    if (IndiceInsercao == -1) {
        /*Inserção realizada na ultima posição*/
        Area->Itens[disp].Ant = pos;
        Area->Itens[disp].Prox = -1;
        Area->Itens[pos].Prox = disp;
        Area->Ultimo = disp;
        return;
    }
    Area->Itens[disp].Ant = pos;
    Area->Itens[disp].Prox = Area->Itens[pos].Prox;
    Area->Itens[pos].Prox = disp;
    pos = Area->Itens[disp].Prox;
    Area->Itens[pos].Ant = disp;
    /*Inserção realizada no meio da área*/
}

void RetiraPrimeiro(TArea *Area, registro* Item, int* comp) {

    Apontador ProxTmp;
    (*comp)++;
    if (Area->NumCelOcupadas == 0) {
        /*Área Vazia*/
        //printf("Erro. Memoria vazia\n");
        return;
    }
    *Item = Area->Itens[Area->Primeiro].Item;
    ProxTmp = Area->Itens[Area->Primeiro].Prox;
    Area->Itens[Area->Primeiro].Prox = Area->CelulasDisp;
    Area->CelulasDisp = Area->Primeiro;
    Area->Primeiro = ProxTmp;
    (*comp)++;
    if ((unsigned int) Area->Primeiro < TAMAREA)
        Area->Itens[Area->Primeiro].Ant = -1;
    Area->NumCelOcupadas--;
}

void RetiraUltimo(TArea* Area, registro *Item, int *comp) {
    Apontador AntTmp;
    (*comp)++;
    if (Area->NumCelOcupadas == 0) {
        /*Area vazia*/
        printf("Erro. Memoria vazia\n");
        return;
    }
    *Item = Area->Itens[Area->Ultimo].Item;
    AntTmp = Area->Itens[Area->Ultimo].Ant;
    Area->Itens[Area->Ultimo].Prox = Area->CelulasDisp;
    Area->CelulasDisp = Area->Ultimo;
    Area->Ultimo = AntTmp;
    (*comp)++;
    if ((unsigned int) Area->Ultimo < TAMAREA)
        Area->Itens[Area->Ultimo].Prox = -1;
    Area->NumCelOcupadas--;

}

void ImprimeArea(TArea* Area) {
    int pos;
    if (Area->NumCelOcupadas <= 0) {
        printf("Memoria vazia\n");
        return;
    }
    printf("Memoria\n");
    printf("Numero de celulas ocupadas: %d\n", Area->NumCelOcupadas);
    pos = Area->Primeiro;
    while (pos != -1) {
        printf("%f ", Area->Itens[pos].Item.nota);
        pos = Area->Itens[pos].Prox;
    }
}

/*Procedimentos utilizados pela Partição do QuickSort*/

void LeSup(FILE **ArqLEs, TReg* UltLido, int* Ls, short *OndeLer, int* transf) {
    //assert(ArqLEs);
    fseek(*ArqLEs, (*Ls - 1) * sizeof (registro), SEEK_SET);
    fread(UltLido, sizeof (registro), 1, *ArqLEs);
    (*transf)++;
    (*Ls)--;
    *OndeLer = false;
}

void LeInf(FILE** ArqLi, TReg* UltLido, int* Li, short* OndeLer, int* transf) {
    //assert(LeInf);
    fread(UltLido, sizeof (TReg), 1, *ArqLi);
    (*transf)++;
    (*Li)++;
    *OndeLer = true;
}

void InserirArea(TArea* Area, TReg* UltLido, int* NRArea, int* comp) {
    /*Insere UltLido de forma ordenada na área*/
    InsereItem(*UltLido, Area, comp);
    *NRArea = ObterNumCelOcupadas(Area);
}

void EscreveMax(FILE **ArqLEs, TReg R, int *Es, int* transf) {
    //assert(ArqLEs);
    fseek(*ArqLEs, (*Es - 1) * sizeof (TReg), SEEK_SET);
    fwrite(&R, sizeof (TReg), 1, *ArqLEs);
    (*transf)++;
    (*Es)--;
}

void EscreveMin(FILE **ArqEi, TReg R, int *Ei, int* transf) {
    fwrite(&R, sizeof (TReg), 1, *ArqEi);
    (*transf)++;
    (*Ei)++;
}

void RetiraMax(TArea* Area, TReg *R, int *NRArea, int* comp) {
    RetiraUltimo(Area, R, comp);
    *NRArea = ObterNumCelOcupadas(Area);
}

void RetiraMin(TArea* Area, TReg* R, int *NRArea, int* comp) {
    RetiraPrimeiro(Area, R, comp);
    *NRArea = ObterNumCelOcupadas(Area);
}

void Particao(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, TArea Area, int Esq, int Dir, int*i, int*j, int* transf, int* comp) {
    assert(ArqLi);
    assert(ArqEi);
    assert(ArqLEs);
    int Ls = Dir;
    int Es = Dir;
    int Li = Esq;
    int Ei = Esq;
    int NRArea = 0;
    int Linf = INT_MIN;
    int Lsup = INT_MAX;
    short OndeLer = true;
    TReg UltLido, R;

    fseek(*ArqLi, (Li - 1) * sizeof (TReg), SEEK_SET);
    fseek(*ArqEi, (Ei - 1) * sizeof (TReg), SEEK_SET);
    *i = Esq - 1;
    *j = Dir + 1;
    while (Ls >= Li) {
        (*comp)++;
        if (NRArea < TAMAREA - 1) {
            if (OndeLer)
                LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, transf);
            else
                LeInf(ArqLi, &UltLido, &Li, &OndeLer, transf);

            InserirArea(&Area, &UltLido, &NRArea, comp);
            continue;
        }
        (*comp)++;
        if (Ls == Es)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, transf);
        else
        (*comp)++;
        if (Li == Ei)
            LeInf(ArqLi, &UltLido, &Li, &OndeLer, transf);
        else
            if (OndeLer)
            LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, transf);
        else
            LeInf(ArqLi, &UltLido, &Li, &OndeLer, transf);
        (*comp)++;
        if (UltLido.nota > Lsup) {
            *j = Es;
            EscreveMax(ArqLEs, UltLido, &Es, transf);
            continue;
        }
        (*comp)++;
        if (UltLido.nota < Linf) {
            *i = Ei;
            EscreveMin(ArqEi, UltLido, &Ei, transf);
            continue;
        }
        InserirArea(&Area, &UltLido, &NRArea, comp);
        (*comp)++;
        if (Ei - Esq < Dir - Es) {
            RetiraMin(&Area, &R, &NRArea, comp);
            EscreveMin(ArqEi, R, &Ei, transf);
            Linf = R.nota;
        } else {
            RetiraMax(&Area, &R, &NRArea, comp);
            EscreveMax(ArqLEs, R, &Es, comp);
            Lsup = R.nota;
        }
    }
    while (Ei <= Es) {
        (*comp)++;
        RetiraMin(&Area, &R, &NRArea, comp);
        EscreveMin(ArqEi, R, &Ei, comp);
    }
}

void QuickSortExterno(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, int Esq, int Dir, int* transf, int* comp) {
    assert(ArqLi);
    assert(ArqEi);
    assert(ArqLEs);
    int i, j;
    TArea Area; /*Área de armazenamento interna*/
    if (Dir - Esq < 1)
        return;
    FAVazia(&Area);
    Particao(ArqLi, ArqEi, ArqLEs, Area, Esq, Dir, &i, &j, transf, comp);
    if (i - Esq < Dir - j) {
        /*Ordena primeiro o subarquivo menor */
        QuickSortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, transf, comp);
        QuickSortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, transf, comp);
    } else {

        QuickSortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, transf, comp);
        QuickSortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, transf, comp);
    }
}
