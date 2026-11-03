#pragma once

#include "mercado.h"

class Menu {
public:
    explicit Menu(Mercado& mercado);

    void executar();

private:
    void exibirMenu() const;
    void cadastrarProduto();
    void consultarPreco();
    void buscarPorNome();
    void registrarCompra();
    void listarFilas();
    void finalizarAtendimento();
    void registrarDesistencia();
    void listarMaisVendidos();

    Mercado& mercado_;
};
