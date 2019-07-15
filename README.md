# Ordenação Externa

O objetivo deste trabalho consiste em um estudo mais profundo da complexidade de desempenho dos seguintes métodos de ordenação externa:
* **Intercalação balanceada de vários caminhos (2f fitas)** utilizando, na etapa de geração dos blocos ordenados, **qualquer método de ordenação interna**;
* **Intercalação balanceada de vários caminhos (2f fitas)** utilizando, na etapa de geração dos blocos ordenados, a técnica de **seleção por substituição**;
* **Intercalação balanceada de vários caminhos (f+1 fitas)** utilizando, na etapa de geração dos blocos ordenados, a técnica de **seleção por substituição**;
* **Quicksort externo**.

Os métodos de ordenação em memória externa são utilizados em ocasiões em que os itens presentes em um arquivo externo são muito maiores que a capacidade de armazenamento da memória principal.

Com o intuito de avaliar o desempenho dos diferentes métodos de ordenação externa listados, foi realizada uma série de testes em arquivos com diferentes tamanhos (contendo 100, 1.000, 10.000, 100.000 e 471.705 registros) e diferentes situações iniciais (podendo estar ordenado ascendentemente, ordenado descendentemente e desordenado aleatoriamente).

Em cada um dos testes, foi realizada a medida de:
* número de transferências (leitura) de registros da memória externa para a memória interna;
* número de transferências (escrita) de registros da memória interna para a memória externa;
* número de comparações entre valores do campo de ordenação dos registros;
* tempo médio de execução do algoritmo.
	
Ao final deste documento, será realizada uma comparação de desempenho de tais algoritmos.

# Execução

Para a compilação do programa foi gerado um arquivo [MAKEFILE](/src/makefile), para compilar o programa corretamente deve ser utilizado o comando `make` na linha de comando.

# Implementação

## 1. Arquivo principal (main)

A primeira coisa que o algoritmo principal irá fazer é recuperar as informações do metodo a ser utilizado, a quantidade de elementos a serem ordenados do arquivo principal a situação em que o arquivo inicial deve estar para ser aplicada a ordenação. O método a ser utilizado sera dado por um valor inteiro de 1 a 4 e será armazenada na variável metodo. Cada valor indica o método implementado a ser utilizado.

A quantidade de elementos a serem ordenados é um valor inteiro que será armazenada na variável inteira quantidade. Seu próprio valor será a quantidade de elementos. A situação em que o arquivo inicial deve estar para a ordenação será um valor inteiro de 1 a 3, e será armazenada na variável situacao. Cada valor significa:
1. arquivo ordenado ascendentemente;
2. arquivo ordenado descendentemente;
3. arquivo desordenado aleatoriamente.

Tendo essas informações, o algoritmo irá fazer a abertura do arquivo principal ([PROVAO.TXT](/PROVAO.TXT)) e criará um arquivo binário temporário `temp.bin` que será o arquivo que será o arquivo a ser ordenado. O arquivo `temp.bin` será gerado pela função `gera_arquivo`, que irá gerá-lo baseado na quantidade de elementos a ser ordenada do arquivo principal.

Tendo o arquivo `temp.bin` gerado, é criado um **vetor de fitas** (ponteiros de arquivos), do tamanho igual a quantidade de fitas, definida pela constante `FF`. Em seguida, cada um desses arquivos são abertos. Esses serão os arquivos que serão usados como as fitas temporárias, que são utilizadas pelos métodos de intercalação balanceada de vários caminhos.

```c
FILE** fitas = malloc(sizeof(FILE*)*FF); // vetor de arquivos/fitas
for(int i = 0; i < FF; i++) {
  char nome[11];
  sprintf(nome, "fita%.2i.bin", i);
  fitas[i] = fopen(nome, "w+b");
}
```

É criado, também, um vetor de inteiros chamado elementos, de tamanho `FF`. Cada posição `i` desse vetor informa o numero de elementos armazenados na fita `i` do vetor de fitas. Outra estrutura auxiliar será um vetor de listas de inteiros chamado blocos. Foi utilizada uma lista pois o numero de blocos que uma fita pode conter é indefinido, por isso há a necessidade de utilizar uma estrutura dinâmica. Cada lista `i` do vetor de listas é a lista de blocos de sua respectiva fita `i`. Cada valor inteiro da lista de blocos informa a posição do primeiro elemento de cada bloco. Com isso, o número de elementos de cada lista diz o número de blocos que cada fita possui.

```c
int elementos[FF]; // vetor de elementos
lista blocos[FF]; // vetor de blocos
```

Antes de fazer a ordenação do arquivo que será analisada, o arquivo `temp.bin` deve estar na situação inicial especificada na inicialização do programa. Para isso, é feita uma ordenação prévia no arquivo `temp.bin`, caso a situação escolhida tenha sido 1 ou 2. Tendo todas as estruturas necessárias para a ordenação e o arquivo temporário na situação desejada, o algoritmo chamará os métodos necessários para a ordenação, baseada no método escolhido pelo usuário.

### Geração do arquivo temporário

A função `gera_arquivo` irá executar um loop de n iterações, sendo n a quantidade especificada na inicialização do programa. A cada iteração, a função irá ler um registro do arquivo [PROVAO.TXT](/PROVAO.TXT) e irá escrevê-lo no arquivo `temp.bin`. Depois disso, o arquivo temporário deve ser ordenado, caso a situação escolhida tenha sido a situação 1 ou 2. No caso da situação 1, o algoritmo ainda irá fazer a inverção da ordem dos registros armazenados. Depois disso, o arquivo temporário estará pronto para ser ordenado.

