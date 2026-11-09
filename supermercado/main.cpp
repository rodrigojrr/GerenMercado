// =====================================================================
// Sistema de Supermercado e Filas de Caixa
// Projeto de Estruturas de Dados - C++ basico
//
// Conceitos usados neste arquivo:
//   ponteiro : nos da fila de clientes e da pilha de operacoes
//   fila     : clientes esperando em cada caixa (FIFO)
//   pilha    : historico de operacoes (LIFO)
//   recursao : busca de produto, soma do carrinho, soma da matriz,
//              liberacao da memoria
//   matriz   : vendas[produto][caixa]
// =====================================================================

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// ---------------------------------------------------------------------
// Constantes
// ---------------------------------------------------------------------

const int MAX_PRODUTOS = 50;
const int MAX_CAIXAS = 5;
const int MAX_ITENS = 20;

// ---------------------------------------------------------------------
// Estruturas
// ---------------------------------------------------------------------

struct Produto {
    int codigo;
    string nome;
    double preco;
    int estoque;
};

struct ItemCompra {
    int codigoProduto;
    string nome;
    int quantidade;
    double precoUnitario;
};

struct Cliente {
    int id;
    string nome;
    ItemCompra itens[MAX_ITENS];
    int qtdItens;
};

// ---- fila encadeada (um no para cada cliente) ----
struct NoCliente {
    Cliente dado;
    NoCliente *proximo;
};

struct FilaClientes {
    NoCliente *inicio;
    NoCliente *fim;
    int tamanho;
};

// ---- pilha encadeada (um no para cada operacao) ----
struct NoOperacao {
    string descricao;
    double valor;
    string horario;
    NoOperacao *proximo;
};

struct PilhaOperacoes {
    NoOperacao *topo;
    int tamanho;
};

// ---- o sistema inteiro ----
struct Mercado {
    Produto produtos[MAX_PRODUTOS];
    int totalProdutos;

    FilaClientes caixas[MAX_CAIXAS];
    int numCaixas;

    PilhaOperacoes historico;

    int vendas[MAX_PRODUTOS][MAX_CAIXAS];  // matriz: produto x caixa

    int proximoId;
    double faturamento;
    int atendimentos;
};

// ---------------------------------------------------------------------
// Funcoes auxiliares (texto, dinheiro e horario)
// ---------------------------------------------------------------------

string formatarMoeda(double valor) {
    ostringstream texto;
    texto << "R$ " << fixed << setprecision(2) << valor;
    return texto.str();
}

string horarioAtual() {
    time_t agora = time(NULL);
    struct tm *quebrado = localtime(&agora);

    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", quebrado);
    return string(buffer);
}

string trim(const string &texto) {
    unsigned int inicio = 0;
    unsigned int fim = texto.size();

    while (inicio < fim && (texto[inicio] == ' ' || texto[inicio] == '\t' ||
                            texto[inicio] == '\r' || texto[inicio] == '\n')) {
        inicio++;
    }
    while (fim > inicio && (texto[fim - 1] == ' ' || texto[fim - 1] == '\t' ||
                            texto[fim - 1] == '\r' || texto[fim - 1] == '\n')) {
        fim--;
    }
    return texto.substr(inicio, fim - inicio);
}

// ---------------------------------------------------------------------
// Leitura de dados pelo teclado
// ---------------------------------------------------------------------

string lerLinha(const string &prompt) {
    cout << prompt;
    string linha;
    if (!getline(cin, linha)) {
        cout << "\nEntrada encerrada. Saindo.\n";
        exit(EXIT_SUCCESS);
    }
    return linha;
}

int lerInteiro(const string &prompt) {
    while (true) {
        string linha = lerLinha(prompt);
        istringstream entrada(linha);
        int valor = 0;
        char extra = 0;

        if (entrada >> valor && !(entrada >> extra)) {
            return valor;
        }
        cout << "Entrada invalida. Digite um numero inteiro.\n";
    }
}

double lerDouble(const string &prompt) {
    while (true) {
        string linha = lerLinha(prompt);
        istringstream entrada(linha);
        double valor = 0.0;
        char extra = 0;

        if (entrada >> valor && !(entrada >> extra)) {
            return valor;
        }
        cout << "Entrada invalida. Digite um numero.\n";
    }
}

string lerTexto(const string &prompt) {
    while (true) {
        string texto = trim(lerLinha(prompt));
        if (!texto.empty()) {
            return texto;
        }
        cout << "O texto nao pode ser vazio.\n";
    }
}

