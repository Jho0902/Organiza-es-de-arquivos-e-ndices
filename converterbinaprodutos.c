#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id_produto[9];  // Tamanho fixo de 7 caracteres + '\0'
    char marca[41];      // Tamanho fixo de 41 caracteres + '\0'
    char preco[11];      // Tamanho fixo de 10 caracteres + '\0'
} Produto;

void escreverRegistroBinario(FILE *arquivoBinario, Produto *produto) {
    fwrite(produto, sizeof(Produto), 1, arquivoBinario);
}

void preencherEspacosBrancos(char *str, size_t tamanho) {
    size_t len = strlen(str);
    if (len < tamanho) {
        memset(str + len, ' ', tamanho - len);
    }
}

void lerDadosBinario(const char *nomeArquivo) {
    FILE *arquivoBinario = fopen(nomeArquivo, "rb");
    if (arquivoBinario == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo binário.\n");
        return;
    }

    Produto produto;
    while (fread(&produto, sizeof(Produto), 1, arquivoBinario) == 1) {
        printf("ID Produto: %.8s\n", produto.id_produto);
        printf("Marca: %.40s\n", produto.marca);
        printf("Preço: %.10s\n", produto.preco);
        printf("-----------------------------------\n");
    }

    fclose(arquivoBinario);
}

int main() {

   /*
    FILE *arquivoTexto = fopen("produtos.txt", "r");
    FILE *arquivoBinario = fopen("produtos.bin", "wb");

    if (arquivoTexto == NULL || arquivoBinario == NULL) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return 1;
    }

    Produto produto;
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivoTexto)) {
        // Remover nova linha se existir
        linha[strcspn(linha, "\n")] = '\0';

        // Separar os campos usando strtok
        char *token = strtok(linha, ";");

        strncpy(produto.id_produto, token, sizeof(produto.id_produto) - 1);
        produto.id_produto[sizeof(produto.id_produto) - 1] = '\0';

        token = strtok(NULL, ";");
        strncpy(produto.marca, token, sizeof(produto.marca) - 1);
        produto.marca[sizeof(produto.marca) - 1] = '\0';

        token = strtok(NULL, ";");
        strncpy(produto.preco, token, sizeof(produto.preco) - 1);
        produto.preco[sizeof(produto.preco) - 1] = '\0';

        // Preencher os campos restantes com espaços em branco
        preencherEspacosBrancos(produto.id_produto, sizeof(produto.id_produto) - 1);
        preencherEspacosBrancos(produto.marca, sizeof(produto.marca) - 1);
        preencherEspacosBrancos(produto.preco, sizeof(produto.preco) - 1);

        escreverRegistroBinario(arquivoBinario, &produto);
    }

    fclose(arquivoTexto);
    fclose(arquivoBinario);

    printf("Dados gravados em formato binário com sucesso.\n");
*/
    // Descomente para ler os dados binários
    lerDadosBinario("produtos.bin");

    return 0;
}

