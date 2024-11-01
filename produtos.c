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

// Função de comparação para ordenar os índices
int compararEntradas(const void *a, const void *b) {
    return strncmp(((EntradaIndice *)a)->id, ((EntradaIndice *)b)->id, 8);
}

// Função para criar o arquivo de índice
void criarIndice() {
    FILE *arquivoDados = fopen(NOME_ARQUIVO, "rb");
    if (!arquivoDados) {
        perror("Erro ao abrir o arquivo de dados");
        return;
    }

    // Determine o número de produtos
    fseek(arquivoDados, 0, SEEK_END);
    long numProdutos = ftell(arquivoDados) / sizeof(Produto);
    fseek(arquivoDados, 0, SEEK_SET);

    // Alocar memória para todas as entradas de índice
    EntradaIndice *entradas = malloc(numProdutos * sizeof(EntradaIndice));
    if (!entradas) {
        perror("Erro ao alocar memória");
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

    // Escrever as entradas ordenadas no arquivo de índice
    FILE *arquivoIndice = fopen(NOME_ARQUIVO_INDICE, "wb");
    if (!arquivoIndice) {
        perror("Erro ao abrir o arquivo de índice");
        free(entradas);
        return;
    }
    fwrite(entradas, sizeof(EntradaIndice), numProdutos, arquivoIndice);
    fclose(arquivoIndice);

    // Liberar memória alocada
    free(entradas);
}

// Função para inserir um novo produto
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

    // Reconstruir o índice após inserção
    criarIndice();
}

// Função para remover um produto por ID
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
        // Reconstruir o índice após remoção
        criarIndice();
    } else {
        // Se não encontrou, remova o arquivo temporário
        remove(NOME_ARQUIVO_TEMP);
        printf("Produto com ID %s não encontrado para remoção.\n", id);
    }
}

// Função para mostrar todos os registros do arquivo
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
        printf("Preço: %.10s\n", produto.preco);
        printf("-----------------------------------\n");
    }
    fclose(arquivo);
}

// Função para realizar uma busca binária no arquivo de índice
long buscarIndice(const char *id) {
    FILE *arquivoIndice = fopen(NOME_ARQUIVO_INDICE, "rb");
    if (!arquivoIndice) {
        perror("Erro ao abrir o arquivo de índice");
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
    return -1;  // ID não encontrado
}

// Função para consultar um produto usando o índice
void buscarProdutoUsandoIndice(const char *id) {
    long posicao = buscarIndice(id);
    if (posicao == -1) {
        printf("Produto com ID %s não encontrado.\n", id);
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
    printf("ID: %.8s\nNome: %.40s\nPreço: %.10s\n", produto.id, produto.nome, produto.preco);
}

// Função para mostrar produtos abaixo de um valor especificado
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
            printf("Preço: %.10s\n", produto.preco);
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

    // Realizar pesquisa binaria no índice
    //printf("\nPesquisa usando o índice:\n");
    //buscarProdutoUsandoIndice("54863259");

    // Remover produto
    //removerProduto("54863259");

    //buscarProdutoUsandoIndice("54863259");

    //printf("\nProdutos com preço abaixo de 15.00:\n");
    //mostrarProdutosAbaixoDe(5.00);

    return 0;
}

