#pragma once

#include <vector>

#include "chucho.h"

// Avanza la simulacion un paso de dt segundos:
//   1. Integra la posicion de cada chucho (x += vx*dt, y += vy*dt).
//   2. Rebota contra los bordes de la ventana.
//   3. Descuenta dt de la pausa para cambiar de color.
//
// Este bucle no tiene dependencias entre iteraciones: es la primera region
// candidata a paralelizar con OpenMP.
void actualizarChuchos(std::vector<Chucho>& chuchos, float dt);