// ---------------------------------------------------------------------
// FILA - clientes do caixa (FIFO)
// ---------------------------------------------------------------------

void inicializarFila(FilaClientes &fila) {
    fila.inicio = NULL;
    fila.fim = NULL;
    fila.tamanho = 0;
}

bool filaVazia(const FilaClientes &fila) {
    return fila.inicio == NULL;
}

void enfileirar(FilaClientes &fila, const Cliente &cliente) {
    NoCliente *novo = new NoCliente;
    novo->dado = cliente;
    novo->proximo = NULL;

    if (fila.fim == NULL) {  // fila vazia
        fila.inicio = novo;
        fila.fim = novo;
    } else {
        fila.fim->proximo = novo;
        fila.fim = novo;
    }
    fila.tamanho++;
}

bool desenfileirar(FilaClientes &fila, Cliente &saida) {
    if (fila.inicio == NULL) {
        return false;
    }

    NoCliente *removido = fila.inicio;
    saida = removido->dado;
    fila.inicio = removido->proximo;

    if (fila.inicio == NULL) {
        fila.fim = NULL;
    }

    delete removido;
    fila.tamanho--;
    return true;
}

// Recursao: libera a fila do fim para o inicio.
void liberarFila(NoCliente *no) {
    if (no == NULL) {
        return;
    }
    liberarFila(no->proximo);
    delete no;
}

// ---------------------------------------------------------------------
// PILHA - historico de operacoes (LIFO)
// ---------------------------------------------------------------------

void inicializarPilha(PilhaOperacoes &pilha) {
    pilha.topo = NULL;
    pilha.tamanho = 0;
}

void empilhar(PilhaOperacoes &pilha, const string &descricao, double valor) {
    NoOperacao *novo = new NoOperacao;
    novo->descricao = descricao;
    novo->valor = valor;
    novo->horario = horarioAtual();
    novo->proximo = pilha.topo;

    pilha.topo = novo;
    pilha.tamanho++;
}

// Recursao: libera a pilha do fim para o topo.
void liberarPilha(NoOperacao *no) {
    if (no == NULL) {
        return;
    }
    liberarPilha(no->proximo);
    delete no;
}

// ---------------------------------------------------------------------
// Inicializacao e finalizacao do sistema
// ---------------------------------------------------------------------

void inicializarMercado(Mercado &m, int caixas) {
    if (caixas < 1) {
        caixas = 1;
    }
    if (caixas > MAX_CAIXAS) {
        caixas = MAX_CAIXAS;
    }

    m.totalProdutos = 0;
    m.numCaixas = caixas;
    m.proximoId = 1;
    m.faturamento = 0.0;
    m.atendimentos = 0;

    for (int i = 0; i < MAX_CAIXAS; i++) {
        inicializarFila(m.caixas[i]);
    }
    inicializarPilha(m.historico);

    for (int i = 0; i < MAX_PRODUTOS; i++) {
        for (int j = 0; j < MAX_CAIXAS; j++) {
            m.vendas[i][j] = 0;
        }
    }
}

void liberarMercado(Mercado &m) {
    for (int i = 0; i < m.numCaixas; i++) {
        liberarFila(m.caixas[i].inicio);
        inicializarFila(m.caixas[i]);
    }
    liberarPilha(m.historico.topo);
    inicializarPilha(m.historico);
}

// ---------------------------------------------------------------------
// Produtos
// ---------------------------------------------------------------------

// Recursao: procura o produto do indice "i" ate o fim do vetor.
// Devolve a posicao encontrada ou -1.
int buscarProduto(const Produto produtos[], int i, int total, int codigo) {
    if (i >= total) {
        return -1;
    }
    if (produtos[i].codigo == codigo) {
        return i;
    }
    return buscarProduto(produtos, i + 1, total, codigo);
}

// Recursao: soma o total de uma compra.
double somarCarrinho(const ItemCompra itens[], int i, int total) {
    if (i >= total) {
        return 0.0;
    }
    return itens[i].quantidade * itens[i].precoUnitario +
           somarCarrinho(itens, i + 1, total);
}

// Soma quantas unidades de um produto existem no carrinho.
int quantidadeNoCarrinho(const ItemCompra itens[], int total, int codigo) {
    int soma = 0;
    for (int i = 0; i < total; i++) {
        if (itens[i].codigoProduto == codigo) {
            soma += itens[i].quantidade;
        }
    }
    return soma;
}

