// Benchmark del screensaver de chuchos: mide la simulacion SIN ventana para
// comparar la version secuencial contra OpenMP con distintos hilos.
//
// Uso:  ./benchmark [opciones]
//   --n 500,1000,2000,4000   cantidades de chuchos a probar
//   --hilos 1,2,4,8          hilos de OpenMP a probar (por defecto potencias
//                            de 2 hasta los nucleos disponibles)
//   --frames 50              frames medidos por repeticion
//   --calentamiento 5        frames que se corren antes de medir
//   --reps 10                repeticiones de cada configuracion
//   --semilla 7              semilla de la escena
//   --salida resultados/benchmark.csv
//
// Por cada N se mide:
//   1. La version secuencial de referencia (triangulo i < j, Parte 2).
//   2. La version OpenMP (recorrido completo, Parte 3) con cada cantidad de hilos.
// y se reporta ms por frame (por etapa), speedup, eficiencia y si el
// resultado fue identico al secuencial.

#include <omp.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "argumentos.h"
#include "medicion.h"
#include "reporte.h"

namespace {

struct OpcionesBenchmark {
    std::vector<int> ns{500, 1000, 2000, 4000};
    std::vector<int> hilos;  // vacio = elegir segun los nucleos
    int frames = 50;
    int calentamiento = 5;
    int repeticiones = 10;  // la rubrica pide minimo 10 mediciones por prueba
    unsigned int semilla = 7;
    std::string salida = "resultados/benchmark.csv";
};

// 1, 2, 4, ... hasta los nucleos disponibles (y los nucleos, si no es
// potencia de 2).
std::vector<int> hilosPorDefecto() {
    const int nucleos = omp_get_num_procs();
    std::vector<int> hilos;
    for (int h = 1; h <= nucleos; h *= 2) {
        hilos.push_back(h);
    }
    if (hilos.back() != nucleos) {
        hilos.push_back(nucleos);
    }
    return hilos;
}

bool parsearArgumentos(int argc, char* argv[], OpcionesBenchmark& op) {
    for (int i = 1; i < argc; ++i) {
        // Todas las opciones llevan un valor en el siguiente argumento.
        if (i + 1 >= argc) {
            return false;
        }
        const char* opcion = argv[i];
        const char* valor = argv[++i];
        long numero = 0;

        if (std::strcmp(opcion, "--n") == 0) {
            if (!parsearListaEnteros(valor, 2, 100000, op.ns)) return false;
        } else if (std::strcmp(opcion, "--hilos") == 0) {
            if (!parsearListaEnteros(valor, 1, 1024, op.hilos)) return false;
        } else if (std::strcmp(opcion, "--frames") == 0) {
            if (!parsearEntero(valor, 1, 100000, numero)) return false;
            op.frames = static_cast<int>(numero);
        } else if (std::strcmp(opcion, "--calentamiento") == 0) {
            if (!parsearEntero(valor, 0, 100000, numero)) return false;
            op.calentamiento = static_cast<int>(numero);
        } else if (std::strcmp(opcion, "--reps") == 0) {
            if (!parsearEntero(valor, 1, 1000, numero)) return false;
            op.repeticiones = static_cast<int>(numero);
        } else if (std::strcmp(opcion, "--semilla") == 0) {
            if (!parsearEntero(valor, 1, 4294967295L, numero)) return false;
            op.semilla = static_cast<unsigned int>(numero);
        } else if (std::strcmp(opcion, "--salida") == 0) {
            op.salida = valor;
        } else {
            return false;
        }
    }
    if (op.hilos.empty()) {
        op.hilos = hilosPorDefecto();
    }
    return true;
}

void imprimirUso(const char* programa) {
    std::fprintf(stderr,
                 "Uso: %s [--n 500,1000] [--hilos 1,2,4,8] [--frames 50] [--calentamiento 5]\n"
                 "          [--reps 10] [--semilla 7] [--salida resultados/benchmark.csv]\n",
                 programa);
}

const char* variableEntorno(const char* nombre) {
    const char* valor = std::getenv(nombre);
    return valor != nullptr ? valor : "(sin definir)";
}

}  // namespace

int main(int argc, char* argv[]) {
    OpcionesBenchmark op;
    if (!parsearArgumentos(argc, argv, op)) {
        imprimirUso(argv[0]);
        return EXIT_FAILURE;
    }

    // Datos del entorno, para anotarlos en el reporte.
    std::printf("Nucleos disponibles: %d\n", omp_get_num_procs());
    std::printf("OMP_WAIT_POLICY: %s, OMP_PROC_BIND: %s\n",
                variableEntorno("OMP_WAIT_POLICY"), variableEntorno("OMP_PROC_BIND"));
    std::printf("Frames: %d (+%d de calentamiento), repeticiones: %d, semilla: %u\n",
                op.frames, op.calentamiento, op.repeticiones, op.semilla);
    std::fflush(stdout);  // que salga antes que los mensajes de progreso

    std::vector<ResultadoMedicion> resultados;
    for (int n : op.ns) {
        ConfigMedicion config{n, Version::Secuencial, 1, op.frames, op.calentamiento,
                              op.repeticiones, op.semilla};

        std::fprintf(stderr, "N=%d: secuencial...\n", n);
        resultados.push_back(medir(config));

        config.version = Version::OpenMP;
        for (int h : op.hilos) {
            std::fprintf(stderr, "N=%d: openmp con %d hilos...\n", n, h);
            config.hilos = h;
            resultados.push_back(medir(config));
        }
    }

    const std::vector<FilaReporte> filas = armarReporte(resultados);
    imprimirTabla(filas);

    std::printf("S(sec) = speedup contra la version secuencial (triangulo i<j)\n");
    std::printf("S(1h)  = speedup contra OpenMP con 1 hilo\n");
    std::printf("efic   = S(sec) / hilos\n\n");

    if (!escribirCsv(op.salida, filas)) {
        std::fprintf(stderr, "No se pudo escribir %s\n", op.salida.c_str());
        return EXIT_FAILURE;
    }
    std::printf("Resultados guardados en %s\n", op.salida.c_str());

    // Cada medicion individual va a un segundo CSV: "x.csv" -> "x_detalle.csv"
    std::string rutaDetalle = op.salida;
    const std::size_t punto = rutaDetalle.rfind(".csv");
    rutaDetalle.insert(punto == std::string::npos ? rutaDetalle.size() : punto, "_detalle");
    if (!escribirCsvDetalle(rutaDetalle, filas)) {
        std::fprintf(stderr, "No se pudo escribir %s\n", rutaDetalle.c_str());
        return EXIT_FAILURE;
    }
    std::printf("Mediciones individuales guardadas en %s\n", rutaDetalle.c_str());

    for (const FilaReporte& f : filas) {
        if (!f.identico) {
            std::fprintf(stderr, "ADVERTENCIA: hay resultados distintos al secuencial.\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
