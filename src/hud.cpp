#include "hud.h"

#include <cstdio>
#include <cstring>

// ---------------------------------------------------------------------------
// ContadorFps
// ---------------------------------------------------------------------------

void ContadorFps::registrarFrame(float dt) {
    tiempoIntervalo_ += dt;
    ++framesIntervalo_;
    tiempoTotal_ += dt;
    ++framesTotales_;

    if (tiempoIntervalo_ >= kIntervalo) {
        fpsActual_ = static_cast<float>(framesIntervalo_) / tiempoIntervalo_;
        tiempoIntervalo_ = 0.0f;
        framesIntervalo_ = 0;
    }
}

float ContadorFps::fpsPromedio() const {
    if (tiempoTotal_ <= 0.0) {
        return 0.0f;
    }
    return static_cast<float>(framesTotales_ / tiempoTotal_);
}

// ---------------------------------------------------------------------------
// Fuente de pixeles de 5 filas (solo los caracteres que usa el HUD).
// Casi todos los glifos miden 3 columnas; la N necesita 4 para leerse bien.
// ---------------------------------------------------------------------------

namespace {

constexpr int kAltoGlifo = 5;
constexpr int kEscalaTexto = 3;   // cada "pixel" del glifo mide 3x3
constexpr int kEspacioGlifo = 1;  // columnas vacias entre letras
constexpr int kMargen = 8;        // margen del recuadro del HUD

struct Glifo {
    char caracter;
    const char* filas[kAltoGlifo];
};

const Glifo kFuente[] = {
    {'0', {"###", "#.#", "#.#", "#.#", "###"}},
    {'1', {".#.", "##.", ".#.", ".#.", "###"}},
    {'2', {"###", "..#", "###", "#..", "###"}},
    {'3', {"###", "..#", "###", "..#", "###"}},
    {'4', {"#.#", "#.#", "###", "..#", "..#"}},
    {'5', {"###", "#..", "###", "..#", "###"}},
    {'6', {"###", "#..", "###", "#.#", "###"}},
    {'7', {"###", "..#", "..#", "..#", "..#"}},
    {'8', {"###", "#.#", "###", "#.#", "###"}},
    {'9', {"###", "#.#", "###", "..#", "###"}},
    {'F', {"###", "#..", "##.", "#..", "#.."}},
    {'P', {"###", "#.#", "###", "#..", "#.."}},
    {'S', {"###", "#..", "###", "..#", "###"}},
    {'N', {"#..#", "##.#", "#.##", "#..#", "#..#"}},
    {':', {"...", ".#.", "...", ".#.", "..."}},
    {'.', {"...", "...", "...", "...", ".#."}},
};

// Ancho (en columnas) de un glifo: el largo de sus filas.
int anchoGlifo(const Glifo* glifo) {
    return (glifo != nullptr) ? static_cast<int>(std::strlen(glifo->filas[0])) : 3;
}

const Glifo* buscarGlifo(char c) {
    for (const Glifo& g : kFuente) {
        if (g.caracter == c) {
            return &g;
        }
    }
    return nullptr;  // espacio o caracter no soportado: se deja en blanco
}

// Ancho en pixeles de pantalla que ocupa un texto.
int anchoTexto(const char* texto) {
    int columnas = 0;
    for (const char* p = texto; *p != '\0'; ++p) {
        columnas += anchoGlifo(buscarGlifo(*p)) + kEspacioGlifo;
    }
    return columnas * kEscalaTexto;
}

// Dibuja texto con el color actual del renderer, desde (x, y).
void dibujarTexto(SDL_Renderer* renderer, const char* texto, int x, int y) {
    for (const char* p = texto; *p != '\0'; ++p) {
        const Glifo* glifo = buscarGlifo(*p);
        const int ancho = anchoGlifo(glifo);
        if (glifo != nullptr) {
            for (int fila = 0; fila < kAltoGlifo; ++fila) {
                for (int col = 0; col < ancho; ++col) {
                    if (glifo->filas[fila][col] == '#') {
                        const SDL_Rect punto{
                            x + col * kEscalaTexto,
                            y + fila * kEscalaTexto,
                            kEscalaTexto,
                            kEscalaTexto,
                        };
                        SDL_RenderFillRect(renderer, &punto);
                    }
                }
            }
        }
        x += (ancho + kEspacioGlifo) * kEscalaTexto;
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// HUD
// ---------------------------------------------------------------------------

void dibujarHud(SDL_Renderer* renderer, float fps, int n) {
    char lineaFps[32];
    char lineaN[32];
    std::snprintf(lineaFps, sizeof(lineaFps), "FPS: %.1f", fps);
    std::snprintf(lineaN, sizeof(lineaN), "N: %d", n);

    const int altoLinea = kAltoGlifo * kEscalaTexto;
    const int separacion = kEscalaTexto * 2;
    const int anchoMax = SDL_max(anchoTexto(lineaFps), anchoTexto(lineaN));

    // Recuadro semitransparente detras del texto.
    const SDL_Rect fondo{
        kMargen,
        kMargen,
        anchoMax + kMargen * 2,
        altoLinea * 2 + separacion + kMargen * 2,
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &fondo);

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    const int xTexto = fondo.x + kMargen;
    const int yTexto = fondo.y + kMargen;
    dibujarTexto(renderer, lineaFps, xTexto, yTexto);
    dibujarTexto(renderer, lineaN, xTexto, yTexto + altoLinea + separacion);
}
