#include "reporte.h"

#include <cstdio>
#include <filesystem>

namespace {

// Busca la medicion de un N y una version (y cantidad de hilos, si es
// OpenMP). Devuelve nullptr si no esta.
const ResultadoMedicion* buscar(const std::vector<ResultadoMedicion>& resultados, int n,
                                Version version, int hilos) {
    for (const ResultadoMedicion& r : resultados) {
        const bool mismosHilos = (version == Version::Secuencial) || r.config.hilos == hilos;
        if (r.config.n == n && r.config.version == version && mismosHilos) {
            return &r;
        }
    }
    return nullptr;
}

}  // namespace

std::vector<FilaReporte> armarReporte(const std::vector<ResultadoMedicion>& resultados) {
    std::vector<FilaReporte> filas;

    for (const ResultadoMedicion& r : resultados) {
        FilaReporte fila;
        fila.resultado = r;

        const ResultadoMedicion* secuencial = buscar(resultados, r.config.n, Version::Secuencial, 0);
        if (secuencial != nullptr) {
            fila.speedupVsSecuencial = secuencial->promedio() / r.promedio();
            fila.identico = mismosEstados(secuencial->estadoFinal, r.estadoFinal);
        }

        if (r.config.version == Version::OpenMP) {
            const ResultadoMedicion* unHilo = buscar(resultados, r.config.n, Version::OpenMP, 1);
            if (unHilo != nullptr) {
                fila.speedupVs1Hilo = unHilo->promedio() / r.promedio();
            }
            fila.eficiencia = fila.speedupVsSecuencial / r.config.hilos;
        }

        filas.push_back(fila);
    }
    return filas;
}

void imprimirTabla(const std::vector<FilaReporte>& filas) {
    std::printf("\n%6s  %-10s %5s | %9s %8s %9s | %8s %9s %8s | %8s %8s %6s  %s\n",
                "N", "version", "hilos", "ms/frame", "desv", "min", "mover", "detectar",
                "aplicar", "S(sec)", "S(1h)", "efic", "identico");
    std::printf("%s\n", std::string(124, '-').c_str());

    int nAnterior = -1;
    for (const FilaReporte& f : filas) {
        const ResultadoMedicion& r = f.resultado;
        const TiemposEtapas etapas = r.promedioEtapas();

        if (nAnterior != -1 && r.config.n != nAnterior) {
            std::printf("\n");
        }
        nAnterior = r.config.n;

        char hilos[8];
        char speedup1h[16];
        if (r.config.version == Version::Secuencial) {
            std::snprintf(hilos, sizeof(hilos), "-");
            std::snprintf(speedup1h, sizeof(speedup1h), "-");
        } else {
            std::snprintf(hilos, sizeof(hilos), "%d", r.config.hilos);
            std::snprintf(speedup1h, sizeof(speedup1h), "%.2fx", f.speedupVs1Hilo);
        }

        std::printf("%6d  %-10s %5s | %9.3f %8.3f %9.3f | %8.3f %9.3f %8.3f | %7.2fx %8s %6.2f  %s\n",
                    r.config.n, nombreVersion(r.config.version), hilos, r.promedio(),
                    r.desviacion(), r.minimo(), etapas.mover, etapas.detectar, etapas.aplicar,
                    f.speedupVsSecuencial, speedup1h, f.eficiencia, f.identico ? "si" : "NO");
    }
    std::printf("\n");
}

bool escribirCsv(const std::string& ruta, const std::vector<FilaReporte>& filas) {
    const std::filesystem::path carpeta = std::filesystem::path(ruta).parent_path();
    if (!carpeta.empty()) {
        std::error_code error;
        std::filesystem::create_directories(carpeta, error);
    }

    std::FILE* archivo = std::fopen(ruta.c_str(), "w");
    if (archivo == nullptr) {
        return false;
    }

    std::fprintf(archivo,
                 "n,version,hilos,frames,repeticiones,ms_promedio,ms_desviacion,ms_minimo,"
                 "ms_mover,ms_detectar,ms_aplicar,speedup_vs_secuencial,speedup_vs_1hilo,"
                 "eficiencia,resultado_identico\n");

    for (const FilaReporte& f : filas) {
        const ResultadoMedicion& r = f.resultado;
        const TiemposEtapas etapas = r.promedioEtapas();
        const bool esOpenMP = (r.config.version == Version::OpenMP);

        // En la fila secuencial "hilos" y "speedup_vs_1hilo" quedan vacios.
        char hilos[16] = "";
        char speedup1h[32] = "";
        if (esOpenMP) {
            std::snprintf(hilos, sizeof(hilos), "%d", r.config.hilos);
            std::snprintf(speedup1h, sizeof(speedup1h), "%.4f", f.speedupVs1Hilo);
        }

        std::fprintf(archivo, "%d,%s,%s,%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%s,%.4f,%s\n",
                     r.config.n, nombreVersion(r.config.version), hilos, r.config.frames,
                     r.config.repeticiones, r.promedio(), r.desviacion(), r.minimo(),
                     etapas.mover, etapas.detectar, etapas.aplicar, f.speedupVsSecuencial,
                     speedup1h, f.eficiencia, f.identico ? "si" : "no");
    }

    std::fclose(archivo);
    return true;
}

bool escribirCsvDetalle(const std::string& ruta, const std::vector<FilaReporte>& filas) {
    std::FILE* archivo = std::fopen(ruta.c_str(), "w");
    if (archivo == nullptr) {
        return false;
    }

    std::fprintf(archivo, "n,version,hilos,repeticion,ms_total,ms_mover,ms_detectar,ms_aplicar\n");
    for (const FilaReporte& f : filas) {
        const ConfigMedicion& c = f.resultado.config;
        char hilos[16] = "";
        if (c.version == Version::OpenMP) {
            std::snprintf(hilos, sizeof(hilos), "%d", c.hilos);
        }

        int repeticion = 1;
        for (const TiemposEtapas& t : f.resultado.porRepeticion) {
            std::fprintf(archivo, "%d,%s,%s,%d,%.4f,%.4f,%.4f,%.4f\n", c.n,
                         nombreVersion(c.version), hilos, repeticion, t.total(), t.mover,
                         t.detectar, t.aplicar);
            ++repeticion;
        }
    }

    std::fclose(archivo);
    return true;
}
