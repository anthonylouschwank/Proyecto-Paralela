#include "render.h"

#include <cstdio>

#include "config.h"
#include "texto.h"

namespace {

constexpr int kEscalaEtiqueta = 2;
constexpr int kMargenEtiqueta = 6;
constexpr int kAnchoBorde = 2;

// Dibuja una etiqueta con fondo semitransparente, centrada en el rango
// [izquierda, derecha) y pegada al borde de abajo de la ventana. Si no cabe,
// no se dibuja.
void dibujarEtiqueta(SDL_Renderer* renderer, const char* texto, int izquierda, int derecha) {
    const int ancho = anchoTexto(texto, kEscalaEtiqueta) + kMargenEtiqueta * 2;
    const int alto = altoTexto(kEscalaEtiqueta) + kMargenEtiqueta * 2;
    if (ancho > derecha - izquierda) {
        return;
    }

    const SDL_Rect caja{
        izquierda + (derecha - izquierda - ancho) / 2,
        config::kAltoVentana - alto - kMargenEtiqueta,
        ancho,
        alto,
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &caja);

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    dibujarTexto(renderer, texto, caja.x + kMargenEtiqueta, caja.y + kMargenEtiqueta,
                 kEscalaEtiqueta);
}

}  // namespace

void dibujarFondo(SDL_Renderer* renderer, const ZonaParalela& zona) {
    SDL_SetRenderDrawColor(renderer, config::kFondoR, config::kFondoG, config::kFondoB, 255);
    SDL_RenderClear(renderer);

    const int inicio = zona.inicioX();
    if (inicio >= config::kAnchoVentana) {
        return;  // zona de 0%: no hay nada que pintar
    }

    const SDL_Rect areaParalela{inicio, 0, config::kAnchoVentana - inicio, config::kAltoVentana};
    SDL_SetRenderDrawColor(renderer, config::kFondoZonaR, config::kFondoZonaG,
                           config::kFondoZonaB, 255);
    SDL_RenderFillRect(renderer, &areaParalela);

    if (inicio > 0) {
        const SDL_Rect borde{inicio, 0, kAnchoBorde, config::kAltoVentana};
        SDL_SetRenderDrawColor(renderer, config::kBordeZonaR, config::kBordeZonaG,
                               config::kBordeZonaB, 255);
        SDL_RenderFillRect(renderer, &borde);
    }
}

void dibujarChuchos(SDL_Renderer* renderer, SDL_Texture* texturaChucho,
                    const std::vector<Chucho>& chuchos) {
    for (const Chucho& c : chuchos) {
        const SDL_Rect destino{
            static_cast<int>(c.x),
            static_cast<int>(c.y),
            config::kTamSprite,
            config::kTamSprite,
        };
        const SDL_RendererFlip volteo = (c.vx < 0.0f) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

        SDL_SetTextureColorMod(texturaChucho, c.tinte.r, c.tinte.g, c.tinte.b);
        SDL_RenderCopyEx(renderer, texturaChucho, nullptr, &destino, 0.0, nullptr, volteo);
    }
}

void dibujarEtiquetasZonas(SDL_Renderer* renderer, const ZonaParalela& zona,
                           int chuchosSecuenciales, int chuchosParalelos) {
    const int inicio = zona.inicioX();
    char texto[48];

    if (inicio > 0) {
        std::snprintf(texto, sizeof(texto), "SECUENCIAL (%d)", chuchosSecuenciales);
        dibujarEtiqueta(renderer, texto, 0, inicio);
    }
    if (inicio < config::kAnchoVentana) {
        std::snprintf(texto, sizeof(texto), "OPENMP (%d)", chuchosParalelos);
        dibujarEtiqueta(renderer, texto, inicio, config::kAnchoVentana);
    }
}