bool cadastrarProduto(Mercado &m, int codigo, const string &nome, double preco,
                      int estoque, string &mensagem) {
    if (codigo <= 0) {
        mensagem = "O codigo do produto deve ser positivo.";
        return false;
    }
    if (nome.empty()) {
        mensagem = "O nome do produto nao pode ser vazio.";
        return false;
    }
    if (preco <= 0.0) {
        mensagem = "O preco deve ser maior que zero.";
        return false;
    }
    if (estoque < 0) {
        mensagem = "O estoque nao pode ser negativo.";
        return false;
    }
    if (m.totalProdutos >= MAX_PRODUTOS) {
        mensagem = "Limite de produtos cadastrados atingido.";
        return false;
    }
    if (buscarProduto(m.produtos, 0, m.totalProdutos, codigo) >= 0) {
        mensagem = "Ja existe um produto com o codigo " + to_string(codigo) + ".";
        return false;
    }

    Produto novo;
    novo.codigo = codigo;
    novo.nome = nome;
    novo.preco = preco;
    novo.estoque = estoque;
    m.produtos[m.totalProdutos] = novo;
    m.totalProdutos++;

    empilhar(m.historico,
             "Produto " + to_string(codigo) + " - " + nome +
                 " cadastrado com estoque " + to_string(estoque),
             0.0);

    mensagem = "Produto " + nome + " cadastrado com sucesso.";
    return true;
}

void listarProdutos(const Mercado &m) {
    if (m.totalProdutos == 0) {
        cout << "Nenhum produto cadastrado.\n";
        return;
    }

    cout << left << setw(8) << "Codigo" << setw(26) << "Nome" << right
         << setw(12) << "Preco" << setw(10) << "Estoque" << '\n';

    for (int i = 0; i < m.totalProdutos; i++) {
        string nome = m.produtos[i].nome;
        if (nome.size() > 24) {
            nome = nome.substr(0, 23) + ".";
        }

        cout << left << setw(8) << m.produtos[i].codigo << setw(26) << nome
             << right << setw(12) << formatarMoeda(m.produtos[i].preco)
             << setw(10) << m.produtos[i].estoque << '\n';
    }
}

// ---------------------------------------------------------------------
// Compras e atendimento
// ---------------------------------------------------------------------

int caixaMenorFila(const Mercado &m) {
    int escolhido = 1;
    for (int i = 2; i <= m.numCaixas; i++) {
        if (m.caixas[i - 1].tamanho < m.caixas[escolhido - 1].tamanho) {
            escolhido = i;
        }
    }
    return escolhido;
}

bool estoqueSuficiente(const Mercado &m, const Cliente &cliente,
                       string &mensagem) {
    for (int i = 0; i < cliente.qtdItens; i++) {
        int codigo = cliente.itens[i].codigoProduto;

        // verifica cada codigo uma unica vez
        bool repetido = false;
        for (int j = 0; j < i; j++) {
            if (cliente.itens[j].codigoProduto == codigo) {
                repetido = true;
                break;
            }
        }
        if (repetido) {
            continue;
        }

        int posicao = buscarProduto(m.produtos, 0, m.totalProdutos, codigo);
        if (posicao < 0) {
            mensagem = "Produto de codigo " + to_string(codigo) +
                       " nao encontrado.";
            return false;
        }

        int necessario = quantidadeNoCarrinho(cliente.itens, cliente.qtdItens,
                                              codigo);
        if (m.produtos[posicao].estoque < necessario) {
            mensagem = "Estoque insuficiente para '" + m.produtos[posicao].nome +
                       "': disponivel " +
                       to_string(m.produtos[posicao].estoque) +
                       ", solicitado " + to_string(necessario) + ".";
            return false;
        }
    }
    return true;
}

