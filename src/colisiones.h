#pragma once

#include <vector>

#include "chucho.h"

// Detecta y resuelve choques entre chuchos usando cajas envolventes.
//
// Se trabaja en dos fases, y cada una recibe la lista de chuchos que debe
// procesar y si lo hace en paralelo (OpenMP) o en un solo hilo:
//
//   detectar: cada chucho i revisa a TODOS los demas y anota en su propia
//     entrada del buffer la velocidad y la rapidez de cada companero con el
//     que se traslapa y se esta acercando. Solo LEE a los chuchos y solo
//     ESCRIBE la entrada i del buffer.
//
//   aplicar: cada chucho que choco toma
//       direccion = la del promedio de las velocidades de sus companeros
//       rapidez   = el promedio de las rapideces de sus companeros
//     Con un solo companero esto es intercambiar velocidades (choque
//     elastico entre masas iguales). Con varios, la rapidez siempre queda
//     dentro del rango inicial; promediar los vectores directamente le
//     quitaria energia al sistema hasta que todos se detienen.
//
// Por que "cada i revisa a todos" y no el triangulo i < j: en el triangulo,
// al encontrar un choque se escribe en la entrada i Y en la j, y en paralelo
// dos hilos podrian escribir la misma j al mismo tiempo (condicion de
// carrera). Revisando la fila completa cada hilo escribe solo en las
// entradas de sus propios chuchos, sin locks ni atomics. El costo es hacer
// N*(N-1) revisiones en vez de N*(N-1)/2.
//
// Ademas, cada chucho suma a sus companeros siempre en el mismo orden
// (j = 0, 1, 2, ...), asi que el resultado es identico bit a bit sin
// importar cuantos hilos se usen ni que chuchos esten en cada zona.
class ManejadorColisiones {
public:
    // Deja los buffers listos para n chuchos. Llamar una vez por frame,
    // antes de detectar.
    void preparar(int n);

    // Fase 1 para los chuchos en "indices".
    void detectar(const std::vector<Chucho>& chuchos, const std::vector<int>& indices,
                  bool paralelo);

    // Version secuencial de referencia de la fase 1 (la de la Parte 2), para
    // TODOS los chuchos: recorre solo el triangulo i < j, o sea la mitad de
    // revisiones, pero al encontrar un choque escribe en i Y en j, por eso no
    // se puede paralelizar tal cual. Da exactamente el mismo resultado que
    // detectar(). El benchmark la usa como "mejor version secuencial" para
    // calcular el speedup.
    void detectarTriangulo(const std::vector<Chucho>& chuchos);

    // Fase 2 para los chuchos en "indices". Debe llamarse cuando ya se
    // detecto para TODOS los chuchos.
    void aplicar(std::vector<Chucho>& chuchos, const std::vector<int>& indices,
                 bool paralelo) const;

private:
    // Por cada chucho i, sumas sobre todos los chuchos j con los que choco:
    // velocidad de j, rapidez de j, y cuantos fueron.
    std::vector<float> sumaVx_;
    std::vector<float> sumaVy_;
    std::vector<float> sumaRapidez_;
    std::vector<int> choques_;
};
