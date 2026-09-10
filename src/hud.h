#pragma once

#include <SDL.h>

// Mide los FPS. El valor mostrado se recalcula cada medio segundo para que
// el numero sea legible; ademas lleva el total para reportar el promedio de
// toda la corrida al cerrar.
class ContadorFps {
public:
    // Registra que paso un frame de dt segundos.
    void registrarFrame(float dt);

    // FPS de la ultima ventana de medicion (~0.5 s).
    float fpsActual() const { return fpsActual_; }

    // FPS promedio desde que arranco el programa.
    float fpsPromedio() const;

private:
    static constexpr float kIntervalo = 0.5f;

    float tiempoIntervalo_ = 0.0f;
    int framesIntervalo_ = 0;
    float fpsActual_ = 0.0f;

    double tiempoTotal_ = 0.0;
    long long framesTotales_ = 0;
};

// Dibuja en la esquina superior izquierda un recuadro con los FPS y N.
// Usa una fuente de pixeles propia, asi no dependemos de SDL2_ttf.
void dibujarHud(SDL_Renderer* renderer, float fps, int n);
