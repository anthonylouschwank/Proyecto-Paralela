#pragma once

#include "simulacion.h"

// Mide FPS y los milisegundos que tarda cada zona en simularse. Los valores
// "actuales" se recalculan cada medio segundo para que el HUD sea legible;
// ademas lleva los totales para reportar el promedio de toda la corrida.
class Estadisticas {
public:
    // Registra un frame que duro dt segundos y lo que tardo cada zona.
    void registrarFrame(float dt, const TiemposFrame& tiempos);

    // Promedios de la ultima ventana de medicion (~0.5 s).
    float fpsActual() const { return fpsActual_; }
    const TiemposFrame& tiemposActuales() const { return tiemposActuales_; }

    // Promedios desde que arranco el programa.
    float fpsPromedio() const;
    TiemposFrame tiemposPromedio() const;

private:
    static constexpr float kIntervalo = 0.5f;

    float tiempoIntervalo_ = 0.0f;
    int framesIntervalo_ = 0;
    TiemposFrame sumaIntervalo_;
    float fpsActual_ = 0.0f;
    TiemposFrame tiemposActuales_;

    double tiempoTotal_ = 0.0;
    long long framesTotales_ = 0;
    TiemposFrame sumaTotal_;
};
