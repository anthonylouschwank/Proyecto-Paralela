#include "hud.h"

#include <cstdio>

#include "texto.h"

namespace {

constexpr int kEscala = 2;   // cada punto de la fuente mide 2x2
constexpr int kMargen = 8;   // margen del recuadro
constexpr int kLineas = 4;

}  // namespace

void dibujarHud(SDL_Renderer* renderer, const DatosHud& datos) {
    char lineas[kLineas][48];
    std::snprintf(lineas[0], sizeof(lineas[0]), "FPS: %.1f", datos.fps);
    std::snprintf(lineas[1], sizeof(lineas[1]), "N: %d  HILOS: %d  ZONA: %d%%",
                  datos.n, datos.hilos, datos.porcentajeZona);
    std::snprintf(lineas[2], sizeof(lineas[2]), "SEC: %.2f MS", datos.tiempos.msSecuencial);
    std::snprintf(lineas[3], sizeof(lineas[3]), "OMP: %.2f MS", datos.tiempos.msParalelo);

    const int altoLinea = altoTexto(kEscala);
    const int separacion = kEscala * 3;

    int anchoMax = 0;
    for (const char* linea : lineas) {
        anchoMax = SDL_max(anchoMax, anchoTexto(linea, kEscala));
    }

    // Recuadro semitransparente detras del texto.
    const SDL_Rect fondo{
        kMargen,
        kMargen,
        anchoMax + kMargen * 2,
        altoLinea * kLineas + separacion * (kLineas - 1) + kMargen * 2,
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &fondo);

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    int y = fondo.y + kMargen;
    for (const char* linea : lineas) {
        dibujarTexto(renderer, linea, fondo.x + kMargen, y, kEscala);
        y += altoLinea + separacion;
    }
}