## 2. Intercalação Balanceada de Vários Caminhos

### Geração dos blocos ordenados

Os métodos de geração dos blocos ordenados foram divididos em duas funções:
* Geração dos blocos ordenados por ordenação;
* Geração dos blocos ordenados por seleção.
As funções funcionam da mesma forma utilizando o modo **2F fitas** e **F+1 fitas**.

### Geração dos blocos ordenados por ordenação

O método de geração de blocos ordenados por ordenação irá ler um um número de registros do arquivo temporário, ordená-lo, usando o um método de ordenação interna, e jogar todos esses registros em uma fita temporária como sendo um bloco de registros. O algoritmo escolhido para fazer essa ordenação foi o **Quicksort**. O algoritmo receberá como parâmetro o número de registros do arquivo temporário `n`, o ponteiro do arquivo temporário `temp`, o vetor de fitas `fitas`, o vetor que possui o numero de elementos `elementos`, o vetor de listas com as informações dos blocos `blocos` e o modo de utilização das fitas `modo`. A variável `modo` armazena valores inteiros de `0` a `1`. Caso a variável `modo` possua o valor `0`, o algoritmo utilizará o modo **2F fitas**, e esse valor sendo `1` o algoritmo utilizará o modo **F+1 fitas**.

```c
void gerarblocos_ordenacao (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo);
```

A modo escolhido irá diferenciar apenas no tamanho da memória interna utilizada para fazer a ordenação. Se o modo for de **2F fitas**, o tamanho da memoria é `F`, e caso o modo seja **F+1 fitas**, o tamanho da memória é `FF-1` (definido pela constante `Fm1`).

```c
int TAMMEMORIA = (modo)? Fm1 : F; // Define o tamanho da memoria relativo ao modo
registro memoria[TAMMEMORIA]; // memoria interna que sera usada
```

Também no pré-processamento, o algoritmo também retorna todos os cursores dos arquivos para a posição `0`, zera todas as posições do vetor de elementos e limpa todas as listas de blocos. Depois disso, algoritmo cria uma variável inteira chamada `fita`, que armazenará o indice da fita temporária no vetor de fitas que estará recebendo o bloco de registros naquela iteração. A variável fita começa com o valor `0`.

Para saber o numero de iterações necessárias para distribuir todos os blocos entre as fitas temporárias, o algoritmo calcula o número de blocos que serão gerados, já que ele sabe o número de registros do arquivo temporário e o tamanho máximo de cada bloco, que é o tamanho da memória interna disponível. Esse valor é armazenado na variável inteira `numblocos`.

```c
int numblocos = (n%TAMMEMORIA == 0)? n/TAMMEMORIA : n/TAMMEMORIA + 1;
```

Enfim, a criação dos blocos começa. A criação dos blocos acontece em um laço de repetição for que terá numblocos iterações. A cada iteração, o algoritmo adiciona um novo bloco na fita alvo (definida pela variável `fita`), adicionando um novo valor na lista de blocos dessa fita. Esse valor é o numero de elementos da fita, que coincidentemente é a posição do cursor dessa fita no momento, e onde o próximo bloco irá começar. Em seguida, o algoritmo verifica o numero de registros que terá no bloco a ser gerado. Isso é feito porquê caso este seja o ultimo bloco do arquivo, e o numero de elementos não seja divisivel pelo tamanho da memória, o tamanho desse bloco será menor que o tamanho da memória. Se esse calculo não for feito, o algoritmo irá inserir dados duplicados nas fitas temporárias. O valor desse cálculo será armazenado na variável inteira `numregistros`.

```c
for(int i = 0; i < numblocos; i++) { 
	// adiciona um novo bloco na fita
	lista_adicionar(&blocos[fita], elementos[fita]);
  // calcula o numero de registros que tera no bloco
  int numregistros = (i == numblocos-1 && n%TAMMEMORIA != 0)? n%TAMMEMORIA : TAMMEMORIA;
```

Em seguida, o algoritmo irá ler os próximos numregistros registros do arquivo temporário e colocá-los na memória. O algoritmo ordena a memória através do Quicksort interno, e escreve os registros em ordem na fita alvo temporária. O numero de elementos da fita também é incrementado.

```c
fread(memoria, sizeof(registro), numregistros, temp); // faz a leitura dos elementos da fita		
sort(memoria, numregistros); // ordena a memoria
fwrite(memoria, sizeof(registro), numregistros, fitas[fita]); // escreve os elementos ordenados
elementos[fita] += numregistros; // incrementa o numero de elementos da fita temporaria
```

Feito isso, o algoritmo deve selecionar a próxima fita e ir para a próxima iteração. A seleção da próxima fita é feita relativa ao modo escolhido (2F fitas ou F+1 fitas).

```c
if(modo == 0) // 2F fitas
	fita = (fita+1)%(FF/2); 
else if(modo == 1) // F+1 fitas
	fita = (fita+1)%(FF-1);
```

Ao final do laço de repetição, o algoritmo termina.

### Geração dos blocos ordenados por seleção

O algoritmo de criação dos blocos ordenados por seleção irá fazer jogar o menor elemento da memória na fita alvo, pegar o próximo elemento do arquivo temporário e fazer esse processo até pegar todos os registros do arquivo temporário. Caso o próximo elemento obtido do arquivo temporário seja menor que o registro que acabou de ser inserido na fita temporária, esse registro é marcado. Quando todos os registros da memória estão marcados, o algoritmo finaliza esse bloco, começa a inserção de um novo bloco na próxima fita temporária. O algoritmo de geração dos blocos por seleção recebe os mesmos parâmetros do algoritmo de geração dos blocos por ordenação.

