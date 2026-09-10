#pragma once

#include <SDL.h>

// Genera por codigo la textura del chucho (64x64, RGBA) a partir de un
// bitmap de 16x16 escalado 4x. El sprite se dibuja en blanco y grises para
// que SDL_SetTextureColorMod pueda tenirlo de cualquier color.
//
// Devuelve nullptr si falla; el llamador es dueno de la textura.
SDL_Texture* crearTexturaChucho(SDL_Renderer* renderer);
