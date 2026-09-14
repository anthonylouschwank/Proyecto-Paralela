#pragma once

#include "chucho.h"

// Franja derecha de la pantalla cuyos chuchos se procesan en paralelo.
// Se define con un porcentaje del ancho de la ventana:
//   0   -> no hay zona paralela (todo secuencial)
//   50  -> mitad derecha paralela
//   100 -> toda la pantalla paralela
class ZonaParalela {
public:
    explicit ZonaParalela(int porcentaje);

    int porcentaje() const { return porcentaje_; }

    // Suma delta al porcentaje, sin salirse de [0, 100].
    void ajustar(int delta);

    // Coordenada x (en pixeles) donde empieza la zona.
    int inicioX() const;

    // Un chucho pertenece a la zona si su centro esta dentro de ella.
    bool contiene(const Chucho& c) const;

private:
    int porcentaje_;
};
