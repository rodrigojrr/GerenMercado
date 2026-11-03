#include "menu.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "util.h"

namespace {

std::string trim(const std::string& texto) {
    const auto inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) {
        return "";
    }
    const auto fim = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fim - inicio + 1);
}

std::string lerLinha(const std::string& prompt) {
    std::cout << prompt;
    std::string linha;
    if (!std::getline(std::cin, linha)) {
        std::cout << "\nEntrada encerrada. Saindo.\n";
        std::exit(EXIT_SUCCESS);
    }
    return linha;
}

int lerInteiro(const std::string& prompt) {
    while (true) {
        const std::string linha = lerLinha(prompt);
        std::istringstream ss(linha);
        int valor = 0;
        char extra = '\0';
        if (ss >> valor && !(ss >> extra)) {
            return valor;
        }
        std::cout << "Entrada invalida. Digite um numero inteiro.\n";
    }
}

double lerDouble(const std::string& prompt) {
    while (true) {
        const std::string linha = lerLinha(prompt);
        std::istringstream ss(linha);
        double valor = 0.0;
        char extra = '\0';
        if (ss >> valor && !(ss >> extra)) {
            return valor;
        }
        std::cout << "Entrada invalida. Digite um numero.\n";
    }
}

std::string lerTexto(const std::string& prompt) {
    while (true) {
        const std::string texto = trim(lerLinha(prompt));
        if (!texto.empty()) {
            return texto;
        }
        std::cout << "O texto nao pode ser vazio.\n";
    }
}

void imprimirStatus(Status status, const std::string& mensagem) {
    if (status != Status::OK) {
        std::cout << "Erro: ";
    }
    std::cout << mensagem << '\n';
}

}  // namespace

Menu::Menu(Mercado& mercado) : mercado_(mercado) {}

void Menu::exibirMenu() const {
    std::cout << "\n===== MENU =====\n"
              << "1  Cadastrar produto\n"
              << "2  Consultar preco por codigo\n"
              << "3  Buscar produtos por nome\n"
              << "4  Listar produtos\n"
              << "5  Registrar compra\n"
              << "6  Listar filas dos caixas\n"
              << "7  Finalizar atendimento\n"
              << "8  Registrar desistencia da fila\n"
              << "9  Historico de operacoes\n"
              << "10 Produtos mais vendidos\n"
              << "11 Resumo geral\n"
              << "0  Sair\n";
}

void Menu::executar() {
    std::cout << "Sistema de Supermercado e Filas de Caixa\n";
    std::cout << "Caixas ativos: " << mercado_.numeroCaixas() << '\n';

    while (true) {
        exibirMenu();
        const int opcao = lerInteiro("Opcao: ");
        std::cout << '\n';

        switch (opcao) {
            case 1:
                cadastrarProduto();
                break;
            case 2:
                consultarPreco();
                break;
            case 3:
                buscarPorNome();
                break;
            case 4:
                mercado_.listarProdutos();
                break;
            case 5:
                registrarCompra();
                break;
            case 6:
                listarFilas();
                break;
            case 7:
                finalizarAtendimento();
                break;
            case 8:
                registrarDesistencia();
                break;
            case 9:
                mercado_.mostrarHistorico();
                break;
            case 10:
                listarMaisVendidos();
                break;
            case 11:
                mercado_.exibirResumo();
                break;
            case 0:
                std::cout << "Encerrando o sistema. Ate logo!\n";
                return;
            default:
                std::cout << "Opcao invalida.\n";
                break;
        }
    }
}

void Menu::cadastrarProduto() {
    const int codigo = lerInteiro("Codigo: ");
    const std::string nome = lerTexto("Nome: ");
    const double preco = lerDouble("Preco: ");
    const int estoque = lerInteiro("Estoque inicial: ");

    std::string mensagem;
    const Status status = mercado_.cadastrarProduto(codigo, nome, preco,
                                                    estoque, mensagem);
    imprimirStatus(status, mensagem);
}

void Menu::consultarPreco() {
    const int codigo = lerInteiro("Codigo do produto: ");
    const Produto* produto = mercado_.consultarProduto(codigo);

    if (produto == nullptr) {
        std::cout << "Produto nao encontrado.\n";
        return;
    }

    std::cout << "Codigo..: " << produto->codigo << '\n'
              << "Nome....: " << produto->nome << '\n'
              << "Preco...: " << formatarMoeda(produto->preco) << '\n'
              << "Estoque.: " << produto->estoque << '\n'
              << "Vendidos: " << produto->vendidos << '\n';
}

