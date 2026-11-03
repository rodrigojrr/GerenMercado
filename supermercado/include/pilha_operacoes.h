#pragma once

#include "operacao.h"

struct NoOperacao {
    Operacao dado;
    NoOperacao* proximo = nullptr;
};

class PilhaOperacoes {
public:
    PilhaOperacoes() = default;
    ~PilhaOperacoes();

    PilhaOperacoes(const PilhaOperacoes&) = delete;
    PilhaOperacoes& operator=(const PilhaOperacoes&) = delete;

    void empilhar(const Operacao& operacao);
    bool desempilhar(Operacao& saida);
    const Operacao* topo() const;
    bool vazia() const;
    int tamanho() const;
    void listar() const;

private:
    void liberar();

    NoOperacao* topo_ = nullptr;
    int tamanho_ = 0;
};
