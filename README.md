# Projeto de Manipulação de Arquivos de Acessos e Produtos

## Arquivo de Dados

Os arquivos de dados foram criados a partir do arquivo "2019-Oct.csv". Utilizamos o PostgreSQL para manipular esse arquivo e dividi-lo em dois arquivos menores.

## Códigos

O trabalho foi separado em dois arquivos:
- **`acessos.c`**: Implementa as operações para o arquivo de dados de acessos.
- **`produtos.c`**: Implementa as operações para o arquivo de produtos.

## Índices

Os índices foram implementados de forma exaustiva, pois a inserção dos dados é feita no final do arquivo. Após cada inserção, o arquivo de índices é atualizado.

## Consultas

As consultas são realizadas com base no índice, utilizando pesquisa binária para encontrar os registros de forma eficiente.

## Inserção

A inserção dos dados é sempre feita no final do arquivo de dados. Após cada inserção, o arquivo de índices é reconstruído para manter a consistência dos dados.

## Exclusão

A exclusão dos registros é feita de forma física. Um arquivo temporário é criado, e os dados são copiados para ele, excluindo o registro desejado. Em seguida, o arquivo original é substituído pelo arquivo temporário atualizado.