bool registrarCompra(Mercado &m, const string &nomeCliente,
                     const ItemCompra itens[], int qtdItens, int caixaPedido,
                     int &caixaUsado, string &mensagem) {
    if (qtdItens <= 0) {
        mensagem = "A compra precisa ter pelo menos um item.";
        return false;
    }
    if (qtdItens > MAX_ITENS) {
        mensagem = "Limite de itens por compra atingido.";
        return false;
    }

    Cliente cliente;
    cliente.nome = nomeCliente;
    cliente.qtdItens = 0;

    for (int i = 0; i < qtdItens; i++) {
        if (itens[i].quantidade <= 0) {
            mensagem = "Quantidade invalida para o produto " +
                       to_string(itens[i].codigoProduto) + ".";
            return false;
        }

        int posicao = buscarProduto(m.produtos, 0, m.totalProdutos,
                                    itens[i].codigoProduto);
        if (posicao < 0) {
            mensagem = "Produto de codigo " +
                       to_string(itens[i].codigoProduto) + " nao encontrado.";
            return false;
        }

        // o preco vem sempre do cadastro, nunca do que o usuario digitou
        cliente.itens[cliente.qtdItens].codigoProduto = m.produtos[posicao].codigo;
        cliente.itens[cliente.qtdItens].nome = m.produtos[posicao].nome;
        cliente.itens[cliente.qtdItens].precoUnitario = m.produtos[posicao].preco;
        cliente.itens[cliente.qtdItens].quantidade = itens[i].quantidade;
        cliente.qtdItens++;
    }

    if (!estoqueSuficiente(m, cliente, mensagem)) {
        return false;
    }

    int caixa;
    if (caixaPedido == 0) {
        caixa = caixaMenorFila(m);
    } else if (caixaPedido < 1 || caixaPedido > m.numCaixas) {
        mensagem = "Caixa invalido. Informe um valor entre 1 e " +
                   to_string(m.numCaixas) + ".";
        return false;
    } else {
        caixa = caixaPedido;
    }

    cliente.id = m.proximoId;
    m.proximoId++;

    enfileirar(m.caixas[caixa - 1], cliente);
    caixaUsado = caixa;

    double total = somarCarrinho(cliente.itens, 0, cliente.qtdItens);
    empilhar(m.historico,
             "Cliente " + cliente.nome + " (id " + to_string(cliente.id) +
                 ") entrou na fila do caixa " + to_string(caixa),
             total);

    mensagem = "Cliente " + cliente.nome + " (id " + to_string(cliente.id) +
               ") entrou na fila do caixa " + to_string(caixa) + ".";
    return true;
}

bool finalizarAtendimento(Mercado &m, int caixa, Cliente &atendido,
                          string &mensagem) {
    if (caixa < 1 || caixa > m.numCaixas) {
        mensagem = "Caixa invalido. Informe um valor entre 1 e " +
                   to_string(m.numCaixas) + ".";
        return false;
    }

    FilaClientes &fila = m.caixas[caixa - 1];
    if (filaVazia(fila)) {
        mensagem = "O caixa " + to_string(caixa) + " nao tem clientes.";
        return false;
    }

    // revalida o estoque antes de chamar o cliente
    Cliente frente = fila.inicio->dado;
    if (!estoqueSuficiente(m, frente, mensagem)) {
        mensagem = "Atendimento bloqueado: " + mensagem;
        return false;
    }

    desenfileirar(fila, atendido);

    for (int i = 0; i < atendido.qtdItens; i++) {
        int posicao = buscarProduto(m.produtos, 0, m.totalProdutos,
                                    atendido.itens[i].codigoProduto);
        if (posicao >= 0) {
            m.produtos[posicao].estoque -= atendido.itens[i].quantidade;
            m.vendas[posicao][caixa - 1] += atendido.itens[i].quantidade;
        }
    }

    double total = somarCarrinho(atendido.itens, 0, atendido.qtdItens);
    m.faturamento += total;
    m.atendimentos++;

    empilhar(m.historico,
             "Cliente " + atendido.nome + " (id " + to_string(atendido.id) +
                 ") atendido no caixa " + to_string(caixa),
             total);

    mensagem = "Atendimento do cliente " + atendido.nome +
               " finalizado no caixa " + to_string(caixa) + ".";
    return true;
}

// ---------------------------------------------------------------------
// Listagens e relatorios
// ---------------------------------------------------------------------

void listarFilas(const Mercado &m) {
    for (int i = 0; i < m.numCaixas; i++) {
        cout << "Caixa " << (i + 1) << " (" << m.caixas[i].tamanho
             << " cliente(s)):\n";

        if (filaVazia(m.caixas[i])) {
            cout << "  (fila vazia)\n";
            continue;
        }

        NoCliente *atual = m.caixas[i].inicio;
        while (atual != NULL) {
            double total = somarCarrinho(atual->dado.itens, 0,
                                         atual->dado.qtdItens);
            cout << "  #" << atual->dado.id << " " << atual->dado.nome << " - "
                 << atual->dado.qtdItens << " item(ns) - total "
                 << formatarMoeda(total) << '\n';
            atual = atual->proximo;
        }
    }
}

