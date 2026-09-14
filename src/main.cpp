// Screensaver de Chuchos - Proyecto 1, Computacion Paralela y Distribuida.
//
// Uso:  ./screensaver [N] [semilla] [--zona P] [--hilos H]
//   N        cantidad de chuchos (entero > 0, por defecto 50)
//   semilla  semilla del generador aleatorio (por defecto, la hora actual)
//   --zona   porcentaje de la pantalla (franja derecha) que se procesa con
//            OpenMP: 0 = todo secuencial, 100 = todo paralelo (por defecto 50)
//   --hilos  cantidad de hilos de OpenMP (por defecto, los que decida OpenMP)
//
// Teclas: flechas izquierda/derecha agrandan/achican la zona paralela;
//         Esc cierra.
//
// Ciclo principal: eventos -> simulacion -> dibujo -> presentar.

#include <SDL.h>
#include <omp.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "argumentos.h"
#include "chucho.h"
#include "config.h"
#include "estadisticas.h"
#include "hud.h"
#include "render.h"
#include "simulacion.h"
#include "sprite.h"
#include "zona.h"

namespace {

struct Opciones {
    int n = config::kNPorDefecto;
    unsigned int semilla = 0;
    int zona = config::kZonaPorDefecto;
    int hilos = 0;  // 0 = no cambiar lo que decida OpenMP
};

bool parsearArgumentos(int argc, char* argv[], Opciones& op) {
    op.semilla = static_cast<unsigned int>(std::time(nullptr));

    int posicionales = 0;
    for (int i = 1; i < argc; ++i) {
        long valor = 0;
        // Las opciones con nombre necesitan un valor en el siguiente argumento.
        const bool tieneValor = i + 1 < argc;

        if (std::strcmp(argv[i], "--zona") == 0) {
            if (!tieneValor || !parsearEntero(argv[++i], 0, 100, valor)) {
                return false;
            }
            op.zona = static_cast<int>(valor);
        } else if (std::strcmp(argv[i], "--hilos") == 0) {
            if (!tieneValor || !parsearEntero(argv[++i], 1, 1024, valor)) {
                return false;
            }
            op.hilos = static_cast<int>(valor);
        } else if (posicionales == 0) {
            if (!parsearEntero(argv[i], 1, 1000000, valor)) {
                return false;
            }
            op.n = static_cast<int>(valor);
            ++posicionales;
        } else if (posicionales == 1) {
            if (!parsearEntero(argv[i], 1, 4294967295L, valor)) {
                return false;
            }
            op.semilla = static_cast<unsigned int>(valor);
            ++posicionales;
        } else {
            return false;
        }
    }
    return true;
}

// Devuelve true mientras el usuario no haya cerrado la ventana o
// presionado Escape. Las flechas cambian el tamano de la zona paralela.
bool procesarEventos(ZonaParalela& zona) {
    SDL_Event evento;
    while (SDL_PollEvent(&evento)) {
        if (evento.type == SDL_QUIT) {
            return false;
        }
        if (evento.type == SDL_KEYDOWN) {
            switch (evento.key.keysym.sym) {
                case SDLK_ESCAPE: return false;
                case SDLK_LEFT:   zona.ajustar(config::kPasoZona); break;
                case SDLK_RIGHT:  zona.ajustar(-config::kPasoZona); break;
                default: break;
            }
        }
    }
    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    Opciones op;
    if (!parsearArgumentos(argc, argv, op)) {
        std::fprintf(stderr, "Uso: %s [N] [semilla] [--zona P] [--hilos H]\n", argv[0]);
        std::fprintf(stderr, "  N y semilla: enteros positivos\n");
        std::fprintf(stderr, "  --zona: porcentaje de 0 a 100 procesado con OpenMP\n");
        std::fprintf(stderr, "  --hilos: cantidad de hilos de OpenMP\n");
        return EXIT_FAILURE;
    }

    if (op.hilos > 0) {
        omp_set_num_threads(op.hilos);
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

    Simulacion simulacion(crearChuchos(op.n, op.semilla));
    ZonaParalela zona(op.zona);
    Estadisticas estadisticas;
    const int hilos = omp_get_max_threads();

    // El renderer importa al medir: si es por software, dibujar compite por
    // los mismos nucleos que los hilos de OpenMP.
    SDL_RendererInfo infoRenderer;
    const char* nombreRenderer =
        (SDL_GetRendererInfo(renderer, &infoRenderer) == 0) ? infoRenderer.name : "desconocido";

    // OMP_WAIT_POLICY solo se puede fijar desde afuera (variable de entorno);
    // ver el Makefile para saber por que importa.
    const char* politicaEspera = std::getenv("OMP_WAIT_POLICY");

    std::printf("N = %d, semilla = %u, zona = %d%%, hilos = %d, renderer = %s, "
                "OMP_WAIT_POLICY = %s\n",
                op.n, op.semilla, zona.porcentaje(), hilos, nombreRenderer,
                politicaEspera != nullptr ? politicaEspera : "(por defecto)");

    const double frecuencia = static_cast<double>(SDL_GetPerformanceFrequency());
    Uint64 tiempoAnterior = SDL_GetPerformanceCounter();

    while (procesarEventos(zona)) {
        const Uint64 tiempoActual = SDL_GetPerformanceCounter();
        const float dtReal = static_cast<float>((tiempoActual - tiempoAnterior) / frecuencia);
        tiempoAnterior = tiempoActual;

        const TiemposFrame tiempos = simulacion.paso(SDL_min(dtReal, config::kDtMaximo), zona);
        estadisticas.registrarFrame(dtReal, tiempos);

        dibujarFondo(renderer, zona);
        dibujarChuchos(renderer, texturaChucho, simulacion.chuchos());
        dibujarEtiquetasZonas(renderer, zona, simulacion.chuchosSecuenciales(),
                              simulacion.chuchosParalelos());
        dibujarHud(renderer, DatosHud{estadisticas.fpsActual(), op.n, hilos,
                                      zona.porcentaje(), estadisticas.tiemposActuales()});
        SDL_RenderPresent(renderer);
    }

    const TiemposFrame promedio = estadisticas.tiemposPromedio();
    std::printf("FPS promedio: %.2f\n", estadisticas.fpsPromedio());
    std::printf("ms por frame -> secuencial: %.3f, openmp: %.3f\n",
                promedio.msSecuencial, promedio.msParalelo);

    SDL_DestroyTexture(texturaChucho);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(ventana);
    SDL_Quit();
    return EXIT_SUCCESS;
}
