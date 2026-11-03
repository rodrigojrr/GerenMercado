#pragma once

#include <string>

enum class TipoOperacao {
    CADASTRO_PRODUTO,
    REGISTRO_COMPRA,
    FINALIZACAO,
    DESISTENCIA
};

struct Operacao {
    TipoOperacao tipo = TipoOperacao::REGISTRO_COMPRA;
    std::string descricao;
    double valor = 0.0;
    std::string horario;
};

inline std::string nomeTipoOperacao(TipoOperacao tipo) {
    switch (tipo) {
        case TipoOperacao::CADASTRO_PRODUTO:
            return "Cadastro de produto";
        case TipoOperacao::REGISTRO_COMPRA:
            return "Registro de compra";
        case TipoOperacao::FINALIZACAO:
            return "Finalizacao de atendimento";
        case TipoOperacao::DESISTENCIA:
            return "Desistencia de fila";
    }
    return "Desconhecida";
}
