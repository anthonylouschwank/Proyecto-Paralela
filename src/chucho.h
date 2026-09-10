#pragma once

#include <cstdint>
#include <vector>

// Tinte RGB que se aplica sobre el sprite (que es blanco/gris).
struct Color {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

// Estado de un chucho: posicion (esquina superior izquierda del sprite),
// velocidad en pixeles por segundo y su tinte.
struct Chucho {
    float x;
    float y;
    float vx;
    float vy;
    Color tinte;
};

// Crea n chuchos con posicion, velocidad (magnitud y direccion) y tinte
// aleatorios. La misma semilla produce siempre la misma escena, lo cual
// sirve para comparar corridas secuenciales contra paralelas.
std::vector<Chucho> crearChuchos(int n, unsigned int semilla);