```c
void gerarblocos_selecao (int n, FILE *temp, FILE **fitas, int elementos[FF], lista blocos[FF], short modo);
```

Assim como o algoritmo de criação dos blocos por ordenação, o esse algoritmo cria a memória interna como um vetor de registros, e seu tamanho é relativo ao modo escolhido (2F ou F+1 fitas). Uma das estruturas novas desse algoritmo é o vetor `marcados`, que diz se um elemento da memória está marcado ou não. Também é criada a variável `proximo`, que é utilizada para guardar o próximo elemento lido do arquivo temporário antes que ele seja jogado na memória, para que possa ser feita a comparação dele com o elemento que será removido da memória.

Antes de entrar no laço de repetição principal, o algoritmo pega os primeiro `TAMMEMORIA` elementos do arquivo temporário e joga-os no vetor de memória interna. O algoritmo também inicializa todos os valores de marcados como 0.

```c
//pega os 'TAMMEMORIA' primeiros elementos do arquivo
fread(memoria, sizeof(registro), TAMMEMORIA, temp);
for(int i = 0; i < TAMMEMORIA; i++) marcados[i] = 0;
```

Enfim, o algoritmo entra no primeiro laço principal. O laço irá se repetir enquanto houver registros no arquivo temporário que não foram lidos e enquanto houver registros na memória que não foram transferidos para uma fita temporária. 

No inicio desse laço, o algoritmo começa desmarcando todas os registros que foram marcados com `1` durante a geração dos blocos, que acontece no laço a seguir. Os registros podem ser marcados com os valores `0`, `1`, e `-1`. Registros com a marcação `0` significa que o registro não está marcado. Registros com a marcação `1` significa que ele foi marcado por ser menor que o último registro removido da memória. Registros com a marcação `-1` são registros que foram removidos da memória e não existia mais registros no arquivo temporário para ser colocado no lugar, isso acontece quando o arquivo temporário é esgotado. Essa marcação diferente é necessária para essa etapa de remover a marcação dos registros. Um registro marcado com `1` ainda não foi transferido para uma fita temporária, ao contrário do registro marcado com `-1`. Por isso, nessa etapa, apenas os registros marcados com `1` são desmarcados. Antes de entrar no próximo laço de repetição, o algoritmo também cria um novo bloco para a fita alvo.

```c
// Enquanto nao percorrer todo o arquivo ou houver elementos na memoria
while (ftell(temp)/sizeof(registro) < n || temElementoNaMemoria(marcados, TAMMEMORIA)) {
	for(int i = 0; i < TAMMEMORIA; i++) 
		if(marcados[i] == 1) marcados[i] = 0; // desmarca todos que estavam na memoria	
  lista_adicionar(&blocos[fita], elementos[fita]); // adiciona um novo bloco para a fita alvo
```

O próximo laço de repetição fará a ordenação da memória, transferirá o menor elemento para a fita alvo e fará a leitura do próximo registro do arquivo temporário. Esse laço termina quando todos os registros da memória estiverem marcados (com `1` ou `-1`). O algoritmo começa ordenando a memória, para obter o menor elemento. Como o único registro de interesse é o menor, o algoritmo de ordenação utilizado nessa etapa é o **Heapsort**. Feito a ordenação, o menor registro é escrito no fita temporária alvo, e o tamanho da mesma é incrementado. 

```c
heapsort_marcados(memoria, marcados, TAMMEMORIA, modo); // ordena a memoria
fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor registro na fita alvo
elementos[fita]++;
```

Logo após, é feita a marcação. Antes de ler o próximo elemento do arquivo temporário, deve ser verificado se ainda há registro a ser lido, ou se todos eles já foram lidos. Caso não haja, o algoritmo simplesmente marca o primeiro elemento do vetor marcados com `-1`. Caso contrário, o algoritmo faz a leitura do próximo elemento do arquivo temporário, jogando o registro lido na variável `proximo`. Caso o registro `proximo` seja menor que o registro removido da memória (registro na posição `0` da memória), o algoritmo marca o primeiro elemento do vetor de marcados com `1`. Depois da verificação, o registro armazenado na variável `proximo` é armazenado na primeira posição da memória.

```c
if(ftell(temp)/sizeof(registro) < n) { // se o arquivo nao acabou
	fread(&proximo, sizeof(registro), 1, temp); // lê o proximo elemento do arquivo
	if(proximo.nota < memoria[0].nota)  // se o proximo elemento for menor que o primeiro elemento da memoria
		marcados[0] = 1; // marca o primeiro elemento
	memoria[0] = proximo; // joga o proximo elemento na primeira posição da memoria
} else {
	marcados[0] = -1; // marca o primeiro elemento (nao tem elemento na memoria)
}
```

Depois da marcação feita, o laço de repetição interno termina. Depois dele, é feita a seleção da próxima fita alvo, do mesmo modo que foi feito no algoritmo de criação dos blocos por ordenação.

```c
if(modo == 0) // 2F fitas
	fita = (fita+1)%(FF/2); 
else if(modo == 1) // F+1 fitas
	fita = (fita+1)%(FF-1);
```

Depois da seleção da seleção da próxima fita, o laço de repetição principal termina, e também a função de criação dos blocos.

### Intercalação dos blocos ordenados

A intercalação dos blocos foi dividida em duas funções que atuam em situações diferentes. São elas:
* `intercalablocos`: Função de intercalação dos blocos utilizada no modo **2F fitas**;
* `intercalablocos_f`: Função de intercalação dos blocos utilizada no modo **F+1 fitas**.

#### Intercalação dos blocos ordenados utilizando 2F fitas

