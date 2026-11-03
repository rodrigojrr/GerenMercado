#include "lista_produtos.h"

#include <iomanip>
#include <iostream>
#include <string>

#include "util.h"

ListaProdutos::~ListaProdutos() {
    liberar();
}

void ListaProdutos::liberar() {
    NoProduto* atual = inicio_;
    while (atual != nullptr) {
        NoProduto* proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }
    inicio_ = nullptr;
    tamanho_ = 0;
}

bool ListaProdutos::inserir(const Produto& produto) {
    NoProduto* novo = new NoProduto{produto, nullptr};

    if (inicio_ == nullptr || produto.codigo < inicio_->dado.codigo) {
        novo->proximo = inicio_;
        inicio_ = novo;
        ++tamanho_;
        return true;
    }

    NoProduto* atual = inicio_;
    while (atual->proximo != nullptr &&
           atual->proximo->dado.codigo < produto.codigo) {
        atual = atual->proximo;
    }

    if (atual->dado.codigo == produto.codigo ||
        (atual->proximo != nullptr &&
         atual->proximo->dado.codigo == produto.codigo)) {
        delete novo;
        return false;
    }

    novo->proximo = atual->proximo;
    atual->proximo = novo;
    ++tamanho_;
    return true;
}

Produto* ListaProdutos::buscar(int codigo) {
    for (NoProduto* atual = inicio_; atual != nullptr; atual = atual->proximo) {
        if (atual->dado.codigo == codigo) {
            return &atual->dado;
        }
    }
    return nullptr;
}

const Produto* ListaProdutos::buscar(int codigo) const {
    for (const NoProduto* atual = inicio_; atual != nullptr;
         atual = atual->proximo) {
        if (atual->dado.codigo == codigo) {
            return &atual->dado;
        }
    }
    return nullptr;
}

bool ListaProdutos::remover(int codigo) {
    NoProduto* anterior = nullptr;
    NoProduto* atual = inicio_;

    while (atual != nullptr && atual->dado.codigo != codigo) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == nullptr) {
        return false;
    }

    if (anterior == nullptr) {
        inicio_ = atual->proximo;
    } else {
        anterior->proximo = atual->proximo;
    }

    delete atual;
    --tamanho_;
    return true;
}

bool ListaProdutos::vazia() const {
    return inicio_ == nullptr;
}

int ListaProdutos::tamanho() const {
    return tamanho_;
}

void ListaProdutos::listar() const {
    if (inicio_ == nullptr) {
        std::cout << "Nenhum produto cadastrado.\n";
        return;
    }

    std::cout << std::left << std::setw(8) << "Codigo" << std::setw(28)
              << "Nome" << std::right << std::setw(12) << "Preco"
              << std::setw(10) << "Estoque" << std::setw(10) << "Vendidos"
              << '\n';

    for (const NoProduto* atual = inicio_; atual != nullptr;
         atual = atual->proximo) {
        const Produto& produto = atual->dado;
        std::string nome = produto.nome;
        if (nome.size() > 26) {
            nome = nome.substr(0, 25) + ".";
        }

        std::cout << std::left << std::setw(8) << produto.codigo
                  << std::setw(28) << nome << std::right << std::setw(12)
                  << formatarMoeda(produto.preco) << std::setw(10)
                  << produto.estoque << std::setw(10) << produto.vendidos
                  << '\n';
    }
}

std::vector<Produto> ListaProdutos::paraVetor() const {
    std::vector<Produto> produtos;
    produtos.reserve(static_cast<std::size_t>(tamanho_));

    for (const NoProduto* atual = inicio_; atual != nullptr;
         atual = atual->proximo) {
        produtos.push_back(atual->dado);
    }

    return produtos;
}
