#pragma once

#include <vector>

#include "produto.h"

struct NoProduto {
    Produto dado;
    NoProduto* proximo = nullptr;
};

class ListaProdutos {
public:
    ListaProdutos() = default;
    ~ListaProdutos();

    ListaProdutos(const ListaProdutos&) = delete;
    ListaProdutos& operator=(const ListaProdutos&) = delete;

    bool inserir(const Produto& produto);
    Produto* buscar(int codigo);
    const Produto* buscar(int codigo) const;
    bool remover(int codigo);
    bool vazia() const;
    int tamanho() const;
    void listar() const;
    std::vector<Produto> paraVetor() const;

private:
    void liberar();

    NoProduto* inicio_ = nullptr;
    int tamanho_ = 0;
};