O algoritmo de intercalação dos blocos, quando utilizado **2F fitas**, intercala o primeiro bloco de cada fita de entrada e joga o bloco intercalado na fita de saida alvo. Quando todos os blocos das fitas de entrada são intercalados, as fitas de entrada se tornam as fitas de saida, e as fitas de saida se tornam as fitas de entrada, e o processo recomeça. O algoritmo termina quando só existe uma fita de saida preenchida. Essa fita estará com todos os elementos ordenados.

O algoritmo começa criando as estruturas necessárias para o processo de intercalação. Uma delas é o vetor de registros utilizado como memória interna, de tamanho `F`, definido como uma constante sendo a metade de `FF`. Um vetor de inteiros chamado `from` será criado, de tamanho `F`. Esse vetor dirá, para cada registro na memória, o indice da fita de que ele veio. Outro vetor de tamanho `F`, agora de `short`, será criado, chamado `ativas`. Esse vetor informará a situação do bloco que está sendo lido de cada fita de entrada no processo de leitura. Se o bloco ainda possuir elementos, o valor daquela fita é `1`. Se a fita teve o ultimo elemento do bloco lido, o valor daquela fita passa a ser `0`. E se o algoritmo tenta ler um registro do bloco de uma fita que possui o valor `0`, essa fita recebe o valor `-1`.

Além dessas estruturas, o algoritmo cria a variável inteira fita, que armazenará o indice da fita de saída alvo. Outra variável é a `saltofitas`. Essa variável dirá a distribuição das fitas de entrada e saida. Quando essa variável valer `0`, significa que as primeiras `F` fitas são as fitas de saída, e as outras são as fitas de entrada. Quando essa variável valer `1`, as primeiras `F` fitas são as fitas de entrada, e as outras são as fitas de saída. Essa variável inicia valendo `1`, pois os algoritmos de criação dos blocos ordenados criam os blocos nas primeiras `F` fitas, quando é utilizado o modo **2F fitas**. Além disso, os cursores de todas as fitas temporárias são retornados para a posição inicial.

```c
registro memoria[F];
int from[F];
short ativas[F];
short saltofitas = 1;
int fita;
for(int i = 0; i<FF; i++) rewind(fitas[i]);
```

Depois disso, o algoritmo entra no laço de repetição principal, que roda até restar uma única fita de saida preenchida. Essa verificação é feita pela função `restaUmaFitaPreenchida`, que retorna o indice da fita, caso ela seja a única fita preenchida. Caso essa fita não exista, ela retorna `-1`. O retorno dessa fita é armazenado em uma variável inteira `f` que é utilizada no final do algoritmo, para transferir os registros da fita de saida para o arquivo temporário. Então, o laço roda enquando `f` for igual a `-1`.

```c
int restaUmaFitaPreenchida(int nElementos[], int n, int inicio) {
	int preenchida = -1;
	for(int i = 0; i < n; i++) {
		if(nElementos[inicio+i] > 0) {
			if(preenchida == -1) preenchida = i;
			else return -1;
		}
	}
	return (preenchida == -1) ? preenchida : inicio + preenchida;
}
```

```c
int f = restaUmaFitaPreenchida(elementos, F, F*saltofitas);
while (f == -1) { /* … */
```

No inicio do laço, o algoritmo seleciona a primeira fita das fitas de saida como sendo a fita alvo. Cria a variável inteira `bloco`, que irá dizer em qual posição estão os blocos que estão sendo intercalados na iteração. Essas informações serão utilizadas no laço de repetição interno a seguir.

```c
fita = F*saltofitas; // alveja a primeira fita de saida
int bloco = 0;
```

Dentro desse laço de repetição principal, temos um laço de repetição interno. Ele irá rodar enquanto todas as fitas de entrada não forem percorridas. Dentro desse laço, o algoritmo começa adicionando um novo bloco na fita de saida selecionada. Depois, o algoritmo cria um vetor de inteiros chamado `limiteblocos` de tamanho `F`, que armazena em que posição do arquivo termina o bloco a ser intercalado de cada fita. Esse cálculo deve ser feito, assim como na etapa de criação dos blocos, porquê nem sempre os blocos terão o tamanho igual o tamanho da memória interna.

```c
lista_adicionar(&blocos[fita], ftell(fitas[fita])/sizeof(registro)); // adiciona um novo bloco na fita alvo
int limiteblocos[F]; // indice onde termina o bloco de cada fita
for(int i = 0; i<F; i++) {
	if (lista_tamanho(&blocos[(F*!saltofitas)+i]) > bloco+1)
		limiteblocos[i] = lista_posicao(&blocos[(F*!saltofitas)+i], bloco+1);
	else
		limiteblocos[i] = elementos[(F*!saltofitas)+i];
}
```

Depois disso, o algoritmo faz a leitura do primeiro elemento do bloco a ser intercalado de cada uma das `F` fitas. Ao mesmo tempo, a inicialização dos valores do vetor ativas é feito.

```c
for (int i = 0; i < F; i++) {
// tem elemento no bloco?
	if(ftell(fitas[(F*!saltofitas)+i])/sizeof(registro) < limiteblocos[i]) { 
// pega o primeiro elemento
		fread(&memoria[i], sizeof(registro), 1, fitas[(F*!saltofitas)+i]); 
// foi o ultimo elemento?
		if (ftell(fitas[(F*!saltofitas)+i])/sizeof(registro) >= limiteblocos[i]) 
			ativas[i] = 0;
		else
			ativas[i] = 1;
	}
	else { ativas[i] = -1; }
	from[i] = i+(!saltofitas*F);
}	
```

