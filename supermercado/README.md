# Sistema de Supermercado e Filas de Caixa

Projeto de Estruturas de Dados em C++ para controlar produtos, compras, caixas e
histórico de operações. Todo o código está em `main.cpp`, usando apenas os
conceitos vistos em sala: **struct, vetor de tamanho fixo, ponteiro, fila, pilha,
função recursiva e matriz**.

## Conceitos e onde estão no código

| Conceito | Função | O que faz |
|---|---|---|
| Ponteiro | `enfileirar`, `empilhar` | nós (`NoCliente`, `NoOperacao`) ligados por `proximo` |
| Fila | `enfileirar` / `desenfileirar` | clientes de cada caixa, em ordem de chegada |
| Pilha | `empilhar` / `listarHistorico` | histórico de operações, do mais recente para o mais antigo |
| Recursão | `buscarProduto` | procura o produto percorrendo o vetor |
| | `somarCarrinho` | soma o total de uma compra |
| | `totalVendidoProduto` / `totalVendidoCaixa` | somam uma linha/coluna da matriz |
| | `liberarFila` / `liberarPilha` | liberam a memória ao sair |
| Matriz | `vendas[produto][caixa]` | quantidade vendida de cada produto em cada caixa |

## Compilação

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o mercado

./mercado        # 3 caixas
./mercado 5      # define a quantidade de caixas (1 a 5)
```

O que cada flag faz:

| Flag | O que faz |
|---|---|
| `-std=c++17` | escolhe a versão da linguagem; o código usa recursos de C++11, como `to_string` |
| `-Wall` | liga os avisos mais comuns do compilador |
| `-Wextra` | liga avisos adicionais, além dos que o `-Wall` já mostra |
| `-pedantic` | avisa sobre qualquer coisa fora do padrão C++ (extensões do GCC) |
| `-o mercado` | define o nome do arquivo executável gerado |

`-Wall -Wextra -pedantic` não mudam o programa: só fazem o compilador avisar quando
algo está suspeito. O projeto compila com **zero avisos** nessas condições.

## Menu

```
1 Cadastrar produto          5 Listar filas dos caixas
2 Consultar preco            6 Finalizar atendimento
3 Listar produtos            7 Historico de operacoes (pilha)
4 Registrar compra           8 Relatorio de vendas (matriz)
0 Sair
```

## Regras principais

- Código de produto único e positivo, preço maior que zero, estoque não negativo.
- A compra é validada no registro e **revalidada** na finalização.
- O estoque só é debitado ao finalizar o atendimento.
- Se o estoque acabar enquanto o cliente espera, a finalização é bloqueada e ele
  continua na fila.
- Caixa `0` no registro de compra escolhe automaticamente o caixa com a menor fila.

## Limites (vetores de tamanho fixo)

`MAX_PRODUTOS = 50`, `MAX_CAIXAS = 5`, `MAX_ITENS = 20` por compra.

## Apresentação

`apresentacao/slides.html` traz a demonstração do caso de uso com a saída real do
programa; veja `apresentacao/README.md`.
