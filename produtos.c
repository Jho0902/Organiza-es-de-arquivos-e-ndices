#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_ARQUIVO "produtos.bin"
#define NOME_ARQUIVO_INDICE "produtos.idx"
#define NOME_ARQUIVO_TEMP "temp_produtos.bin"

// Estrutura para armazenar um registro temporariamente
typedef struct {
    char id[9];    
    char nome[41];
    char preco[11];
} Produto;

typedef struct {
    char id[9];
    long posicao;
} EntradaIndice;

// Fun��o de compara��o para ordenar os �ndices
int compararEntradas(const void *a, const void *b) {
    return strncmp(((EntradaIndice *)a)->id, ((EntradaIndice *)b)->id, 8);
}

// Fun��o para criar o arquivo de �ndice
void criarIndice() {
    FILE *arquivoDados = fopen(NOME_ARQUIVO, "rb");
    if (!arquivoDados) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }

    // Determine o n�mero de produtos
    fseek(arquivoDados, 0, SEEK_END);
    long numProdutos = ftell(arquivoDados) / sizeof(Produto);
    fseek(arquivoDados, 0, SEEK_SET);

    // Alocar mem�ria para todas as entradas de �ndice
    EntradaIndice *entradas = malloc(numProdutos * sizeof(EntradaIndice));
    if (!entradas) {
        perror("Erro ao alocar mem�ria");
        fclose(arquivoDados);
        return;
    }

    Produto produto;
    long posicao = 0;
    int i = 0;
    while (fread(&produto, sizeof(Produto), 1, arquivoDados) == 1) {
        strncpy(entradas[i].id, produto.id, 9);
        entradas[i].posicao = posicao;
        posicao++;
        i++;
    }
    fclose(arquivoDados);

    // Ordenar as entradas pelo ID
    qsort(entradas, numProdutos, sizeof(EntradaIndice), compararEntradas);

    // Escrever as entradas ordenadas no arquivo de �ndice
    FILE *arquivoIndice = fopen(NOME_ARQUIVO_INDICE, "wb");
    if (!arquivoIndice) {
        perror("Erro ao abrir o arquivo de �ndice");
        free(entradas);
        return;
    }
    fwrite(entradas, sizeof(EntradaIndice), numProdutos, arquivoIndice);
    fclose(arquivoIndice);

    // Liberar mem�ria alocada
    free(entradas);
}

// Fun��o para inserir um novo produto
void inserirProduto(const char *id, const char *nome, const char *preco) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "ab");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    Produto produto;
    snprintf(produto.id, sizeof(produto.id), "%-8s", id);
    snprintf(produto.nome, sizeof(produto.nome), "%-40s", nome);
    snprintf(produto.preco, sizeof(produto.preco), "%-10s", preco);

    fwrite(&produto, sizeof(Produto), 1, arquivo);
    fclose(arquivo);

    // Reconstruir o �ndice ap�s inser��o
    criarIndice();
}

// Fun��o para remover um produto por ID
void removerProduto(const char *id) {
    FILE *arquivoDados = fopen(NOME_ARQUIVO, "rb");
    FILE *arquivoTemp = fopen(NOME_ARQUIVO_TEMP, "wb");
    if (!arquivoDados || !arquivoTemp) {
        perror("Erro ao abrir os arquivos");
        return;
    }

    Produto produto;
    int encontrado = 0;
    while (fread(&produto, sizeof(Produto), 1, arquivoDados) == 1) {
        if (strncmp(produto.id, id, 8) != 0) {
            fwrite(&produto, sizeof(Produto), 1, arquivoTemp);
        } else {
            encontrado = 1;
        }
    }

    fclose(arquivoDados);
    fclose(arquivoTemp);

    if (encontrado) {
        remove(NOME_ARQUIVO);
        rename(NOME_ARQUIVO_TEMP, NOME_ARQUIVO);
        // Reconstruir o �ndice ap�s remo��o
        criarIndice();
    } else {
        // Se n�o encontrou, remova o arquivo tempor�rio
        remove(NOME_ARQUIVO_TEMP);
        printf("Produto com ID %s n�o encontrado para remo��o.\n", id);
    }
}

