# Sistema de Supermercado e Filas de Caixa

Projeto em C++17 que controla produtos, compras, caixas e historico de operacoes.
A lista de produtos, a fila de clientes e a pilha de operacoes foram implementadas
manualmente, sem `std::list`, `std::queue` ou `std::stack`.

## Compilacao

```bash
make
./mercado        # 3 caixas por padrao
./mercado 5      # define a quantidade de caixas
```

## Testes

```bash
make test
```

## Estruturas

- `ListaProdutos`: lista simplesmente encadeada, ordenada por codigo.
- `FilaClientes`: fila encadeada com ponteiros de inicio e fim, uma por caixa.
- `PilhaOperacoes`: pilha encadeada com o historico (LIFO) das operacoes.

## Regras principais

- Codigo de produto unico e positivo, preco maior que zero e estoque nao negativo.
- A compra e validada no registro e revalidada na finalizacao.
- O estoque e debitado somente ao finalizar o atendimento.
- Se o estoque acabar enquanto o cliente espera, a finalizacao e bloqueada e ele
  permanece na fila.
- A atribuicao automatica escolhe o caixa com a menor fila.
