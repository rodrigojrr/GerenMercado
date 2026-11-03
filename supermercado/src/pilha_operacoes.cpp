#include "pilha_operacoes.h"

#include <iomanip>
#include <iostream>

#include "util.h"

PilhaOperacoes::~PilhaOperacoes() {
    liberar();
}

void PilhaOperacoes::liberar() {
    NoOperacao* atual = topo_;
    while (atual != nullptr) {
        NoOperacao* proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }
    topo_ = nullptr;
    tamanho_ = 0;
}

void PilhaOperacoes::empilhar(const Operacao& operacao) {
    NoOperacao* novo = new NoOperacao{operacao, topo_};
    topo_ = novo;
    ++tamanho_;
}

bool PilhaOperacoes::desempilhar(Operacao& saida) {
    if (topo_ == nullptr) {
        return false;
    }

    NoOperacao* removido = topo_;
    saida = removido->dado;
    topo_ = removido->proximo;
    delete removido;
    --tamanho_;
    return true;
}

const Operacao* PilhaOperacoes::topo() const {
    return topo_ == nullptr ? nullptr : &topo_->dado;
}

bool PilhaOperacoes::vazia() const {
    return topo_ == nullptr;
}

int PilhaOperacoes::tamanho() const {
    return tamanho_;
}

void PilhaOperacoes::listar() const {
    if (topo_ == nullptr) {
        std::cout << "Nenhuma operacao registrada.\n";
        return;
    }

    std::cout << "Historico (mais recente primeiro):\n";
    int posicao = 0;
    for (const NoOperacao* atual = topo_; atual != nullptr;
         atual = atual->proximo) {
        const Operacao& operacao = atual->dado;
        std::cout << std::right << std::setw(3) << ++posicao << ". ["
                  << operacao.horario << "] " << std::left << std::setw(30)
                  << nomeTipoOperacao(operacao.tipo) << operacao.descricao;
        if (operacao.valor > 0.0) {
            std::cout << " (" << formatarMoeda(operacao.valor) << ")";
        }
        std::cout << '\n';
    }
}
