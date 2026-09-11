// Screensaver de Chuchos - Proyecto 1, Computacion Paralela y Distribuida.
//
// Uso:  ./screensaver [N] [semilla]
//   N        cantidad de chuchos (entero > 0, por defecto 50)
//   semilla  semilla del generador aleatorio (por defecto, la hora actual)
//
// Ciclo principal: eventos -> simulacion -> dibujo -> presentar.

#include <SDL.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "chucho.h"
#include "colisiones.h"
#include "config.h"
#include "hud.h"
#include "render.h"
#include "simulacion.h"
#include "sprite.h"

namespace {

struct Opciones {
    int n = config::kNPorDefecto;
    unsigned int semilla = 0;
};

// Convierte texto a entero positivo. Devuelve false si no es valido.
bool parsearEnteroPositivo(const char* texto, long& salida) {
    char* fin = nullptr;
    errno = 0;
    const long valor = std::strtol(texto, &fin, 10);
    if (errno != 0 || fin == texto || *fin != '\0' || valor <= 0) {
        return false;
    }
    salida = valor;
    return true;
}

bool parsearArgumentos(int argc, char* argv[], Opciones& op) {
    op.semilla = static_cast<unsigned int>(std::time(nullptr));

    if (argc > 3) {
        return false;
    }
    long valor = 0;
    if (argc >= 2) {
        if (!parsearEnteroPositivo(argv[1], valor)) {
            return false;
        }
        op.n = static_cast<int>(valor);
    }
    if (argc == 3) {
        if (!parsearEnteroPositivo(argv[2], valor)) {
            return false;
        }
        op.semilla = static_cast<unsigned int>(valor);
    }
    return true;
}

// Devuelve true mientras el usuario no haya cerrado la ventana o
// presionado Escape.
bool procesarEventos() {
    SDL_Event evento;
    while (SDL_PollEvent(&evento)) {
        if (evento.type == SDL_QUIT) {
            return false;
        }
        if (evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE) {
            return false;
        }
    }
    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    Opciones op;
    if (!parsearArgumentos(argc, argv, op)) {
        std::fprintf(stderr, "Uso: %s [N] [semilla]\n", argv[0]);
        std::fprintf(stderr, "  N y semilla deben ser enteros positivos.\n");
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::fprintf(stderr, "Error en SDL_Init: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* ventana = SDL_CreateWindow(
        config::kTituloVentana, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        config::kAnchoVentana, config::kAltoVentana, SDL_WINDOW_SHOWN);
    if (ventana == nullptr) {
        std::fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Sin VSYNC a proposito: queremos medir cuantos FPS puede sacar el
    // programa, no quedar topados a la tasa de refresco del monitor.
    SDL_Renderer* renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer(ventana, -1, SDL_RENDERER_SOFTWARE);
    }
    if (renderer == nullptr) {
        std::fprintf(stderr, "Error al crear el renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture* texturaChucho = crearTexturaChucho(renderer);
    if (texturaChucho == nullptr) {
        std::fprintf(stderr, "Error al crear el sprite: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    std::vector<Chucho> chuchos = crearChuchos(op.n, op.semilla);
    ManejadorColisiones colisiones;
    ContadorFps contador;

    std::printf("N = %d, semilla = %u\n", op.n, op.semilla);

    const double frecuencia = static_cast<double>(SDL_GetPerformanceFrequency());
    Uint64 tiempoAnterior = SDL_GetPerformanceCounter();

    while (procesarEventos()) {
        const Uint64 tiempoActual = SDL_GetPerformanceCounter();
        const float dtReal = static_cast<float>((tiempoActual - tiempoAnterior) / frecuencia);
        tiempoAnterior = tiempoActual;

        contador.registrarFrame(dtReal);

        // Simulacion: primero mover y rebotar en bordes, luego resolver
        // choques con las posiciones ya actualizadas.
        actualizarChuchos(chuchos, SDL_min(dtReal, config::kDtMaximo));
        colisiones.resolver(chuchos);

        dibujarEscena(renderer, texturaChucho, chuchos);
        dibujarHud(renderer, contador.fpsActual(), op.n);
        SDL_RenderPresent(renderer);
    }

    std::printf("FPS promedio: %.2f\n", contador.fpsPromedio());

    SDL_DestroyTexture(texturaChucho);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return EXIT_SUCCESS;
}