void Menu::buscarPorNome() {
    const std::string trecho = lerTexto("Trecho do nome: ");
    const std::vector<Produto> encontrados = mercado_.buscarPorNome(trecho);

    if (encontrados.empty()) {
        std::cout << "Nenhum produto encontrado.\n";
        return;
    }

    std::cout << std::left << std::setw(8) << "Codigo" << std::setw(28)
              << "Nome" << std::right << std::setw(12) << "Preco"
              << std::setw(10) << "Estoque" << '\n';

    for (const Produto& produto : encontrados) {
        std::string nome = produto.nome;
        if (nome.size() > 26) {
            nome = nome.substr(0, 25) + ".";
        }

        std::cout << std::left << std::setw(8) << produto.codigo
                  << std::setw(28) << nome << std::right << std::setw(12)
                  << formatarMoeda(produto.preco) << std::setw(10)
                  << produto.estoque << '\n';
    }
}

void Menu::registrarCompra() {
    const std::string nomeCliente = lerTexto("Nome do cliente: ");
    std::vector<ItemCompra> itens;

    while (true) {
        const int codigo =
            lerInteiro("Codigo do produto (0 para concluir): ");
        if (codigo == 0) {
            break;
        }

        const Produto* produto = mercado_.consultarProduto(codigo);
        if (produto == nullptr) {
            std::cout << "Produto nao encontrado.\n";
            continue;
        }

        const int quantidade = lerInteiro("Quantidade: ");
        if (quantidade <= 0) {
            std::cout << "A quantidade deve ser positiva.\n";
            continue;
        }

        itens.push_back(ItemCompra{produto->codigo, produto->nome, quantidade,
                                   produto->preco});
        std::cout << "Adicionado: " << produto->nome << " x" << quantidade
                  << " = " << formatarMoeda(produto->preco * quantidade)
                  << '\n';
    }

    if (itens.empty()) {
        std::cout << "Compra cancelada: nenhum item informado.\n";
        return;
    }

    int caixa = lerInteiro("Caixa (1.." +
                           std::to_string(mercado_.numeroCaixas()) +
                           ", 0 para automatico): ");
    if (caixa == 0) {
        caixa = -1;
    }

    int caixaAtribuido = 0;
    std::string mensagem;
    const Status status = mercado_.registrarCompra(
        nomeCliente, itens, caixa, caixaAtribuido, mensagem);
    imprimirStatus(status, mensagem);
}

void Menu::listarFilas() {
    mercado_.listarFilas();
}

void Menu::finalizarAtendimento() {
    const int caixa = lerInteiro("Numero do caixa: ");

    Cliente atendido;
    std::string mensagem;
    const Status status =
        mercado_.finalizarAtendimento(caixa, atendido, mensagem);

    if (status != Status::OK) {
        imprimirStatus(status, mensagem);
        return;
    }

    std::cout << "=== Comprovante ===\n"
              << "Cliente: " << atendido.nome << " (id " << atendido.id
              << ")\n";

    for (const ItemCompra& item : atendido.carrinho) {
        std::cout << "  - " << item.nome << " x" << item.quantidade << " x "
                  << formatarMoeda(item.precoUnitario) << " = "
                  << formatarMoeda(item.subtotal()) << '\n';
    }

    std::cout << "Total..: " << formatarMoeda(atendido.total()) << '\n';
    imprimirStatus(status, mensagem);
}

void Menu::registrarDesistencia() {
    mercado_.listarFilas();
    const int caixa = lerInteiro("Numero do caixa: ");
    const int idCliente = lerInteiro("Id do cliente: ");

    Cliente desistente;
    std::string mensagem;
    const Status status = mercado_.registrarDesistencia(
        caixa, idCliente, desistente, mensagem);
    imprimirStatus(status, mensagem);
}

void Menu::listarMaisVendidos() {
    const int quantidade =
        lerInteiro("Quantos produtos listar (0 = todos): ");
    mercado_.listarMaisVendidos(quantidade);
}
