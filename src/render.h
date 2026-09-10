#pragma once

#include <SDL.h>

#include <vector>

#include "chucho.h"

// Limpia la pantalla con el color de fondo y dibuja todos los chuchos con su
// tinte. Si un chucho va hacia la izquierda, el sprite se voltea para que
// siempre mire en la direccion en que camina.
void dibujarEscena(SDL_Renderer* renderer, SDL_Texture* texturaChucho,
                   const std::vector<Chucho>& chuchos);
