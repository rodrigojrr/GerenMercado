#include "mercado.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>

#include "util.h"

namespace {

bool textoVazio(const std::string& texto) {
    for (const char caractere : texto) {
        if (!std::isspace(static_cast<unsigned char>(caractere))) {
            return false;
        }
    }
    return true;
}

std::string normalizar(const std::string& texto) {
    std::string resultado = texto;
    std::transform(resultado.begin(), resultado.end(), resultado.begin(),
                   [](unsigned char caractere) {
                       return static_cast<char>(std::tolower(caractere));
                   });
    return resultado;
}

}  // namespace

Mercado::Mercado(int numCaixas) {
    if (numCaixas < 1) {
        numCaixas = 1;
    }

    caixas_.reserve(static_cast<std::size_t>(numCaixas));
    for (int i = 0; i < numCaixas; ++i) {
        caixas_.emplace_back();
    }
}

bool Mercado::caixaValido(int caixa) const {
    return caixa >= 1 && caixa <= numeroCaixas();
}

int Mercado::escolherCaixaMenorFila() const {
    int escolhido = 1;
    for (int i = 1; i < numeroCaixas(); ++i) {
        if (caixas_[static_cast<std::size_t>(i)].tamanho() <
            caixas_[static_cast<std::size_t>(escolhido - 1)].tamanho()) {
            escolhido = i + 1;
        }
    }
    return escolhido;
}

void Mercado::registrarHistorico(TipoOperacao tipo,
                                 const std::string& descricao, double valor) {
    historico_.empilhar(Operacao{tipo, descricao, valor, horarioAtual()});
}

Status Mercado::cadastrarProduto(int codigo, const std::string& nome,
                                 double preco, int estoque,
                                 std::string& mensagem) {
    if (codigo <= 0) {
        mensagem = "O codigo do produto deve ser positivo.";
        return Status::DADOS_INVALIDOS;
    }
    if (textoVazio(nome)) {
        mensagem = "O nome do produto nao pode ser vazio.";
        return Status::DADOS_INVALIDOS;
    }
    if (!std::isfinite(preco) || preco <= 0.0) {
        mensagem = "O preco deve ser maior que zero.";
        return Status::DADOS_INVALIDOS;
    }
    if (estoque < 0) {
        mensagem = "O estoque nao pode ser negativo.";
        return Status::DADOS_INVALIDOS;
    }
    if (produtos_.buscar(codigo) != nullptr) {
        mensagem = "Ja existe um produto com o codigo " +
                   std::to_string(codigo) + ".";
        return Status::CODIGO_DUPLICADO;
    }

    Produto produto;
    produto.codigo = codigo;
    produto.nome = nome;
    produto.preco = preco;
    produto.estoque = estoque;
    produto.vendidos = 0;

    if (!produtos_.inserir(produto)) {
        mensagem = "Nao foi possivel cadastrar o produto.";
        return Status::DADOS_INVALIDOS;
    }

    registrarHistorico(TipoOperacao::CADASTRO_PRODUTO,
                       "Produto " + std::to_string(codigo) + " - " + nome +
                           " cadastrado com estoque inicial " +
                           std::to_string(estoque) +
                           " (preco " + formatarMoeda(preco) + ")",
                       0.0);

    mensagem = "Produto " + nome + " cadastrado com sucesso.";
    return Status::OK;
}

const Produto* Mercado::consultarProduto(int codigo) const {
    return produtos_.buscar(codigo);
}

std::vector<Produto> Mercado::buscarPorNome(const std::string& trecho) const {
    const std::string alvo = normalizar(trecho);
    std::vector<Produto> encontrados;

    for (const Produto& produto : produtos_.paraVetor()) {
        if (alvo.empty() || normalizar(produto.nome).find(alvo) !=
                                std::string::npos) {
            encontrados.push_back(produto);
        }
    }

    return encontrados;
}

std::vector<Produto> Mercado::produtosMaisVendidos(int quantidade) const {
    std::vector<Produto> ranking;
    for (const Produto& produto : produtos_.paraVetor()) {
        if (produto.vendidos > 0) {
            ranking.push_back(produto);
        }
    }

    std::sort(ranking.begin(), ranking.end(),
              [](const Produto& a, const Produto& b) {
                  if (a.vendidos != b.vendidos) {
                      return a.vendidos > b.vendidos;
                  }
                  if (a.nome != b.nome) {
                      return a.nome < b.nome;
                  }
                  return a.codigo < b.codigo;
              });

    if (quantidade > 0 &&
        quantidade < static_cast<int>(ranking.size())) {
        ranking.resize(static_cast<std::size_t>(quantidade));
    }

    return ranking;
}

void Mercado::listarProdutos() const {
    produtos_.listar();
}

