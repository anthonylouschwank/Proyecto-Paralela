#pragma once

#include <SDL.h>

#include <vector>

#include "chucho.h"
#include "zona.h"

// El orden de dibujo en cada frame es:
//   dibujarFondo -> dibujarChuchos -> dibujarEtiquetasZonas -> (HUD)

// Pinta el fondo: la zona secuencial con el color normal y la zona paralela
// con otro color, separadas por una linea.
void dibujarFondo(SDL_Renderer* renderer, const ZonaParalela& zona);

// Dibuja todos los chuchos con su tinte. Si un chucho va hacia la
// izquierda, el sprite se voltea para que siempre mire hacia donde camina.
void dibujarChuchos(SDL_Renderer* renderer, SDL_Texture* texturaChucho,
                    const std::vector<Chucho>& chuchos);

// Escribe abajo de cada zona su nombre y cuantos chuchos tiene.
void dibujarEtiquetasZonas(SDL_Renderer* renderer, const ZonaParalela& zona,
                           int chuchosSecuenciales, int chuchosParalelos);
