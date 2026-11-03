#include "fila_clientes.h"

#include <iostream>
#include <utility>

#include "util.h"

FilaClientes::~FilaClientes() {
    liberar();
}

FilaClientes::FilaClientes(FilaClientes&& outra) noexcept
    : inicio_(outra.inicio_), fim_(outra.fim_), tamanho_(outra.tamanho_) {
    outra.inicio_ = nullptr;
    outra.fim_ = nullptr;
    outra.tamanho_ = 0;
}

FilaClientes& FilaClientes::operator=(FilaClientes&& outra) noexcept {
    if (this != &outra) {
        liberar();
        inicio_ = outra.inicio_;
        fim_ = outra.fim_;
        tamanho_ = outra.tamanho_;
        outra.inicio_ = nullptr;
        outra.fim_ = nullptr;
        outra.tamanho_ = 0;
    }
    return *this;
}

void FilaClientes::liberar() {
    NoCliente* atual = inicio_;
    while (atual != nullptr) {
        NoCliente* proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }
    inicio_ = nullptr;
    fim_ = nullptr;
    tamanho_ = 0;
}

void FilaClientes::enfileirar(const Cliente& cliente) {
    NoCliente* novo = new NoCliente{cliente, nullptr};

    if (fim_ == nullptr) {
        inicio_ = novo;
        fim_ = novo;
    } else {
        fim_->proximo = novo;
        fim_ = novo;
    }

    ++tamanho_;
}

bool FilaClientes::desenfileirar(Cliente& saida) {
    if (inicio_ == nullptr) {
        return false;
    }

    NoCliente* removido = inicio_;
    saida = std::move(removido->dado);
    inicio_ = removido->proximo;

    if (inicio_ == nullptr) {
        fim_ = nullptr;
    }

    delete removido;
    --tamanho_;
    return true;
}

bool FilaClientes::removerPorId(int id, Cliente& saida) {
    NoCliente* anterior = nullptr;
    NoCliente* atual = inicio_;

    while (atual != nullptr && atual->dado.id != id) {
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

    if (atual == fim_) {
        fim_ = anterior;
    }

    saida = std::move(atual->dado);
    delete atual;
    --tamanho_;
    return true;
}

Cliente* FilaClientes::frente() {
    return inicio_ == nullptr ? nullptr : &inicio_->dado;
}

const Cliente* FilaClientes::frente() const {
    return inicio_ == nullptr ? nullptr : &inicio_->dado;
}

int FilaClientes::tamanho() const {
    return tamanho_;
}

bool FilaClientes::vazia() const {
    return inicio_ == nullptr;
}

void FilaClientes::listar() const {
    if (inicio_ == nullptr) {
        std::cout << "  (fila vazia)\n";
        return;
    }

    for (const NoCliente* atual = inicio_; atual != nullptr;
         atual = atual->proximo) {
        const Cliente& cliente = atual->dado;
        std::cout << "  #" << cliente.id << " " << cliente.nome << " - "
                  << cliente.carrinho.size() << " item(ns) - total "
                  << formatarMoeda(cliente.total()) << '\n';
    }
}