Depois dessa inicialização, o laço de repetição responsável pela intercalação dos blocos é inicializado, que rodará enquanto um bloco não tiver sido esgotado (enquanto houver um valor de ativas diferente de `-1`). O laço começa ordenando a memória, para obter o menor elemento. O método de ordenação utilizado é o **Heapsort**, como o menor elemento é o único elemento que interessa o algoritmo. Depois disso, o algoritmo joga o menor registro da memória na fita alvo.

```c
heapsort(memoria, ativas, from, F, 0); // ordena a memoria
fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor elemento na fita alvo
elementos[fita]++; // aumenta o tamanho da fita
```

Em seguida, o algoritmo faz a leitura do próximo elemento da fita de origem do registro que foi removido da memória, se possível. Se a fita do menor elemento não está mais ativa (se o valor de ativas daquela fita for `0`, significa que ela não possui mais registros daquele bloco a serem lidos), o valor de ativas daquela fita se torna `-1`. Caso contrário, o algoritmo faz a leitura do próximo registro daquela fita. Se esse registro lido tiver sido o último daquele bloco, o valor de ativas daquela fita se torna `0`.

```c
if(ativas[from[0]%F]) { // se a fita do menor está ativa
	fread(&memoria[0], sizeof(registro), 1, fitas[from[0]]); // pega o proximo elemento
// se tiver sido o ultimo elemento
	if(ftell(fitas[from[0]])/sizeof(registro) == limiteblocos[from[0]%F]) 
		ativas[from[0]%F] = 0; // a fita se torna inativa
} else {
	ativas[from[0]%F] = -1; // bloco esgotado
}
```

Depois disso, esse laço de repetição interno termina. Depois dele, antes de chegar ao fim do laço de repetição externo, o algoritmo seleciona a próxima fita de saida alvo, e aumenta o valor da variável `bloco` em `1`.

```c
fita = (fita%F+1)%F + F*saltofitas; // proxima fita alvo
bloco++;
```

Depois disso, o laço de repetição externo termina. O algoritmo deve retornar os cursores de todas as fitas temporárias, zerar o numero de elementos de todas as fitas de entrada, limpar todos os blocos das fitas de entrada e fazer a inversão de fitas de entrada e fitas de saida. O algoritmo também calcula o valor de `f`, que armazena a condição de parada do laço de repetição principal. Feito isso, o laço de repetição principal termina.

Caso o algoritmo saia do laço principal, significa o processo resultou em uma única fita de saida preenchida, e é a fita de índice `f` do vetor de fitas temporárias. O algoritmo agora fará a transferência de todos os registros da fita `f` para o arquivo temporário, e enfim o algoritmo termina.

```c
rewind(fitas[f]);
rewind(temp);
registro r;
// joga todos os elementos da fita f na fita principal
for(int i = 0; i < n; i++) {
	fread(&r, sizeof(registro), 1, fitas[f]);
	fwrite(&r, sizeof(registro), 1, temp);
}
```

#### Intercalação dos blocos ordenados utilizando F+1 fitas

No modo F+1 fitas, todas as fitas exceto a última são fitas de entrada, e a última fita é a fita de saída. Na intercalação, todos os blocos intercalados são jogados na fita de saída e, no final de cada intercalação, todos os blocos da fita de saída são redistribuidos entre as fitas de entrada. Quando a fita de saída possua apenas um bloco ordenado, esse bloco possuirá todos os elementos do arquivo, então o algoritmo finalizará transferindo todos os registros da fita de saida para o arquivo temporário.

A função `intercalablocos_f1` recebe os mesmos argumentos da função `intercalablocos`. Essa função também irá criar os vetores `memoria`, `from` e `ativas`, mas nessa função eles tem o tamanho `FF-1`, definido pela constante `Fm1`. A função também cria uma variável curta chamada finalizado, usada para armazenar o valor que diz se o processo de intercalação finalizou.

```c
registro memoria[Fm1];
int from[Fm1];
short ativas[Fm1];
int fita = Fm1;
short finalizado = 0;
```

Tendo todas as estruturas criadas, o algoritmo entra no laço principal de repetição, que termina quando a fita de saida possuir apenas um bloco de registros. O laço começa retornando os cursores de todas as fitas para o inicio, e criando a variável `bloco` com o valor `0`. Essa variável, assim como na função de intercalação dos blocos utilizando **2F fitas**, diz a posição dos blocos a serem intercalados na lista de blocos das fitas de entradas. Depois disso, o algoritmo entra em um outro laço de repetição, que repetirá enquanto não tiver percorrido todas as fitas de entrada.

Esse laço começa como na função de intercalação dos blocos utilizando **2F fitas**. Ele cria um novo bloco para a fita de saida, cria um vetor de inteiros com valor da ultima posição de cada bloco que será intercalado, e pega o primeiro elemento de cada fita de entrada, preenchendo os vetores `ativas` e `from`.

```c
lista_adicionar(&blocos[fita], ftell(fitas[fita])/sizeof(registro)); // adiciona um novo bloco na fita alvo
int limiteblocos[Fm1]; // indice onde termina o bloco de cada fita
for(int i = 0; i<Fm1; i++) {	
	if (lista_tamanho(&blocos[i]) > bloco+1)
		limiteblocos[i] = lista_posicao(&blocos[i], bloco+1);
	else
		limiteblocos[i] = elementos[i];
}
// Pega o primeiro elemento de cada fita de entrada
for (int i = 0; i<Fm1; i++) {	
	if(ftell(fitas[i])/sizeof(registro) < limiteblocos[i]) { // tem elemento no bloco?
		fread(&memoria[i], sizeof(registro), 1, fitas[i]); // pega o primeiro elemento
		if (ftell(fitas[i])/sizeof(registro) >= limiteblocos[i]) // foi o ultimo elemento?
			ativas[i] = 0;
		else
			ativas[i] = 1;
	}
	else ativas[i] = -1;
	from[i] = i;
}
```