// Fun��o para mostrar todos os registros do arquivo
void mostrarProdutos() {
    FILE *arquivo = fopen(NOME_ARQUIVO, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    Produto produto;
    while (fread(&produto, sizeof(Produto), 1, arquivo) == 1) {   
        printf("ID Produto: %.8s\n", produto.id);
        printf("Nome: %.40s\n", produto.nome);
        printf("Pre�o: %.10s\n", produto.preco);
        printf("-----------------------------------\n");
    }
    fclose(arquivo);
}

// Fun��o para realizar uma busca bin�ria no arquivo de �ndice
long buscarIndice(const char *id) {
    FILE *arquivoIndice = fopen(NOME_ARQUIVO_INDICE, "rb");
    if (!arquivoIndice) {
        perror("Erro ao abrir o arquivo de �ndice");
        return -1;
    }

    int esquerda = 0;
    fseek(arquivoIndice, 0, SEEK_END);
    int direita = ftell(arquivoIndice) / sizeof(EntradaIndice) - 1;

    EntradaIndice entrada;
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        fseek(arquivoIndice, meio * sizeof(EntradaIndice), SEEK_SET);
        fread(&entrada, sizeof(EntradaIndice), 1, arquivoIndice);

        int cmp = strncmp(id, entrada.id, 8);
        if (cmp == 0) {
            fclose(arquivoIndice);
            return entrada.posicao;
        } else if (cmp < 0) {
            direita = meio - 1;
        } else {
            esquerda = meio + 1;
        }
    }
    fclose(arquivoIndice);
    return -1;  // ID n�o encontrado
}

// Fun��o para consultar um produto usando o �ndice
void buscarProdutoUsandoIndice(const char *id) {
    long posicao = buscarIndice(id);
    if (posicao == -1) {
        printf("Produto com ID %s n�o encontrado.\n", id);
        return;
    }

    FILE *arquivoDados = fopen(NOME_ARQUIVO, "rb");
    if (!arquivoDados) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }

    Produto produto;
    fseek(arquivoDados, posicao * sizeof(Produto), SEEK_SET);
    fread(&produto, sizeof(Produto), 1, arquivoDados);
    fclose(arquivoDados);

    printf("Produto encontrado:\n");
    printf("ID: %.8s\nNome: %.40s\nPre�o: %.10s\n", produto.id, produto.nome, produto.preco);
}

// Fun��o para mostrar produtos abaixo de um valor especificado
void mostrarProdutosAbaixoDe(float valorMaximo) {
    FILE *arquivo = fopen(NOME_ARQUIVO, "rb");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    Produto produto;
    float preco;
    while (fread(&produto, sizeof(Produto), 1, arquivo) == 1) {
        preco = atof(produto.preco);
        if (preco < valorMaximo) {
            printf("ID Produto: %.8s\n", produto.id);
            printf("Nome: %.40s\n", produto.nome);
            printf("Pre�o: %.10s\n", produto.preco);
            printf("-----------------------------------\n");
        }
    }
    fclose(arquivo);
}

int main() {
    //criarIndice();
    //Inserir dados
    //inserirProduto("54863259", "sony", "128.68");

    // Exibir todos os produtos
    //printf("Lista de Produtos:\n");
    //mostrarProdutos();

    // Realizar pesquisa binaria no �ndice
    //printf("\nPesquisa usando o �ndice:\n");
    //buscarProdutoUsandoIndice("54863259");

    // Remover produto
    //removerProduto("54863259");

    //buscarProdutoUsandoIndice("54863259");

    //printf("\nProdutos com pre�o abaixo de 15.00:\n");
    //mostrarProdutosAbaixoDe(5.00);

    return 0;
}

