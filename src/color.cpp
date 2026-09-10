#include "color.h"

#include <cmath>

#include "config.h"

namespace {

// Convierte un color HSV (h en [0, 360), s y v en [0, 1]) a RGB.
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

Color colorDesdeTono(float tono) {
    float h = std::fmod(tono, 360.0f);
    if (h < 0.0f) {
        h += 360.0f;
    }
    return hsvARgb(h, config::kSaturacionTinte, config::kBrilloTinte);
}
