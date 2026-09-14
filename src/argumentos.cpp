#include "argumentos.h"

#include <cerrno>
#include <cstdlib>
#include <string>

bool parsearEntero(const char* texto, long min, long max, long& salida) {
    char* fin = nullptr;
    errno = 0;
    const long valor = std::strtol(texto, &fin, 10);
    if (errno != 0 || fin == texto || *fin != '\0' || valor < min || valor > max) {
        return false;
    }
    salida = valor;
    return true;
}

bool parsearListaEnteros(const char* texto, long min, long max, std::vector<int>& salida) {
    std::vector<int> valores;
    const std::string lista(texto);
    std::size_t inicio = 0;

    while (inicio <= lista.size()) {
        std::size_t coma = lista.find(',', inicio);
        if (coma == std::string::npos) {
            coma = lista.size();
        }
        long valor = 0;
        if (!parsearEntero(lista.substr(inicio, coma - inicio).c_str(), min, max, valor)) {
            return false;
        }
        valores.push_back(static_cast<int>(valor));
        inicio = coma + 1;
    }

    salida = valores;
    return true;
}
