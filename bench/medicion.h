#pragma once

#include <vector>

#include "chucho.h"

// Que version de la simulacion se mide.
enum class Version {
    Secuencial,  // mejor version secuencial: triangulo i < j (Parte 2)
    OpenMP,      // recorrido completo con OpenMP (Parte 3), con H hilos
};

const char* nombreVersion(Version version);

// Una configuracion a medir.
struct ConfigMedicion {
    int n;
    Version version;
    int hilos;                // solo cuenta para OpenMP
    int frames;               // frames medidos por repeticion
    int framesCalentamiento;  // frames que se corren antes de medir
    int repeticiones;
    unsigned int semilla;
};

// Milisegundos promedio por frame de cada etapa de la simulacion.
struct TiemposEtapas {
    double mover = 0.0;
    double detectar = 0.0;
    double aplicar = 0.0;

    double total() const { return mover + detectar + aplicar; }
};

struct ResultadoMedicion {
    ConfigMedicion config;
    std::vector<TiemposEtapas> porRepeticion;
    std::vector<Chucho> estadoFinal;  // para verificar que todas las versiones coinciden

    // Estadisticas del tiempo total por frame entre repeticiones.
    double promedio() const;
    double desviacion() const;  // desviacion estandar muestral
    double minimo() const;

    // Promedio de cada etapa entre repeticiones.
    TiemposEtapas promedioEtapas() const;
};

// Corre la simulacion (sin ventana) con la configuracion dada y mide
// cuanto tarda cada etapa por frame. En cada repeticion la escena se crea
// de nuevo con la misma semilla, asi todas miden exactamente el mismo
// trabajo.
ResultadoMedicion medir(const ConfigMedicion& config);

// true si los dos estados son identicos bit a bit.
bool mismosEstados(const std::vector<Chucho>& a, const std::vector<Chucho>& b);