bool Mercado::estoqueSuficiente(const std::vector<ItemCompra>& itens,
                                std::string& mensagem) const {
    std::map<int, int> necessarios;

    for (const ItemCompra& item : itens) {
        if (item.quantidade <= 0) {
            mensagem = "Quantidade invalida para o produto " +
                       std::to_string(item.codigoProduto) + ".";
            return false;
        }
        necessarios[item.codigoProduto] += item.quantidade;
    }

    for (const auto& [codigo, quantidade] : necessarios) {
        const Produto* produto = produtos_.buscar(codigo);
        if (produto == nullptr) {
            mensagem = "Produto de codigo " + std::to_string(codigo) +
                       " nao encontrado.";
            return false;
        }
        if (produto->estoque < quantidade) {
            mensagem = "Estoque insuficiente para '" + produto->nome +
                       "': disponivel " + std::to_string(produto->estoque) +
                       ", solicitado " + std::to_string(quantidade) + ".";
            return false;
        }
    }

    return true;
}

Status Mercado::registrarCompra(const std::string& nomeCliente,
                                const std::vector<ItemCompra>& itens, int caixa,
                                int& caixaAtribuido, std::string& mensagem) {
    if (textoVazio(nomeCliente)) {
        mensagem = "O nome do cliente nao pode ser vazio.";
        return Status::DADOS_INVALIDOS;
    }
    if (itens.empty()) {
        mensagem = "A compra precisa ter pelo menos um item.";
        return Status::DADOS_INVALIDOS;
    }

    std::vector<ItemCompra> carrinho;
    carrinho.reserve(itens.size());

    for (const ItemCompra& item : itens) {
        if (item.codigoProduto <= 0 || item.quantidade <= 0) {
            mensagem = "Item invalido no carrinho.";
            return Status::DADOS_INVALIDOS;
        }

        const Produto* produto = produtos_.buscar(item.codigoProduto);
        if (produto == nullptr) {
            mensagem = "Produto de codigo " +
                       std::to_string(item.codigoProduto) +
                       " nao encontrado.";
            return Status::PRODUTO_NAO_ENCONTRADO;
        }

        carrinho.push_back(ItemCompra{produto->codigo, produto->nome,
                                      item.quantidade, produto->preco});
    }

    if (!estoqueSuficiente(carrinho, mensagem)) {
        return Status::ESTOQUE_INSUFICIENTE;
    }

    if (caixa == -1) {
        caixa = escolherCaixaMenorFila();
    } else if (!caixaValido(caixa)) {
        mensagem = "Caixa invalido. Informe um valor entre 1 e " +
                   std::to_string(numeroCaixas()) + ".";
        return Status::CAIXA_INVALIDO;
    }

    Cliente cliente;
    cliente.id = proximoIdCliente_++;
    cliente.nome = nomeCliente;
    cliente.carrinho = std::move(carrinho);

    const int idCliente = cliente.id;
    const double total = cliente.total();
    const std::size_t itensCarrinho = cliente.carrinho.size();

    caixas_[static_cast<std::size_t>(caixa - 1)].enfileirar(cliente);
    ++comprasRegistradas_;
    caixaAtribuido = caixa;

    registrarHistorico(
        TipoOperacao::REGISTRO_COMPRA,
        "Cliente " + nomeCliente + " (id " + std::to_string(idCliente) +
            ") entrou na fila do caixa " + std::to_string(caixa) + " com " +
            std::to_string(itensCarrinho) + " item(ns)",
        total);

    mensagem = "Cliente " + nomeCliente + " (id " + std::to_string(idCliente) +
               ") entrou na fila do caixa " + std::to_string(caixa) + ".";
    return Status::OK;
}

Status Mercado::finalizarAtendimento(int caixa, Cliente& atendido,
                                     std::string& mensagem) {
    if (!caixaValido(caixa)) {
        mensagem = "Caixa invalido. Informe um valor entre 1 e " +
                   std::to_string(numeroCaixas()) + ".";
        return Status::CAIXA_INVALIDO;
    }

    FilaClientes& fila = caixas_[static_cast<std::size_t>(caixa - 1)];
    if (fila.vazia()) {
        mensagem = "O caixa " + std::to_string(caixa) + " nao tem clientes.";
        return Status::FILA_VAZIA;
    }

    Cliente cliente = *fila.frente();

    if (!estoqueSuficiente(cliente.carrinho, mensagem)) {
        mensagem = "Atendimento bloqueado: " + mensagem;
        return Status::ESTOQUE_INSUFICIENTE;
    }

    fila.desenfileirar(atendido);

    for (const ItemCompra& item : atendido.carrinho) {
        Produto* produto = produtos_.buscar(item.codigoProduto);
        if (produto != nullptr) {
            produto->estoque -= item.quantidade;
            produto->vendidos += item.quantidade;
        }
    }

    const double total = atendido.total();
    faturamentoTotal_ += total;
    ++comprasFinalizadas_;

    registrarHistorico(
        TipoOperacao::FINALIZACAO,
        "Cliente " + atendido.nome + " (id " +
            std::to_string(atendido.id) + ") atendido no caixa " +
            std::to_string(caixa) + " com " +
            std::to_string(atendido.carrinho.size()) + " item(ns)",
        total);

    mensagem = "Atendimento do cliente " + atendido.nome +
               " finalizado no caixa " + std::to_string(caixa) + ".";
    return Status::OK;
}

