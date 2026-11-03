#include <iostream>
#include <string>

#include "menu.h"
#include "mercado.h"

int main(int argc, char* argv[]) {
    int numeroCaixas = 3;

    if (argc > 1) {
        try {
            numeroCaixas = std::stoi(argv[1]);
        } catch (const std::exception&) {
            std::cout << "Numero de caixas invalido. Usando 3.\n";
            numeroCaixas = 3;
        }
    }

    if (numeroCaixas < 1) {
        numeroCaixas = 1;
    }

    Mercado mercado(numeroCaixas);
    Menu menu(mercado);
    menu.executar();

    return 0;
}
