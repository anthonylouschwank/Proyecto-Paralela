#pragma once

#include <SDL.h>

#include "simulacion.h"

// Lo que muestra el HUD.
struct DatosHud {
    float fps;
    int n;
    int hilos;
    int porcentajeZona;
    TiemposFrame tiempos;  // ms por frame de cada zona
};

// Dibuja en la esquina superior izquierda un recuadro con FPS, N, hilos,
// tamano de la zona paralela y cuanto tarda cada zona por frame.
void dibujarHud(SDL_Renderer* renderer, const DatosHud& datos);
