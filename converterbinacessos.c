#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id_usuario[10];
    char tipo_evento[9];
    char tempo_evento[25];
} Acesso;

void escreverRegistroBinario(FILE *arquivoBinario, Acesso *acesso) {
    fwrite(acesso, sizeof(Acesso), 1, arquivoBinario);
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

    Acesso acesso;
    while (fread(&acesso, sizeof(Acesso), 1, arquivoBinario) == 1) {
        printf("ID Usuário: %.9s\n", acesso.id_usuario);
        printf("Tipo Evento: %.8s\n", acesso.tipo_evento);
        printf("Tempo Evento: %.24s\n", acesso.tempo_evento);
        printf("-----------------------------------\n");
    }

    fclose(arquivoBinario);
}

int main() {
	
	/*
    FILE *arquivoTexto = fopen("acessos.txt", "r");
    FILE *arquivoBinario = fopen("acessos.bin", "wb");

    if (arquivoTexto == NULL || arquivoBinario == NULL) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        return 1;
    }

    Acesso acesso;
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivoTexto)) {
        // Remover nova linha, se existir
        linha[strcspn(linha, "\n")] = '\0';

        // Separar os campos usando strtok
        char *token = strtok(linha, ",");
        if (token != NULL) {
            strncpy(acesso.id_usuario, token, sizeof(acesso.id_usuario) - 1);
            acesso.id_usuario[sizeof(acesso.id_usuario) - 1] = '\0';
        }

        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(acesso.tipo_evento, token, sizeof(acesso.tipo_evento) - 1);
            acesso.tipo_evento[sizeof(acesso.tipo_evento) - 1] = '\0';
        }

        token = strtok(NULL, ",");
        if (token != NULL) {
            strncpy(acesso.tempo_evento, token, sizeof(acesso.tempo_evento) - 1);
            acesso.tempo_evento[sizeof(acesso.tempo_evento) - 1] = '\0';
        }

        // Preencher os campos restantes com espaços em branco
        preencherEspacosBrancos(acesso.id_usuario, sizeof(acesso.id_usuario));
        preencherEspacosBrancos(acesso.tipo_evento, sizeof(acesso.tipo_evento));
        preencherEspacosBrancos(acesso.tempo_evento, sizeof(acesso.tempo_evento));

        escreverRegistroBinario(arquivoBinario, &acesso);
    }

    fclose(arquivoTexto);
    fclose(arquivoBinario);

    printf("Dados gravados em formato binário com sucesso.\n");
	*/
	
    lerDadosBinario("acessos.bin"); // Descomente esta linha para ler e exibir os dados

    return 0;
}

