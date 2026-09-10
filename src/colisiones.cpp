#include "colisiones.h"

#include <cmath>

#include "config.h"

namespace {

// Dos cajas del mismo tamano se traslapan si sus esquinas estan mas cerca
// que el ancho en x y que el alto en y.
bool seTraslapan(const Chucho& a, const Chucho& b) {
    return std::fabs(a.x - b.x) < config::kAnchoCaja &&
           std::fabs(a.y - b.y) < config::kAltoCaja;
}

// Dos chuchos se acercan si su velocidad relativa apunta en contra del
// vector que los separa (producto punto negativo). Sin esta revision, un
// par que sigue traslapado en el frame siguiente volveria a intercambiar
// velocidades y se quedaria "pegado" vibrando.
bool seAcercan(const Chucho& a, const Chucho& b) {
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    const float dvx = b.vx - a.vx;
    const float dvy = b.vy - a.vy;
    return dx * dvx + dy * dvy < 0.0f;
}

}  // namespace

int ManejadorColisiones::resolver(std::vector<Chucho>& chuchos) {
    prepararBuffers(static_cast<int>(chuchos.size()));
    const int pares = detectarChoques(chuchos);
    aplicarChoques(chuchos);
    return pares;
}

void ManejadorColisiones::prepararBuffers(int n) {
    sumaVx_.assign(n, 0.0f);
    sumaVy_.assign(n, 0.0f);
    sumaRapidez_.assign(n, 0.0f);
    choques_.assign(n, 0);
}

// Fase 1: O(N^2 / 2) revisiones. Es la region mas cara de la simulacion y la
// principal candidata a paralelizar. Ojo: al anotar un choque se escribe en
// la posicion i Y en la j, por eso en paralelo habra que cuidar condiciones
// de carrera sobre los buffers.
int ManejadorColisiones::detectarChoques(const std::vector<Chucho>& chuchos) {
    const int n = static_cast<int>(chuchos.size());
    int pares = 0;

    for (int i = 0; i < n; ++i) {
        const Chucho& a = chuchos[i];
        for (int j = i + 1; j < n; ++j) {
            const Chucho& b = chuchos[j];
            if (!seTraslapan(a, b) || !seAcercan(a, b)) {
                continue;
            }

            // a recibe lo de b y b recibe lo de a
            sumaVx_[i] += b.vx;
            sumaVy_[i] += b.vy;
            sumaRapidez_[i] += std::hypot(b.vx, b.vy);
            ++choques_[i];

            sumaVx_[j] += a.vx;
            sumaVy_[j] += a.vy;
            sumaRapidez_[j] += std::hypot(a.vx, a.vy);
            ++choques_[j];

            ++pares;
        }
    }
    return pares;
}

// Fase 2: cada chucho solo modifica su propio estado, no hay dependencias.
void ManejadorColisiones::aplicarChoques(std::vector<Chucho>& chuchos) const {
    const int n = static_cast<int>(chuchos.size());

    for (int i = 0; i < n; ++i) {
        const int k = choques_[i];
        if (k == 0) {
            continue;
        }

        Chucho& c = chuchos[i];
        const float rapidez = sumaRapidez_[i] / k;

        // Direccion: la del promedio de las velocidades de los companeros.
        // Si se cancelan (p. ej. dos companeros opuestos), el chucho da
        // media vuelta.
        float dirX = sumaVx_[i] / k;
        float dirY = sumaVy_[i] / k;
        float largo = std::hypot(dirX, dirY);
        if (largo < 1e-4f) {
            dirX = -c.vx;
            dirY = -c.vy;
            largo = std::hypot(dirX, dirY);
        }
        if (largo >= 1e-4f) {
            c.vx = dirX / largo * rapidez;
            c.vy = dirY / largo * rapidez;
        }

        if (config::kCambiarTinteAlChocar && c.pausaColor <= 0.0f) {
            c.tono = std::fmod(c.tono + config::kGiroTonoPorChoque, 360.0f);
            c.tinte = colorDesdeTono(c.tono);
            c.pausaColor = config::kPausaCambioColor;
        }
    }
}
