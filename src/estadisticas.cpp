#include "estadisticas.h"

namespace {

void sumar(TiemposFrame& acumulado, const TiemposFrame& t) {
    acumulado.msSecuencial += t.msSecuencial;
    acumulado.msParalelo += t.msParalelo;
}

TiemposFrame dividir(const TiemposFrame& t, double divisor) {
    return TiemposFrame{t.msSecuencial / divisor, t.msParalelo / divisor};
}

}  // namespace

void Estadisticas::registrarFrame(float dt, const TiemposFrame& tiempos) {
    tiempoIntervalo_ += dt;
    ++framesIntervalo_;
    sumar(sumaIntervalo_, tiempos);

    tiempoTotal_ += dt;
    ++framesTotales_;
    sumar(sumaTotal_, tiempos);

    if (tiempoIntervalo_ >= kIntervalo) {
        fpsActual_ = static_cast<float>(framesIntervalo_) / tiempoIntervalo_;
        tiemposActuales_ = dividir(sumaIntervalo_, framesIntervalo_);
        tiempoIntervalo_ = 0.0f;
        framesIntervalo_ = 0;
        sumaIntervalo_ = TiemposFrame{};
    }
}

float Estadisticas::fpsPromedio() const {
    if (tiempoTotal_ <= 0.0) {
        return 0.0f;
    }
    return static_cast<float>(framesTotales_ / tiempoTotal_);
}

TiemposFrame Estadisticas::tiemposPromedio() const {
    if (framesTotales_ == 0) {
        return TiemposFrame{};
    }
    return dividir(sumaTotal_, static_cast<double>(framesTotales_));
}
