#include "colisiones.h"

#include <algorithm>
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

// detectar() sobrescribe la entrada de cada chucho, asi que basta con que
// los buffers tengan el tamano correcto.
void ManejadorColisiones::preparar(int n) {
    sumaVx_.resize(n);
    sumaVy_.resize(n);
    sumaRapidez_.resize(n);
    choques_.resize(n);
}

// Region mas cara de la simulacion: O(N) por chucho, O(N^2) en total.
void ManejadorColisiones::detectar(const std::vector<Chucho>& chuchos,
                                   const std::vector<int>& indices, bool paralelo) {
    // Sin chuchos no vale la pena despertar a los hilos.
    if (indices.empty()) {
        return;
    }
    const int n = static_cast<int>(chuchos.size());
    const int m = static_cast<int>(indices.size());

    // Cada iteracion recorre los n chuchos, asi que todas cuestan casi lo
    // mismo: schedule(static) basta para repartir la carga parejo.
    #pragma omp parallel for schedule(static) if (paralelo)
    for (int k = 0; k < m; ++k) {
        const int i = indices[k];
        const Chucho& a = chuchos[i];

        // Acumuladores locales: cada hilo trabaja en sus propias variables
        // y escribe al buffer una sola vez al final.
        float vx = 0.0f;
        float vy = 0.0f;
        float rapidez = 0.0f;
        int choques = 0;

        for (int j = 0; j < n; ++j) {
            if (j == i) {
                continue;
            }
            const Chucho& b = chuchos[j];
            if (!seTraslapan(a, b) || !seAcercan(a, b)) {
                continue;
            }
            vx += b.vx;
            vy += b.vy;
            rapidez += std::hypot(b.vx, b.vy);
            ++choques;
        }

        sumaVx_[i] = vx;
        sumaVy_[i] = vy;
        sumaRapidez_[i] = rapidez;
        choques_[i] = choques;
    }
}

// Cada chucho i recibe a sus companeros en el mismo orden que en detectar()
// (primero los j < i, desde las filas anteriores, y luego los j > i desde su
// propia fila), por eso las sumas dan exactamente lo mismo.
void ManejadorColisiones::detectarTriangulo(const std::vector<Chucho>& chuchos) {
    const int n = static_cast<int>(chuchos.size());
    std::fill(sumaVx_.begin(), sumaVx_.end(), 0.0f);
    std::fill(sumaVy_.begin(), sumaVy_.end(), 0.0f);
    std::fill(sumaRapidez_.begin(), sumaRapidez_.end(), 0.0f);
    std::fill(choques_.begin(), choques_.end(), 0);

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
        }
    }
}

// Cada chucho solo modifica su propio estado, no hay dependencias.
void ManejadorColisiones::aplicar(std::vector<Chucho>& chuchos,
                                  const std::vector<int>& indices, bool paralelo) const {
    if (indices.empty()) {
        return;
    }
    const int m = static_cast<int>(indices.size());

    #pragma omp parallel for schedule(static) if (paralelo)
    for (int k = 0; k < m; ++k) {
        const int i = indices[k];
        const int choques = choques_[i];
        if (choques == 0) {
            continue;
        }

        Chucho& c = chuchos[i];
        const float rapidez = sumaRapidez_[i] / choques;

        // Direccion: la del promedio de las velocidades de los companeros.
        // Si se cancelan (p. ej. dos companeros opuestos), el chucho da
        // media vuelta.
        float dirX = sumaVx_[i] / choques;
        float dirY = sumaVy_[i] / choques;
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
