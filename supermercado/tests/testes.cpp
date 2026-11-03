#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "mercado.h"
#include "pilha_operacoes.h"

namespace {

int falhas = 0;
int total = 0;

void verificar(bool condicao, const std::string& descricao) {
    ++total;
    if (condicao) {
        std::cout << "[OK]     " << descricao << '\n';
    } else {
        ++falhas;
        std::cout << "[FALHOU] " << descricao << '\n';
    }
}

bool quaseIgual(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

ItemCompra item(int codigo, int quantidade) {
    return ItemCompra{codigo, "", quantidade, 0.0};
}

void testarCadastroEConsulta() {
    Mercado mercado(3);
    std::string mensagem;

    verificar(mercado.cadastrarProduto(101, "Arroz", 25.0, 10, mensagem) ==
                  Status::OK,
              "cadastra produto valido");
    verificar(mercado.cadastrarProduto(101, "Outro", 10.0, 1, mensagem) ==
                  Status::CODIGO_DUPLICADO,
              "rejeita codigo duplicado");
    verificar(mercado.cadastrarProduto(0, "Zero", 1.0, 1, mensagem) ==
                  Status::DADOS_INVALIDOS,
              "rejeita codigo nao positivo");
    verificar(mercado.cadastrarProduto(102, "   ", 1.0, 1, mensagem) ==
                  Status::DADOS_INVALIDOS,
              "rejeita nome vazio");
    verificar(mercado.cadastrarProduto(103, "Sem preco", 0.0, 1, mensagem) ==
                  Status::DADOS_INVALIDOS,
              "rejeita preco zero");
    verificar(mercado.cadastrarProduto(104, "Sem estoque", 1.0, -1,
                                       mensagem) == Status::DADOS_INVALIDOS,
              "rejeita estoque negativo");

    const Produto* produto = mercado.consultarProduto(101);
    verificar(produto != nullptr && produto->nome == "Arroz" &&
                  quaseIgual(produto->preco, 25.0),
              "consulta produto por codigo");
    verificar(mercado.consultarProduto(999) == nullptr,
              "consulta codigo inexistente");

    const std::vector<Produto> encontrados = mercado.buscarPorNome("ARR");
    verificar(encontrados.size() == 1 && encontrados[0].codigo == 101,
              "busca por nome ignora maiusculas/minusculas");
}

void testarFilasEAutoAtribuicao() {
    Mercado mercado(3);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Feijao", 8.0, 20, mensagem);

    int caixa = 0;
    verificar(mercado.registrarCompra("Maria", {item(1, 2)}, -1, caixa,
                                      mensagem) == Status::OK &&
                  caixa == 1,
              "primeiro cliente vai para o caixa 1");
    verificar(mercado.registrarCompra("Joao", {item(1, 1)}, -1, caixa,
                                      mensagem) == Status::OK &&
                  caixa == 2,
              "segundo cliente vai para o caixa 2");
    verificar(mercado.registrarCompra("Ana", {item(1, 1)}, -1, caixa,
                                      mensagem) == Status::OK &&
                  caixa == 3,
              "terceiro cliente vai para o caixa 3");
    verificar(mercado.registrarCompra("Bia", {item(1, 1)}, -1, caixa,
                                      mensagem) == Status::OK &&
                  caixa == 1,
              "quarto cliente vai para o caixa com menor fila");

    verificar(mercado.tamanhoFila(1) == 2 && mercado.tamanhoFila(2) == 1 &&
                  mercado.tamanhoFila(3) == 1,
              "tamanhos das filas corretos");
    verificar(mercado.registrarCompra("", {item(1, 1)}, -1, caixa, mensagem) ==
                  Status::DADOS_INVALIDOS,
              "rejeita cliente sem nome");
    verificar(mercado.registrarCompra("X", {}, -1, caixa, mensagem) ==
                  Status::DADOS_INVALIDOS,
              "rejeita compra sem itens");
    verificar(mercado.registrarCompra("X", {item(1, 1)}, 9, caixa, mensagem) ==
                  Status::CAIXA_INVALIDO,
              "rejeita caixa inexistente");
}

void testarFinalizacao() {
    Mercado mercado(2);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Cafe", 20.0, 5, mensagem);

    int caixa = 0;
    mercado.registrarCompra("Maria", {item(1, 2)}, -1, caixa, mensagem);

    Cliente atendido;
    verificar(mercado.finalizarAtendimento(1, atendido, mensagem) ==
                  Status::OK,
              "finaliza atendimento");
    verificar(atendido.nome == "Maria" &&
                  quaseIgual(atendido.total(), 40.0),
              "comprovante contem cliente e total");
    verificar(mercado.tamanhoFila(1) == 0,
              "cliente removido da fila apos atendimento");

    const Produto* produto = mercado.consultarProduto(1);
    verificar(produto != nullptr && produto->estoque == 3 &&
                  produto->vendidos == 2,
              "estoque debitado e vendidos incrementados");
    verificar(quaseIgual(mercado.faturamentoTotal(), 40.0) &&
                  mercado.totalComprasFinalizadas() == 1,
              "faturamento e contador atualizados");
    verificar(mercado.finalizarAtendimento(1, atendido, mensagem) ==
                  Status::FILA_VAZIA,
              "rejeita finalizacao em caixa vazio");
}

void testarEstoqueInsuficiente() {
    Mercado mercado(2);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Leite", 5.0, 5, mensagem);

    int caixa = 0;
    verificar(mercado.registrarCompra("A", {item(1, 6)}, -1, caixa,
                                      mensagem) ==
                  Status::ESTOQUE_INSUFICIENTE,
              "rejeita item acima do estoque");
    verificar(mercado.registrarCompra("Dup", {item(1, 3), item(1, 3)}, -1,
                                      caixa, mensagem) ==
                  Status::ESTOQUE_INSUFICIENTE,
              "soma itens repetidos ao validar estoque");

    verificar(mercado.registrarCompra("A", {item(1, 3)}, -1, caixa,
                                      mensagem) == Status::OK,
              "aceita compra dentro do estoque");
    verificar(mercado.registrarCompra("B", {item(1, 3)}, -1, caixa,
                                      mensagem) == Status::OK,
              "aceita segunda compra dentro do estoque atual");

    Cliente atendido;
    verificar(mercado.finalizarAtendimento(1, atendido, mensagem) ==
                  Status::OK,
              "primeira finalizacao consome estoque");
    verificar(mercado.finalizarAtendimento(2, atendido, mensagem) ==
                  Status::ESTOQUE_INSUFICIENTE,
              "segunda finalizacao bloqueada por falta de estoque");
    verificar(mercado.tamanhoFila(2) == 1,
              "cliente permanece na fila quando bloqueado");
}

void testarDesistencia() {
    Mercado mercado(2);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Pao", 2.0, 10, mensagem);

    int caixa = 0;
    mercado.registrarCompra("A", {item(1, 1)}, -1, caixa, mensagem);
    mercado.registrarCompra("B", {item(1, 1)}, -1, caixa, mensagem);
    mercado.registrarCompra("C", {item(1, 1)}, -1, caixa, mensagem);

    verificar(mercado.tamanhoFila(1) == 2,
              "fila do caixa 1 com dois clientes");

    Cliente desistente;
    verificar(mercado.registrarDesistencia(1, 3, desistente, mensagem) ==
                  Status::OK &&
                  desistente.nome == "C",
              "remove cliente especifico da fila");
    verificar(mercado.tamanhoFila(1) == 1,
              "fila diminui apos desistencia");
    verificar(mercado.registrarDesistencia(1, 99, desistente, mensagem) ==
                  Status::CLIENTE_NAO_ENCONTRADO,
              "rejeita desistencia de id inexistente");

    const Produto* produto = mercado.consultarProduto(1);
    verificar(produto != nullptr && produto->estoque == 10 &&
                  produto->vendidos == 0,
              "desistencia nao altera estoque nem vendas");
}

void testarMaisVendidos() {
    Mercado mercado(1);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Arroz", 10.0, 100, mensagem);
    mercado.cadastrarProduto(2, "Feijao", 5.0, 100, mensagem);

    int caixa = 0;
    mercado.registrarCompra("X", {item(1, 3), item(2, 1)}, -1, caixa,
                            mensagem);

    Cliente atendido;
    mercado.finalizarAtendimento(1, atendido, mensagem);

    const std::vector<Produto> ranking = mercado.produtosMaisVendidos(0);
    verificar(ranking.size() == 2 && ranking[0].codigo == 1 &&
                  ranking[0].vendidos == 3,
              "ranking ordena por quantidade vendida");
    verificar(ranking[1].codigo == 2 && ranking[1].vendidos == 1,
              "ranking inclui segundo produto");
    verificar(mercado.produtosMaisVendidos(1).size() == 1,
              "ranking respeita limite solicitado");
}

void testarHistoricoEPilha() {
    PilhaOperacoes pilha;
    pilha.empilhar(
        Operacao{TipoOperacao::CADASTRO_PRODUTO, "primeira", 0.0, "00:00:00"});
    pilha.empilhar(
        Operacao{TipoOperacao::REGISTRO_COMPRA, "segunda", 10.0, "00:00:01"});

    verificar(pilha.tamanho() == 2, "pilha recebe duas operacoes");
    verificar(pilha.topo() != nullptr && pilha.topo()->descricao == "segunda",
              "topo da pilha e a ultima operacao");

    Operacao desempilhada;
    verificar(pilha.desempilhar(desempilhada) &&
                  desempilhada.descricao == "segunda",
              "desempilha em ordem LIFO");
    verificar(pilha.tamanho() == 1, "pilha diminui apos desempilhar");

    Mercado mercado(1);
    std::string mensagem;
    mercado.cadastrarProduto(1, "Sal", 3.0, 10, mensagem);
    int caixa = 0;
    mercado.registrarCompra("Ana", {item(1, 1)}, -1, caixa, mensagem);
    Cliente atendido;
    mercado.finalizarAtendimento(1, atendido, mensagem);
    verificar(mercado.totalOperacoes() == 3,
              "mercado registra cadastro, compra e finalizacao");
}

void testarConfiguracaoDeCaixas() {
    Mercado mercado(0);
    verificar(mercado.numeroCaixas() == 1,
              "numero de caixas invalido e ajustado para 1");
    verificar(mercado.tamanhoFila(0) == -1 && mercado.tamanhoFila(2) == -1,
              "consulta de fila invalida retorna -1");
}

}  // namespace

int main() {
    std::cout << "Executando testes do sistema de supermercado...\n\n";

    testarCadastroEConsulta();
    testarFilasEAutoAtribuicao();
    testarFinalizacao();
    testarEstoqueInsuficiente();
    testarDesistencia();
    testarMaisVendidos();
    testarHistoricoEPilha();
    testarConfiguracaoDeCaixas();

    std::cout << "\nTotal: " << total << " | Falhas: " << falhas << '\n';
    return falhas == 0 ? 0 : 1;
}
