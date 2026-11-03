#pragma once

#include <string>
#include <vector>

#include "cliente.h"
#include "fila_clientes.h"
#include "lista_produtos.h"
#include "pilha_operacoes.h"
#include "produto.h"

enum class Status {
    OK,
    DADOS_INVALIDOS,
    CODIGO_DUPLICADO,
    PRODUTO_NAO_ENCONTRADO,
    ESTOQUE_INSUFICIENTE,
    CAIXA_INVALIDO,
    FILA_VAZIA,
    CLIENTE_NAO_ENCONTRADO
};

class Mercado {
public:
    explicit Mercado(int numCaixas = 3);

    Status cadastrarProduto(int codigo, const std::string& nome, double preco,
                            int estoque, std::string& mensagem);
    const Produto* consultarProduto(int codigo) const;
    std::vector<Produto> buscarPorNome(const std::string& trecho) const;
    std::vector<Produto> produtosMaisVendidos(int quantidade) const;
    void listarProdutos() const;

    Status registrarCompra(const std::string& nomeCliente,
                           const std::vector<ItemCompra>& itens, int caixa,
                           int& caixaAtribuido, std::string& mensagem);
    Status finalizarAtendimento(int caixa, Cliente& atendido,
                                std::string& mensagem);
    Status registrarDesistencia(int caixa, int idCliente, Cliente& desistente,
                                std::string& mensagem);

    void listarFilas() const;
    void listarMaisVendidos(int quantidade) const;
    void mostrarHistorico() const;
    void exibirResumo() const;

    int numeroCaixas() const;
    int tamanhoFila(int caixa) const;
    int totalOperacoes() const;
    int totalComprasFinalizadas() const;
    double faturamentoTotal() const;

private:
    bool caixaValido(int caixa) const;
    bool estoqueSuficiente(const std::vector<ItemCompra>& itens,
                           std::string& mensagem) const;
    int escolherCaixaMenorFila() const;
    void registrarHistorico(TipoOperacao tipo, const std::string& descricao,
                            double valor);

    ListaProdutos produtos_;
    std::vector<FilaClientes> caixas_;
    PilhaOperacoes historico_;
    int proximoIdCliente_ = 1;
    double faturamentoTotal_ = 0.0;
    int comprasFinalizadas_ = 0;
    int comprasRegistradas_ = 0;
    int desistencias_ = 0;
};
