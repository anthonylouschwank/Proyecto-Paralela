#pragma once

#include <vector>

#include "color.h"

// Estado de un chucho: posicion (esquina superior izquierda del sprite),
// velocidad en pixeles por segundo, tono (grados) y el tinte RGB que sale
// de ese tono. Guardamos el tinte ya calculado para no convertir de HSV a
// RGB cada vez que se dibuja.
struct Chucho {
    float x;
    float y;
    float vx;
    float vy;
    float tono;
    Color tinte;
    float pausaColor;  // segundos que faltan para poder cambiar de color otra vez
};

// Crea n chuchos con posicion, velocidad (magnitud y direccion) y tono
// aleatorios. La misma semilla produce siempre la misma escena, lo cual
// sirve para comparar corridas secuenciales contra paralelas.
std::vector<Chucho> crearChuchos(int n, unsigned int semilla);
