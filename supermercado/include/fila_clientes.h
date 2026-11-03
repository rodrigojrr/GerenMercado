#pragma once

#include "cliente.h"

struct NoCliente {
    Cliente dado;
    NoCliente* proximo = nullptr;
};

class FilaClientes {
public:
    FilaClientes() = default;
    ~FilaClientes();

    FilaClientes(const FilaClientes&) = delete;
    FilaClientes& operator=(const FilaClientes&) = delete;
    FilaClientes(FilaClientes&& outra) noexcept;
    FilaClientes& operator=(FilaClientes&& outra) noexcept;

    void enfileirar(const Cliente& cliente);
    bool desenfileirar(Cliente& saida);
    bool removerPorId(int id, Cliente& saida);
    Cliente* frente();
    const Cliente* frente() const;
    int tamanho() const;
    bool vazia() const;
    void listar() const;

private:
    void liberar();

    NoCliente* inicio_ = nullptr;
    NoCliente* fim_ = nullptr;
    int tamanho_ = 0;
};
