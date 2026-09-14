#pragma once

#include <vector>

// Ayudas para leer argumentos de linea de comandos. Las usan tanto el
// screensaver como el benchmark.

// Convierte texto a entero dentro de [min, max]. Devuelve false si no es un
// entero valido o esta fuera de rango.
bool parsearEntero(const char* texto, long min, long max, long& salida);

// Convierte una lista separada por comas ("500,1000,2000") a enteros dentro
// de [min, max]. Devuelve false si algun elemento no es valido.
bool parsearListaEnteros(const char* texto, long min, long max, std::vector<int>& salida);
