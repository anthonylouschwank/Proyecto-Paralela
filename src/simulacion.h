#pragma once

#include <vector>

#include "chucho.h"
#include "colisiones.h"
#include "zona.h"

// Milisegundos que tardo cada zona en un paso de simulacion.
struct TiemposFrame {
    double msSecuencial = 0.0;
    double msParalelo = 0.0;
};

// Mueve los chuchos cuyos indices vienen en "indices":
//   1. Integra la posicion (x += vx*dt, y += vy*dt).
//   2. Rebota contra los bordes de la ventana.
//   3. Descuenta dt de la pausa para cambiar de color.
//
// Cada iteracion solo toca a su propio chucho, asi que no hay dependencias
// entre iteraciones. Si "paralelo" es true se reparte entre los hilos de
// OpenMP; si no, corre en un solo hilo.
void moverChuchos(std::vector<Chucho>& chuchos, const std::vector<int>& indices,
                  float dt, bool paralelo);

// Dueno del estado de los chuchos. En cada paso los separa segun la zona
// donde estan (secuencial o paralela) y procesa cada grupo por su lado,
// midiendo cuanto tarda cada uno.
class Simulacion {
public:
    explicit Simulacion(std::vector<Chucho> chuchos);

    // Avanza la simulacion dt segundos y devuelve cuanto tardo cada zona.
    TiemposFrame paso(float dt, const ZonaParalela& zona);

    const std::vector<Chucho>& chuchos() const { return chuchos_; }

    // Cuantos chuchos quedaron en cada zona en el ultimo paso.
    int chuchosSecuenciales() const { return static_cast<int>(indicesSecuencial_.size()); }
    int chuchosParalelos() const { return static_cast<int>(indicesParalelo_.size()); }

private:
    void clasificar(const ZonaParalela& zona);

    std::vector<Chucho> chuchos_;
    std::vector<int> indicesSecuencial_;
    std::vector<int> indicesParalelo_;
    ManejadorColisiones colisiones_;
};