Feito isso, o algoritmo entra em outro laço de repetição, que fará a intercalação dos blocos. O laço termina quando todos os blocos forem esgotados (também, como na função de intercalação dos blocos utilizando **2F fitas**). No laço, o algoritmo ordena a memória, escreve o menor elemento na fita de saída e faz a leitura do próximo elemento, se possível. Também atualiza o valor da primeira posição do vetor `ativas`, se necessário.

```c
heapsort(memoria, ativas, from, Fm1, 1); // ordena a memoria
fwrite(&memoria[0], sizeof(registro), 1, fitas[fita]); // joga o menor elemento na fita alvo
elementos[fita]++; // atualiza o tamanho da fita de saida
if(ativas[from[0]]) { // se a fita do menor está ativa
	fread(&memoria[0], sizeof(registro), 1, fitas[from[0]]); // pega o proximo elemento
	// se tiver sido o ultimo elemento
	if(ftell(fitas[from[0]])/sizeof(registro) == limiteblocos[from[0]]) 
		ativas[from[0]] = 0; // a fita se torna inativa
} else {
	ativas[from[0]] = -1; // bloco esgotado
}
```

Depois disso, esse laço chega ao fim. Saindo desse laço, no laço externo, o algoritmo apenas incrementa o valor da variável `bloco` em `1`, e esse laço também chega ao fim. Nessa parte, todos os blocos foram intercalados e armazenados na fita de saida. O algoritmo agora vai verificar se a fita de saida possui mais de um bloco de registros. Caso ela possua mais de um bloco, ela deve fazer a distribuição desses blocos entre as fitas de entrada. Para isso, os cursores de todas as fitas voltam para o inicio do arquivo, o numero de elementos de todas as fitas de entrada se torna 0 e a lista de blocos de todas as fitas de entrada são esvaziadas.

Agora as fitas estão prontas para que a distribuição possa ser feita. A distribuição será feita por um laço de repetição, que terá o número de iterações igual ao numero de blocos da fita de saida. A cada iteração, uma fita de entrada é selecionada para receber os registros. O algoritmo começa criando um novo bloco para essa fita. Depois o algoritmo calcula em que posição o bloco de registros termina na fita de saida, e usa esse valor para fazer a leitura de todos os elementos da fita de saida e escreve-los na fita de entrada, enquanto o cursor da fita de saida não alcançar essa posição.

```c

for(int i = 0; i < lista_tamanho(&blocos[fita]); i++) {
	int j = i%Fm1;
	// adiciona um novo bloco na fita de entrada
	lista_adicionar(&blocos[j], ftell(fitas[j])/sizeof(registro)); 
	int limitebloco; // onde termina o bloco a ser lido
	if(lista_tamanho(&blocos[fita]) > i+1)
		limitebloco = lista_posicao(&blocos[fita], i+1);
	else
		limitebloco = elementos[fita];
	// joga os elementos do bloco da fita de saida para a fita de entrada 'j'
	while(ftell(fitas[fita])/sizeof(registro) < limitebloco) {
		fread(&r, sizeof(registro), 1, fitas[fita]);
		fwrite(&r, sizeof(registro), 1, fitas[j]);
		elementos[j]++; // atualiza o tamanho da fita 'j'
	}
}
```

Feito isso, o algoritmo limpa a fita de saida e volta o cursor da mesma para o inicio do arquivo. Com isso, o laço principal de repetição é finalizado. Nesse momento, a fita de saida deve conter um único bloco, com todos os elementos do arquivo ordenados. O algoritmo só precisa fazer a transferência dos registros da fita de saída para o arquivo temporário.

```c
rewind(temp);
rewind(fitas[fita]);
for(int i = 0; i < n; i++) {
	// joga todos os elementos da fita f na fita principal
	fread(&r, sizeof(registro), 1, fitas[fita]);
	fwrite(&r, sizeof(registro), 1, temp);
}
```

## 3. Quicksort Externo

O quicksort externo é implementado nos arquivos `quicksort_externo.c` e `quicksort_externo.h`. A ordenação é feita através de múltiplas divisões no arquivo, gerando o particionamento do arquivo em 3 diferentes áreas: **pivô**, **área esquerda** e **área direita**. O **pivô** apresenta os valores ordenados em sua posição final dentro da ordenação geral do arquivo, enquanto que a **área esquerda** apresenta os valores menores que o pivô, e a **área direita** representa os valores maiors que os contidos no pivô. Tal particionamento é essencial para a execução do algoritmo, sendo realizado pela seguinte função:

```c
void Particao(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, TArea Area, int Esq, int Dir, int*i, int*j, int* transf, int* comp) {
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
```

O algoritmo do quicksort externo é baseado em múltiplas chamadas do particionamento, sendo chamado recursivamente nas partições a direita e a esquerda, conforme a função a seguir:

```c

void QuickSortExterno(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, int Esq, int Dir, int* transf, int* comp) {
    assert(ArqLi);
    assert(ArqEi);
    assert(ArqLEs);
    int i, j;
    TArea Area; /*Área de armazenamento interna*/
    if (Dir - Esq < 1)
        return;
    FAVazia(&Area);  //esvazia memória
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
```

# Testes

