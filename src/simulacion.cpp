#include "simulacion.h"

#include <omp.h>

#include <cmath>
#include <utility>

#include "config.h"

namespace {

// Si la posicion salio del rango [min, max], la regresa al borde (espejando
// lo que se paso) y hace que la velocidad apunte hacia adentro. Usar
// fabs en vez de solo invertir el signo evita que el chucho se quede
// "pegado" vibrando en el borde.
void rebotarEje(float& pos, float& vel, float min, float max) {
    if (pos < min) {
        pos = min + (min - pos);
        vel = std::fabs(vel);
    } else if (pos > max) {
        pos = max - (pos - max);
        vel = -std::fabs(vel);
    }
}

// Ejecuta f y devuelve cuantos milisegundos tardo.
template <typename Funcion>
double medirMs(Funcion&& f) {
    const double inicio = omp_get_wtime();
    f();
    return (omp_get_wtime() - inicio) * 1000.0;
}

}  // namespace

void moverChuchos(std::vector<Chucho>& chuchos, const std::vector<int>& indices,
                  float dt, bool paralelo) {
    // Sin chuchos no vale la pena despertar a los hilos (cuesta tiempo aun
    // con 0 iteraciones).
    if (indices.empty()) {
        return;
    }
    const int m = static_cast<int>(indices.size());

    // Todas las iteraciones cuestan lo mismo: schedule(static) reparte
    // bloques iguales sin costo extra de coordinacion.
    #pragma omp parallel for schedule(static) if (paralelo)
    for (int k = 0; k < m; ++k) {
        Chucho& c = chuchos[indices[k]];

        c.x += c.vx * dt;
        c.y += c.vy * dt;

        rebotarEje(c.x, c.vx, config::kMinX, config::kMaxX);
        rebotarEje(c.y, c.vy, config::kMinY, config::kMaxY);

        c.pausaColor = std::fmax(0.0f, c.pausaColor - dt);
    }
}

Simulacion::Simulacion(std::vector<Chucho> chuchos) : chuchos_(std::move(chuchos)) {}

void Simulacion::clasificar(const ZonaParalela& zona) {
    indicesSecuencial_.clear();
    indicesParalelo_.clear();

    const int n = static_cast<int>(chuchos_.size());
    for (int i = 0; i < n; ++i) {
        if (zona.contiene(chuchos_[i])) {
            indicesParalelo_.push_back(i);
        } else {
            indicesSecuencial_.push_back(i);
        }
    }
}

// Cada etapa se hace primero para la zona secuencial y luego para la
// paralela. Las etapas no se pueden mezclar: los choques deben detectarse
// con TODAS las posiciones ya movidas, y aplicarse cuando TODOS terminaron
// de detectar (porque aplicar cambia las velocidades que detectar lee).
TiemposFrame Simulacion::paso(float dt, const ZonaParalela& zona) {
    // Cada chucho se asigna a la zona donde esta al inicio del frame.
    clasificar(zona);
    colisiones_.preparar(static_cast<int>(chuchos_.size()));

    const std::vector<int>& sec = indicesSecuencial_;
    const std::vector<int>& par = indicesParalelo_;
    TiemposFrame t;

    t.msSecuencial += medirMs([&] { moverChuchos(chuchos_, sec, dt, false); });
    t.msParalelo   += medirMs([&] { moverChuchos(chuchos_, par, dt, true); });

    t.msSecuencial += medirMs([&] { colisiones_.detectar(chuchos_, sec, false); });
    t.msParalelo   += medirMs([&] { colisiones_.detectar(chuchos_, par, true); });

    t.msSecuencial += medirMs([&] { colisiones_.aplicar(chuchos_, sec, false); });
    t.msParalelo   += medirMs([&] { colisiones_.aplicar(chuchos_, par, true); });

    return t;
}
