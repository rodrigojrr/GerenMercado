#pragma once

#include <string>

struct Produto {
    int codigo = 0;
    std::string nome;
    double preco = 0.0;
    int estoque = 0;
    int vendidos = 0;
};
