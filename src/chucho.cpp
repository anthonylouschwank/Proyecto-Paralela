#include "chucho.h"

#include <cmath>
#include <random>

#include "config.h"

namespace {

constexpr float kPi = 3.14159265358979f;

}  // namespace

std::vector<Chucho> crearChuchos(int n, unsigned int semilla) {
    std::mt19937 rng(semilla);
    std::uniform_real_distribution<float> distX(config::kMinX, config::kMaxX);
    std::uniform_real_distribution<float> distY(config::kMinY, config::kMaxY);
    std::uniform_real_distribution<float> distAngulo(0.0f, 2.0f * kPi);
    std::uniform_real_distribution<float> distRapidez(config::kRapidezMin, config::kRapidezMax);
    std::uniform_real_distribution<float> distTono(0.0f, 360.0f);

    std::vector<Chucho> chuchos;
    chuchos.reserve(n);

    for (int i = 0; i < n; ++i) {
        const float angulo = distAngulo(rng);
        const float rapidez = distRapidez(rng);

        Chucho c;
        c.x = distX(rng);
        c.y = distY(rng);
        c.vx = rapidez * std::cos(angulo);
        c.vy = rapidez * std::sin(angulo);
        c.tono = distTono(rng);
        c.tinte = colorDesdeTono(c.tono);
        c.pausaColor = 0.0f;
        chuchos.push_back(c);
    }

    return chuchos;
}
