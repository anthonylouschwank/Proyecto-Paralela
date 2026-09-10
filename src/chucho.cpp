#include "chucho.h"

#include <cmath>
#include <random>

#include "config.h"

namespace {

constexpr float kPi = 3.14159265358979f;

// Convierte un color HSV (h en [0, 360), s y v en [0, 1]) a RGB.
// Usamos HSV para escoger tonos aleatorios que siempre sean vivos.
Color hsvARgb(float h, float s, float v) {
    const float c = v * s;
    const float hp = h / 60.0f;
    const float x = c * (1.0f - std::fabs(std::fmod(hp, 2.0f) - 1.0f));

    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (hp < 1.0f)      { r = c; g = x; }
    else if (hp < 2.0f) { r = x; g = c; }
    else if (hp < 3.0f) { g = c; b = x; }
    else if (hp < 4.0f) { g = x; b = c; }
    else if (hp < 5.0f) { r = x; b = c; }
    else                { r = c; b = x; }

    const float m = v - c;
    return Color{
        static_cast<std::uint8_t>((r + m) * 255.0f),
        static_cast<std::uint8_t>((g + m) * 255.0f),
        static_cast<std::uint8_t>((b + m) * 255.0f),
    };
}

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
        c.tinte = hsvARgb(distTono(rng), 0.55f, 1.0f);
        chuchos.push_back(c);
    }

    return chuchos;
}
