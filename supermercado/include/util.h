#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

inline std::string formatarMoeda(double valor) {
    std::ostringstream ss;
    ss << "R$ " << std::fixed << std::setprecision(2) << valor;
    return ss.str();
}

inline std::string horarioAtual() {
    const auto agora = std::chrono::system_clock::now();
    const std::time_t momento = std::chrono::system_clock::to_time_t(agora);

    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &momento);
#else
    localtime_r(&momento, &tm);
#endif

    std::ostringstream ss;
    ss << std::put_time(&tm, "%H:%M:%S");
    return ss.str();
}
