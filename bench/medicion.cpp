#include "medicion.h"

#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>

#include "colisiones.h"
#include "simulacion.h"

namespace {

// dt fijo: asi cada frame hace el mismo trabajo sin importar lo que tarde,
// y el estado final se puede comparar entre versiones.
constexpr float kDt = 1.0f / 60.0f;

// Avanza un frame y suma a "tiempos" lo que tardo cada etapa.
void pasoMedido(std::vector<Chucho>& chuchos, const std::vector<int>& todos,
                ManejadorColisiones& colisiones, Version version, TiemposEtapas& tiempos) {
    const bool paralelo = (version == Version::OpenMP);

    const double t0 = omp_get_wtime();
    moverChuchos(chuchos, todos, kDt, paralelo);

    const double t1 = omp_get_wtime();
    colisiones.preparar(static_cast<int>(chuchos.size()));
    if (paralelo) {
        colisiones.detectar(chuchos, todos, true);
    } else {
        colisiones.detectarTriangulo(chuchos);
    }

    const double t2 = omp_get_wtime();
    colisiones.aplicar(chuchos, todos, paralelo);

    const double t3 = omp_get_wtime();
    tiempos.mover += (t1 - t0) * 1000.0;
    tiempos.detectar += (t2 - t1) * 1000.0;
    tiempos.aplicar += (t3 - t2) * 1000.0;
}

}  // namespace

const char* nombreVersion(Version version) {
    return version == Version::Secuencial ? "secuencial" : "openmp";
}

ResultadoMedicion medir(const ConfigMedicion& config) {
    if (config.version == Version::OpenMP) {
        omp_set_num_threads(config.hilos);
    }

    ResultadoMedicion resultado;
    resultado.config = config;

    std::vector<int> todos(config.n);
    std::iota(todos.begin(), todos.end(), 0);

    for (int rep = 0; rep < config.repeticiones; ++rep) {
        std::vector<Chucho> chuchos = crearChuchos(config.n, config.semilla);
        ManejadorColisiones colisiones;

        // Calentamiento: despierta a los hilos, llena caches y deja que el
        // CPU suba de frecuencia antes de medir.
        TiemposEtapas descartado;
        for (int f = 0; f < config.framesCalentamiento; ++f) {
            pasoMedido(chuchos, todos, colisiones, config.version, descartado);
        }

        TiemposEtapas suma;
        for (int f = 0; f < config.frames; ++f) {
            pasoMedido(chuchos, todos, colisiones, config.version, suma);
        }

        resultado.porRepeticion.push_back(TiemposEtapas{
            suma.mover / config.frames,
            suma.detectar / config.frames,
            suma.aplicar / config.frames,
        });
        resultado.estadoFinal = chuchos;
    }
    return resultado;
}

double ResultadoMedicion::promedio() const {
    double suma = 0.0;
    for (const TiemposEtapas& t : porRepeticion) {
        suma += t.total();
    }
    return suma / porRepeticion.size();
}

double ResultadoMedicion::desviacion() const {
    if (porRepeticion.size() < 2) {
        return 0.0;
    }
    const double media = promedio();
    double suma = 0.0;
    for (const TiemposEtapas& t : porRepeticion) {
        suma += (t.total() - media) * (t.total() - media);
    }
    return std::sqrt(suma / (porRepeticion.size() - 1));
}

double ResultadoMedicion::minimo() const {
    double menor = porRepeticion.front().total();
    for (const TiemposEtapas& t : porRepeticion) {
        menor = std::min(menor, t.total());
    }
    return menor;
}

TiemposEtapas ResultadoMedicion::promedioEtapas() const {
    TiemposEtapas suma;
    for (const TiemposEtapas& t : porRepeticion) {
        suma.mover += t.mover;
        suma.detectar += t.detectar;
        suma.aplicar += t.aplicar;
    }
    const double k = static_cast<double>(porRepeticion.size());
    return TiemposEtapas{suma.mover / k, suma.detectar / k, suma.aplicar / k};
}

bool mismosEstados(const std::vector<Chucho>& a, const std::vector<Chucho>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    // Se compara campo por campo (no el struct entero) para no depender del
    // relleno que el compilador pueda meter entre campos.
    for (std::size_t i = 0; i < a.size(); ++i) {
        const Chucho& p = a[i];
        const Chucho& q = b[i];
        const bool iguales =
            std::memcmp(&p.x, &q.x, sizeof(float)) == 0 &&
            std::memcmp(&p.y, &q.y, sizeof(float)) == 0 &&
            std::memcmp(&p.vx, &q.vx, sizeof(float)) == 0 &&
            std::memcmp(&p.vy, &q.vy, sizeof(float)) == 0 &&
            std::memcmp(&p.tono, &q.tono, sizeof(float)) == 0 &&
            std::memcmp(&p.pausaColor, &q.pausaColor, sizeof(float)) == 0 &&
            p.tinte.r == q.tinte.r && p.tinte.g == q.tinte.g && p.tinte.b == q.tinte.b;
        if (!iguales) {
            return false;
        }
    }
    return true;
}
