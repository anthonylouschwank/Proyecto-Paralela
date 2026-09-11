#include "simulacion.h"

#include <cmath>

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

}  // namespace

void actualizarChuchos(std::vector<Chucho>& chuchos, float dt) {
    const int n = static_cast<int>(chuchos.size());

    for (int i = 0; i < n; ++i) {
        Chucho& c = chuchos[i];

        c.x += c.vx * dt;
        c.y += c.vy * dt;

        rebotarEje(c.x, c.vx, config::kMinX, config::kMaxX);
        rebotarEje(c.y, c.vy, config::kMinY, config::kMaxY);

        c.pausaColor = std::fmax(0.0f, c.pausaColor - dt);
    }
}
