#pragma once

#include <cstdint>

// Tinte RGB que se aplica sobre el sprite (que es blanco/gris).
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

// Convierte un tono (en grados, cualquier valor; se normaliza a [0, 360))
// al color RGB que usamos para los chuchos, con la saturacion y brillo de
// config.h.
Color colorDesdeTono(float tono);
