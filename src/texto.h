#pragma once

#include <SDL.h>

// Fuente de pixeles propia (5 filas de alto), asi no dependemos de
// SDL2_ttf. Soporta A-Z, 0-9 y algunos simbolos (: . % - ( )); cualquier
// otro caracter se dibuja como espacio.
//
// "escala" es el tamano en pixeles de cada punto de la fuente.

// Alto en pixeles de una linea de texto.
int altoTexto(int escala);

// Ancho en pixeles que ocupa un texto.
int anchoTexto(const char* texto, int escala);

// Dibuja el texto con el color actual del renderer, desde (x, y).
void dibujarTexto(SDL_Renderer* renderer, const char* texto, int x, int y, int escala);
