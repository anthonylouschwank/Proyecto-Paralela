#include "texto.h"

#include <cstring>

namespace {

constexpr int kAltoGlifo = 5;
constexpr int kEspacioGlifo = 1;  // columnas vacias entre letras
constexpr int kAnchoEspacio = 3;  // ancho de ' ' y de caracteres sin glifo

// Cada glifo son 5 filas; '#' es un punto encendido. Casi todos miden 3
// columnas, pero algunos (M, N, W) necesitan mas para leerse bien.
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
    {'A', {".#.", "#.#", "###", "#.#", "#.#"}},
    {'B', {"##.", "#.#", "##.", "#.#", "##."}},
    {'C', {"###", "#..", "#..", "#..", "###"}},
    {'D', {"##.", "#.#", "#.#", "#.#", "##."}},
    {'E', {"###", "#..", "##.", "#..", "###"}},
    {'F', {"###", "#..", "##.", "#..", "#.."}},
    {'G', {"###", "#..", "#.#", "#.#", "###"}},
    {'H', {"#.#", "#.#", "###", "#.#", "#.#"}},
    {'I', {"###", ".#.", ".#.", ".#.", "###"}},
    {'J', {"..#", "..#", "..#", "#.#", "###"}},
    {'K', {"#.#", "#.#", "##.", "#.#", "#.#"}},
    {'L', {"#..", "#..", "#..", "#..", "###"}},
    {'M', {"#...#", "##.##", "#.#.#", "#...#", "#...#"}},
    {'N', {"#..#", "##.#", "#.##", "#..#", "#..#"}},
    {'O', {"###", "#.#", "#.#", "#.#", "###"}},
    {'P', {"###", "#.#", "###", "#..", "#.."}},
    {'Q', {"###", "#.#", "#.#", "###", "..#"}},
    {'R', {"##.", "#.#", "##.", "#.#", "#.#"}},
    {'S', {"###", "#..", "###", "..#", "###"}},
    {'T', {"###", ".#.", ".#.", ".#.", ".#."}},
    {'U', {"#.#", "#.#", "#.#", "#.#", "###"}},
    {'V', {"#.#", "#.#", "#.#", "#.#", ".#."}},
    {'W', {"#...#", "#...#", "#.#.#", "##.##", "#...#"}},
    {'X', {"#.#", "#.#", ".#.", "#.#", "#.#"}},
    {'Y', {"#.#", "#.#", ".#.", ".#.", ".#."}},
    {'Z', {"###", "..#", ".#.", "#..", "###"}},
    {':', {"...", ".#.", "...", ".#.", "..."}},
    {'.', {"...", "...", "...", "...", ".#."}},
    {'%', {"#.#", "..#", ".#.", "#..", "#.#"}},
    {'-', {"...", "...", "###", "...", "..."}},
    {'(', {".#", "#.", "#.", "#.", ".#"}},
    {')', {"#.", ".#", ".#", ".#", "#."}},
};

const Glifo* buscarGlifo(char c) {
    for (const Glifo& g : kFuente) {
        if (g.caracter == c) {
            return &g;
        }
    }
    return nullptr;
}

// Ancho (en columnas) de un glifo: el largo de sus filas.
int anchoGlifo(const Glifo* glifo) {
    return (glifo != nullptr) ? static_cast<int>(std::strlen(glifo->filas[0])) : kAnchoEspacio;
}

}  // namespace

int altoTexto(int escala) {
    return kAltoGlifo * escala;
}

int anchoTexto(const char* texto, int escala) {
    int columnas = 0;
    for (const char* p = texto; *p != '\0'; ++p) {
        columnas += anchoGlifo(buscarGlifo(*p)) + kEspacioGlifo;
    }
    return columnas * escala;
}

void dibujarTexto(SDL_Renderer* renderer, const char* texto, int x, int y, int escala) {
    for (const char* p = texto; *p != '\0'; ++p) {
        const Glifo* glifo = buscarGlifo(*p);
        const int ancho = anchoGlifo(glifo);
        if (glifo != nullptr) {
            for (int fila = 0; fila < kAltoGlifo; ++fila) {
                for (int col = 0; col < ancho; ++col) {
                    if (glifo->filas[fila][col] == '#') {
                        const SDL_Rect punto{x + col * escala, y + fila * escala, escala, escala};
                        SDL_RenderFillRect(renderer, &punto);
                    }
                }
            }
        }
        x += (ancho + kEspacioGlifo) * escala;
    }
}
