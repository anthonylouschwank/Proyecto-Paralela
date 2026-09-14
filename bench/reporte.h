#pragma once

#include <string>
#include <vector>

#include "medicion.h"

// Una medicion con sus metricas de comparacion ya calculadas.
struct FilaReporte {
    ResultadoMedicion resultado;

    // Tiempo secuencial (triangulo) / tiempo de esta fila.
    double speedupVsSecuencial = 1.0;

    // Tiempo de OpenMP con 1 hilo / tiempo de esta fila. Solo aplica a las
    // filas de OpenMP (en la secuencial queda en 0).
    double speedupVs1Hilo = 0.0;

    // speedupVsSecuencial / hilos
    double eficiencia = 1.0;

    // El estado final coincide bit a bit con el de la version secuencial.
    bool identico = true;
};

// Calcula speedup, eficiencia y verificacion. Para cada N debe haber una
// medicion secuencial; si hay una de OpenMP con 1 hilo, se usa para el
// speedup relativo.
std::vector<FilaReporte> armarReporte(const std::vector<ResultadoMedicion>& resultados);

// Imprime una tabla legible en la consola.
void imprimirTabla(const std::vector<FilaReporte>& filas);

// Escribe todas las filas a un CSV (crea la carpeta si no existe).
// Devuelve false si no se pudo escribir.
bool escribirCsv(const std::string& ruta, const std::vector<FilaReporte>& filas);

// Escribe una fila por cada repeticion (cada medicion individual), para la
// bitacora de pruebas del informe. Devuelve false si no se pudo escribir.
bool escribirCsvDetalle(const std::string& ruta, const std::vector<FilaReporte>& filas);
