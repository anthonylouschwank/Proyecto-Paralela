#include "zona.h"

#include <algorithm>

#include "config.h"

ZonaParalela::ZonaParalela(int porcentaje) : porcentaje_(0) {
    ajustar(porcentaje);
}

void ZonaParalela::ajustar(int delta) {
    porcentaje_ = std::clamp(porcentaje_ + delta, 0, 100);
}

int ZonaParalela::inicioX() const {
    return config::kAnchoVentana - config::kAnchoVentana * porcentaje_ / 100;
}

bool ZonaParalela::contiene(const Chucho& c) const {
    const float centroX = c.x + config::kTamSprite / 2.0f;
    return centroX >= static_cast<float>(inicioX());
}