Status Mercado::registrarDesistencia(int caixa, int idCliente,
                                     Cliente& desistente,
                                     std::string& mensagem) {
    if (!caixaValido(caixa)) {
        mensagem = "Caixa invalido. Informe um valor entre 1 e " +
                   std::to_string(numeroCaixas()) + ".";
        return Status::CAIXA_INVALIDO;
    }

    FilaClientes& fila = caixas_[static_cast<std::size_t>(caixa - 1)];
    if (fila.vazia()) {
        mensagem = "O caixa " + std::to_string(caixa) + " nao tem clientes.";
        return Status::FILA_VAZIA;
    }

    if (!fila.removerPorId(idCliente, desistente)) {
        mensagem = "Cliente de id " + std::to_string(idCliente) +
                   " nao esta na fila do caixa " + std::to_string(caixa) +
                   ".";
        return Status::CLIENTE_NAO_ENCONTRADO;
    }

    ++desistencias_;
    registrarHistorico(
        TipoOperacao::DESISTENCIA,
        "Cliente " + desistente.nome + " (id " +
            std::to_string(desistente.id) + ") desistiu da fila do caixa " +
            std::to_string(caixa) + " (total potencial " +
            formatarMoeda(desistente.total()) + ")",
        0.0);

    mensagem = "Desistencia do cliente " + desistente.nome +
               " registrada no caixa " + std::to_string(caixa) + ".";
    return Status::OK;
}

void Mercado::listarFilas() const {
    for (int i = 0; i < numeroCaixas(); ++i) {
        const FilaClientes& fila = caixas_[static_cast<std::size_t>(i)];
        std::cout << "Caixa " << (i + 1) << " (" << fila.tamanho()
                  << " cliente(s)):\n";
        fila.listar();
    }
}

void Mercado::listarMaisVendidos(int quantidade) const {
    const std::vector<Produto> ranking = produtosMaisVendidos(quantidade);

    if (ranking.empty()) {
        std::cout << "Nenhuma venda registrada ate o momento.\n";
        return;
    }

    std::cout << std::left << std::setw(6) << "Rank" << std::setw(8)
              << "Codigo" << std::setw(28) << "Produto" << std::right
              << std::setw(10) << "Vendidos" << std::setw(14) << "Receita"
              << '\n';

    int posicao = 1;
    for (const Produto& produto : ranking) {
        std::string nome = produto.nome;
        if (nome.size() > 26) {
            nome = nome.substr(0, 25) + ".";
        }

        std::cout << std::left << std::setw(6) << posicao++ << std::setw(8)
                  << produto.codigo << std::setw(28) << nome << std::right
                  << std::setw(10) << produto.vendidos << std::setw(14)
                  << formatarMoeda(produto.preco * produto.vendidos) << '\n';
    }
}

void Mercado::mostrarHistorico() const {
    historico_.listar();
}

void Mercado::exibirResumo() const {
    int clientesNasFilas = 0;
    for (const FilaClientes& fila : caixas_) {
        clientesNasFilas += fila.tamanho();
    }

    const double ticketMedio =
        comprasFinalizadas_ > 0 ? faturamentoTotal_ / comprasFinalizadas_ : 0.0;

    std::cout << "Produtos cadastrados...: " << produtos_.tamanho() << '\n'
              << "Compras registradas...: " << comprasRegistradas_ << '\n'
              << "Compras finalizadas...: " << comprasFinalizadas_ << '\n'
              << "Desistencias...........: " << desistencias_ << '\n'
              << "Clientes nas filas.....: " << clientesNasFilas << '\n'
              << "Operacoes no historico.: " << historico_.tamanho() << '\n'
              << "Faturamento total......: " << formatarMoeda(faturamentoTotal_)
              << '\n'
              << "Ticket medio...........: " << formatarMoeda(ticketMedio)
              << '\n';

    for (int i = 0; i < numeroCaixas(); ++i) {
        std::cout << "Caixa " << (i + 1) << " - fila: "
                  << caixas_[static_cast<std::size_t>(i)].tamanho()
                  << " cliente(s)\n";
    }
}

int Mercado::numeroCaixas() const {
    return static_cast<int>(caixas_.size());
}

int Mercado::tamanhoFila(int caixa) const {
    if (!caixaValido(caixa)) {
        return -1;
    }
    return caixas_[static_cast<std::size_t>(caixa - 1)].tamanho();
}

int Mercado::totalOperacoes() const {
    return historico_.tamanho();
}

int Mercado::totalComprasFinalizadas() const {
    return comprasFinalizadas_;
}

double Mercado::faturamentoTotal() const {
    return faturamentoTotal_;
}