void listarHistorico(const Mercado &m) {
    if (m.historico.topo == NULL) {
        cout << "Nenhuma operacao registrada.\n";
        return;
    }

    cout << "Historico (mais recente primeiro):\n";

    NoOperacao *atual = m.historico.topo;
    int posicao = 0;
    while (atual != NULL) {
        posicao++;
        cout << right << setw(3) << posicao << ". [" << atual->horario << "] "
             << atual->descricao;

        if (atual->valor > 0.0) {
            cout << " (" << formatarMoeda(atual->valor) << ")";
        }
        cout << '\n';
        atual = atual->proximo;
    }
}

// Recursao: soma uma linha da matriz de vendas (um produto em todos os caixas).
int totalVendidoProduto(const int vendas[][MAX_CAIXAS], int linha, int coluna,
                        int numCaixas) {
    if (coluna >= numCaixas) {
        return 0;
    }
    return vendas[linha][coluna] +
           totalVendidoProduto(vendas, linha, coluna + 1, numCaixas);
}

// Recursao: soma uma coluna da matriz (um caixa em todos os produtos).
int totalVendidoCaixa(const int vendas[][MAX_CAIXAS], int linha, int totalLinhas,
                      int coluna) {
    if (linha >= totalLinhas) {
        return 0;
    }
    return vendas[linha][coluna] +
           totalVendidoCaixa(vendas, linha + 1, totalLinhas, coluna);
}

void relatorioVendas(const Mercado &m) {
    if (m.totalProdutos == 0) {
        cout << "Nenhum produto cadastrado.\n";
        return;
    }

    cout << left << setw(8) << "Codigo" << setw(24) << "Produto";
    for (int c = 1; c <= m.numCaixas; c++) {
        cout << right << setw(8) << ("Cx " + to_string(c));
    }
    cout << setw(10) << "Total" << '\n';

    int totalGeral = 0;
    for (int i = 0; i < m.totalProdutos; i++) {
        string nome = m.produtos[i].nome;
        if (nome.size() > 22) {
            nome = nome.substr(0, 21) + ".";
        }

        cout << left << setw(8) << m.produtos[i].codigo << setw(24) << nome;
        for (int c = 0; c < m.numCaixas; c++) {
            cout << right << setw(8) << m.vendas[i][c];
        }

        int totalProduto = totalVendidoProduto(m.vendas, i, 0, m.numCaixas);
        totalGeral += totalProduto;
        cout << setw(10) << totalProduto << '\n';
    }

    cout << left << setw(32) << "Total por caixa";
    for (int c = 0; c < m.numCaixas; c++) {
        cout << right << setw(8)
             << totalVendidoCaixa(m.vendas, 0, m.totalProdutos, c);
    }
    cout << setw(10) << totalGeral << '\n';

    cout << "\nItens vendidos.....: " << totalGeral << '\n'
         << "Atendimentos.......: " << m.atendimentos << '\n'
         << "Faturamento total..: " << formatarMoeda(m.faturamento) << '\n';
}

// ---------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------

void exibirMenu() {
    cout << "\n===== MENU =====\n"
         << "1  Cadastrar produto\n"
         << "2  Consultar preco por codigo\n"
         << "3  Listar produtos\n"
         << "4  Registrar compra\n"
         << "5  Listar filas dos caixas\n"
         << "6  Finalizar atendimento\n"
         << "7  Historico de operacoes\n"
         << "8  Relatorio de vendas (matriz)\n"
         << "0  Sair\n";
}

void menuCadastrarProduto(Mercado &m) {
    int codigo = lerInteiro("Codigo: ");
    string nome = lerTexto("Nome: ");
    double preco = lerDouble("Preco: ");
    int estoque = lerInteiro("Estoque inicial: ");

    string mensagem;
    if (cadastrarProduto(m, codigo, nome, preco, estoque, mensagem)) {
        cout << mensagem << '\n';
    } else {
        cout << "Erro: " << mensagem << '\n';
    }
}

void menuConsultarPreco(const Mercado &m) {
    int codigo = lerInteiro("Codigo do produto: ");
    int posicao = buscarProduto(m.produtos, 0, m.totalProdutos, codigo);

    if (posicao < 0) {
        cout << "Produto nao encontrado.\n";
        return;
    }

    cout << "Codigo..: " << m.produtos[posicao].codigo << '\n'
         << "Nome....: " << m.produtos[posicao].nome << '\n'
         << "Preco...: " << formatarMoeda(m.produtos[posicao].preco) << '\n'
         << "Estoque.: " << m.produtos[posicao].estoque << '\n';
}

