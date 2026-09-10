#pragma once

#include <vector>

#include "chucho.h"

// Detecta y resuelve choques entre chuchos usando cajas envolventes.
//
// Se trabaja en dos fases para que el resultado no dependa del orden en que
// se revisan los pares (salvo redondeo de punto flotante). Asi la version
// paralela de la Parte 3 se puede comparar contra la secuencial:
//
//   Fase 1 (detectar): se recorre el triangulo superior de pares (i < j).
//     Por cada par que se traslapa y se esta acercando, se anota en un
//     buffer la velocidad y la rapidez de cada uno en la entrada del otro.
//     Esta fase solo LEE a los chuchos.
//
//   Fase 2 (aplicar): cada chucho que choco toma
//       direccion = la del promedio de las velocidades de sus companeros
//       rapidez   = el promedio de las rapideces de sus companeros
//     Con un solo companero esto es intercambiar velocidades (choque
//     elastico entre masas iguales). Con varios, la rapidez siempre queda
//     dentro del rango inicial; promediar los vectores directamente le
//     quitaria energia al sistema hasta que todos se detienen.
//
// La clase guarda los buffers para reutilizarlos entre frames y no pedir
// memoria nueva cada vez.
class ManejadorColisiones {
public:
    // Resuelve los choques del frame actual. Devuelve cuantos pares
    // chocaron.
    int resolver(std::vector<Chucho>& chuchos);

private:
    void prepararBuffers(int n);
    int detectarChoques(const std::vector<Chucho>& chuchos);
    void aplicarChoques(std::vector<Chucho>& chuchos) const;

    // Por cada chucho i, sumas sobre todos los chuchos j con los que choco:
    // velocidad de j, rapidez de j, y cuantos fueron.
    std::vector<float> sumaVx_;
    std::vector<float> sumaVy_;
    std::vector<float> sumaRapidez_;
    std::vector<int> choques_;
};
