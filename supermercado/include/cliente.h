#pragma once

#include <string>
#include <vector>

#include "produto.h"

struct ItemCompra {
    int codigoProduto = 0;
    std::string nome;
    int quantidade = 0;
    double precoUnitario = 0.0;

    double subtotal() const {
        return quantidade * precoUnitario;
    }
};

struct Cliente {
    int id = 0;
    std::string nome;
    std::vector<ItemCompra> carrinho;

    double total() const {
        double soma = 0.0;
        for (const ItemCompra& item : carrinho) {
            soma += item.subtotal();
        }
        return soma;
    }
};