void menuRegistrarCompra(Mercado &m) {
    string nomeCliente = lerTexto("Nome do cliente: ");

    ItemCompra itens[MAX_ITENS];
    int qtdItens = 0;

    while (qtdItens < MAX_ITENS) {
        int codigo = lerInteiro("Codigo do produto (0 para concluir): ");
        if (codigo == 0) {
            break;
        }

        int posicao = buscarProduto(m.produtos, 0, m.totalProdutos, codigo);
        if (posicao < 0) {
            cout << "Produto nao encontrado.\n";
            continue;
        }

        int quantidade = lerInteiro("Quantidade: ");
        if (quantidade <= 0) {
            cout << "A quantidade deve ser positiva.\n";
            continue;
        }

        itens[qtdItens].codigoProduto = m.produtos[posicao].codigo;
        itens[qtdItens].nome = m.produtos[posicao].nome;
        itens[qtdItens].precoUnitario = m.produtos[posicao].preco;
        itens[qtdItens].quantidade = quantidade;
        qtdItens++;

        cout << "Adicionado: " << m.produtos[posicao].nome << " x" << quantidade
             << " = "
             << formatarMoeda(m.produtos[posicao].preco * quantidade) << '\n';
    }

    if (qtdItens == 0) {
        cout << "Compra cancelada: nenhum item informado.\n";
        return;
    }

    int caixa = lerInteiro("Caixa (1.." + to_string(m.numCaixas) +
                           ", 0 para automatico): ");

    int caixaUsado = 0;
    string mensagem;
    if (registrarCompra(m, nomeCliente, itens, qtdItens, caixa, caixaUsado,
                        mensagem)) {
        cout << mensagem << '\n';
    } else {
        cout << "Erro: " << mensagem << '\n';
    }
}

void menuFinalizarAtendimento(Mercado &m) {
    int caixa = lerInteiro("Numero do caixa: ");

    Cliente atendido;
    string mensagem;
    if (!finalizarAtendimento(m, caixa, atendido, mensagem)) {
        cout << "Erro: " << mensagem << '\n';
        return;
    }

    cout << "=== Comprovante ===\n"
         << "Cliente: " << atendido.nome << " (id " << atendido.id << ")\n";

    for (int i = 0; i < atendido.qtdItens; i++) {
        cout << "  - " << atendido.itens[i].nome << " x"
             << atendido.itens[i].quantidade << " x "
             << formatarMoeda(atendido.itens[i].precoUnitario) << " = "
             << formatarMoeda(atendido.itens[i].quantidade *
                              atendido.itens[i].precoUnitario)
             << '\n';
    }

    cout << "Total..: " << formatarMoeda(somarCarrinho(atendido.itens, 0,
                                                      atendido.qtdItens))
         << '\n';
    cout << mensagem << '\n';
}

// ---------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int numCaixas = 3;

    if (argc > 1) {
        numCaixas = atoi(argv[1]);
        if (numCaixas < 1 || numCaixas > MAX_CAIXAS) {
            cout << "Numero de caixas invalido. Usando 3.\n";
            numCaixas = 3;
        }
    }

    Mercado mercado;
    inicializarMercado(mercado, numCaixas);

    cout << "Sistema de Supermercado e Filas de Caixa\n";
    cout << "Caixas ativos: " << mercado.numCaixas << '\n';

    bool executando = true;
    while (executando) {
        exibirMenu();
        int opcao = lerInteiro("Opcao: ");
        cout << '\n';

        switch (opcao) {
            case 1:
                menuCadastrarProduto(mercado);
                break;
            case 2:
                menuConsultarPreco(mercado);
                break;
            case 3:
                listarProdutos(mercado);
                break;
            case 4:
                menuRegistrarCompra(mercado);
                break;
            case 5:
                listarFilas(mercado);
                break;
            case 6:
                menuFinalizarAtendimento(mercado);
                break;
            case 7:
                listarHistorico(mercado);
                break;
            case 8:
                relatorioVendas(mercado);
                break;
            case 0:
                cout << "Encerrando o sistema. Ate logo!\n";
                executando = false;
                break;
            default:
                cout << "Opcao invalida.\n";
                break;
        }
    }

    liberarMercado(mercado);
    return 0;
}