Para a etapa de testes o programa foi rodado em cima do arquivo [PROVAO.TXT](/PROVAO.TXT. Foram medidos os seguintes dados em cada execução:
* número de transferências de leitura e escrita da memória externa para a memória interna;
* número de comparações entre os valores de campo de ordenação dos registros;
* tempo de execução do algoritmo, frisando que o tempo de criação do arquivo texto temporário com **n** itens não será contabilizado nessa etapa.

O número de transferência e de comparações foi obtido através de uma variável acumuladora que é passada em cada função. Cada medida de tempo foi obtida através da média aritmética dos valores obtidos através da execução do programa 10 vezes. Foram obtidas medidas para a execução programa para **n** igual a `100`, `1.000`, `10.000`, `100.000` e `471.705`.

## Intercalação balanceada de vários caminhos utilizando ordenação interna na geração dos blocos ordenados (quicksort) e 2F fitas

Resultados do algoritmo aplicado ao arquivo nas situações: **ordenado ascendentemente**, **ordenado descendentemente** e **desordenado aleatoriamente**.

### Arquivo ordenado ascendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |15751        |1204           |0.00142700 |
|1.000      |227881       |18411          |0.01728400 |
|10.000     |2507396      |202571         |0.07329500 |
|100.000    |31276926     |2724228        |1.19917095 |
|471.705    |171173012    |16151010       |14.61077499|

### Arquivo ordenado descendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |16127        |1204           |0.00168600 |
|1.000      |235445       |18411          |0.00515600 |
|10.000     |2616609      |202571         |0.07672200 |
|100.000    |32532828     |2724228        |0.89940202 |
|471.705    |176702948    |16151010       |14.56313229|

### Arquivo desordenado aleatoriamente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |4114         |462            |0.00465500 |
|1.000      |73399        |6272           |0.00628500 |
|10.000     |972694       |82192          |0.19580799 |
|100.000    |11992258     |1021212        |1.24907506 |
|471.705    |63430731     |5760167        |15.78745270|

Observando os resultados, podemos ver que os melhores resultados foram obtidos quando a situação inicial do arquivo é aleatória. Justificamos isso analisando nosso algoritmo de ordenação interna escolhido, que foi o **Quicksort**, que tem como seu pior caso a ordenação de um vetor já ordenado, podendo estar ordenado tanto crescentemente quanto decrescentemente. Isso porquê, quando o arquivo o vetor está ordenado, no particionamento, ele sempre irá jogar um unico elemento para uma das partições e o restante para a outra.

Na análise de todos os modos de utilização da Intercalação balanceada de vários caminhos utilizados nesse trabalho, observamos que tempo de execução de todos os métodos foi maior quando o arquivo inicial estava desordenado, mesmo sempre tendo um menor número de comparações e transferências. A resposta mais coerente que obtivemos na comunidade (um resumo da discussão estará no fim desse relatório) foi de um possível uso de branch prediction utilizado pelo compilador, sabendo que os valores estão ordenados. Isso reduz consideravelmente o tempo de execução do algorítmo.

## Intercalação balanceada de vários caminhos utilizando a técnica de substituição por seleção na geração dos blocos ordenados e 2F fitas

Resultados do algoritmo aplicado ao arquivo nas situações: **ordenado ascendentemente**, **ordenado descendentemente** e **desordenado aleatoriamente**.

### Arquivo ordenado ascendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |20578        |1385           |0.00181000 |
|1.000      |263378       |20271          |0.00493500 |
|10.000     |2767215      |220711         |0.08077000 |
|100.000    |30836818     |2703415        |0.69668698 |
|471.705    |158553669    |15108719       |5.75445890 |

### Arquivo ordenado descendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |19499        |1376           |0.00108900 |
|1.000      |241163       |18173          |0.00334400 |
|10.000     |2294227      |180413         |0.03740900 |
|100.000    |26548730     |2303050        |0.43774700 |
|471.705    |138201425    |13221107       |2.28028202 |

### Arquivo desordenado aleatoriamente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |7714         |639            |0.00231300 |
|1.000      |108570       |8107           |0.00609500 |
|10.000     |1311550      |100606         |0.09091200 |
|100.000    |15409863     |1205501        |1.15657198 |
|471.705    |79532873     |6629202        |8.16820812 |

Podemos observar que o método de geração dos blocos utilizando substituição por seleção é um método muito mais eficiente do que utilizando ordenação interna. Essa troca reduziu muito o tempo de execução do algoritmo, mesmo o número de comparações e transferências sendo levemente maior na maioria dos casos.

## Intercalação balanceada de vários caminhos utilizando a técnica de substituição por seleção na geração dos blocos ordenados e F+1 fitas

Resultados do algoritmo aplicado ao arquivo nas situações: **ordenado ascendentemente**, **ordenado descendentemente** e **desordenado aleatoriamente**.

### Arquivo ordenado ascendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |26842        |1484           |0.00105800 |
|1.000      |327213       |24298          |0.02146600 |
|10.000     |3150577      |240698         |0.06965800 |
|100.000    |33715150     |2903415        |0.72805899 |
|471.705    |189116766    |18410684       |4.05806017 |

### Arquivo ordenado descendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |24299        |1484           |0.00044000 |
|1.000      |288099       |19181          |0.00475400 |
|10.000     |2713955      |190421         |0.02895600 |
|100.000    |29880066     |2403058        |0.28264001 |
|471.705    |153609417    |13692820       |2.01439309 |

### Arquivo desordenado aleatoriamente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |13278        |742            |0.00137400 |
|1.000      |181642       |12139          |0.00745500 |
|10.000     |1830546      |120379         |0.10550000 |
|100.000    |22294249     |1703016        |0.98784000 |
|471.705    |118257114    |10390843       |6.16571379 |

Podemos comparar esse método diretamente com o método anterior, onde teve a do uso de **2F fitas** para **F+1 fitas**, utilizando o mesmo método de geração dos blocos ordenados. Os resultados foram bem similares, tendo um leve aumento no número de comparações e transferências, porem o tempo obtido foi levemente menor. Isso porquê, quando é utilizado o modo **F+1 fitas**, o número de passadas das fitas é sempre maior.

Nesse caso, trabalhando com arquivos no disco rígido, o tempo foi menor, mas a utilização de um dispositivo de armazenamento com leitura e escrita mais lenta poderia ocorrer num aumento no tempo de execução do algorítmo utilizando o modo **F+1 fitas**.

## Quicksort Externo

### Arquivo ordenado ascendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |16497        |842            |0.00045000 |
|1.000      |234724       |13589          |0.02590100 |
|10.000     |2507166      |152196         |0.44369400 |
|100.000    |26004980     |2066791        |4.98578882 |
|471.705    |136631253    |12065209       |21.70017242|

### Arquivo ordenado descendentemente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |16547        |842            |0.00037700 |
|1.000      |235157       |13589          |0.03087900 |
|10.000     |2508752      |152196         |0.57720399 |
|100.000    |26006490     |2066791        |5.04941893 |
|471.705    |136635481    |12065209       |25.62002945|

### Arquivo desordenado aleatoriamente

|Quantidade |Comparações  |Transferências |Tempo      |
|-----------|-------------|---------------|-----------|
|100        |2846         |100            |0.00038000 |
|1.000      |71905        |4433           |0.07273500 |
|10.000     |668927       |88691          |1.22180796 |
|100.000    |4858651      |1240459        |15.13516426|
|471.705    |18710461     |5863066        |78.57408905|

Inicialmente podemos notar que o algoritmo funciona de forma bem próxima quando os arquivos estão ordenados ascendentemente e descendentemente. O número de comparações e transferências é bem próximo, tendo uma pequena diferença no tempo decorrido de cada um. Isso se dá devido ao sistema de **particionamento do algoritmo quicksort**. O particionamento ocorre separando as porções maiores e menores que os elementos contidos no pivô, dessa forma haverá uma diferença muito pequena no número de comparações e transferências, pois tanto o array pivô, quanto os arrays maiores e menores serão de igual tamanho tanto para arquivos pré-ordenados ascendentemente quanto para descendentemente.

Um outro fato interessante é que os arquivos ordenados de forma aleatória possuem um número menor de comparações e transferência, algo já esperado, mas possuem um tempo de execução muito maior que nos outros casos, o que pareceu muito contra-intuitivo. Este fenômeno pode ser explicado por questões de implementações do cache e do compilador, e serão explicados mais a frente.

## Branch Prediction

Um fato observado em todos os métodos estudados é que apesar de possuirem um número maior de comparações e transferências, as ordenações realizadas em arquivos pré-ordenados são executadas de forma mais rápida que as ordenações em arquivos organizados de forma aleatória.

Esse fato foi contrário à expectativa geral dos integrantes do grupo, pois o
esperado é que um programa com um número menor de instruções seja executado de forma mais rápida.

Após uma pesquisa, a explicação encontrada para este fenômeno é a do **branch prediction**, que é um comportamento dos computadores modernos que os permite “prever” comportamentos futuros para então otimizar a execução do código. A figura abaixo representa um **if** e sua implementação em **assembly**.

![branch_prediction](/assets/branch_prediction.png "Branch prediction")

Ao ver o comando de *branch* (salto), o processador não sabe o que fazer, assim deve esperar para que a próxima instrução esteja completa, para então continuar a execução do código.

Uma estratégia para minimizar este tempo de espera é basear a predição no conteúdo da memória cache, assim, se tal predição estiver correta, o processador não precisa esperar pelo término da execução do branch para realizar o salto, podendo continuar a execução do programa de forma mais rápida.

Desta forma, arquivos pré-ordenados favorecem essa otimização do processador, tornando as execuções muito mais rápidas, mesmo que possuam um numero maior de instruções internas.

# Comparação dos métodos

Através da análise dos dados obtidos, pode-se constatar que os métodos de **intercalação balanceada** apresentaram um desempenho melhor que o **Quicksort externo**. Entretanto vale lembrar que eles necessitam de fitas externas de memória, tornando assim o processo mais custoso, enquanto que o **QuickSort externo** realiza todas as operações sem o uso de fitas de memória auxiliares.

Já uma comparação entre as diferentes formas de intercalação balanceada mostraram que a geração dos blocos ordenados através de **ordenação interna** são mais ineficientes que o **método da substituição**. Paralelamente, pode-se constatar que o uso de **f+1 fitas** constitui uma perda pequena no desempenho geral do programa em comparação com o uso de **2f fitas**, tornando assim uma boa opção pela necessidade menor de fitas de disco.

# Referências

* [Yee, Alexander. "What is Branch Prediction? - Stack Overflow Example"](https://stackoverflow.com/questions/11227809/why-is-processing-a-sorted-array-faster-than-processing-an-unsorted-array#11227902);
* [Computer Systems: A Programmer's Perspective (2nd Edition): Randal E. Bryant, David R.](https://piazza-resources.s3.amazonaws.com/j7ly9riuca97on/ja86xbbpp0b73b/Randal_E._Bryant_David_R._OHallaron_Computer_SBookZZ.org.pdf);
* [N. ZIVIANI. Projeto de Algoritmos com Implementações em Pascal e C. Cengage Learning, 2010.](http://clip2net.com/clip/m1076/1206625061-ebook-projetos-de-algoritmos-com-implementazhes-em-pascal-e-c-nivio-ziviani-4ed-3618kb.pdf)
