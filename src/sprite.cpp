#include "sprite.h"

#include <cstdint>

#include "config.h"

namespace {

constexpr int kLadoBitmap = 16;
constexpr int kEscala = config::kTamSprite / kLadoBitmap;  // 4

static_assert(kLadoBitmap * kEscala == config::kTamSprite,
              "El bitmap escalado debe medir exactamente kTamSprite");

// Chucho visto de lado, mirando a la derecha.
//   '.' transparente
//   '#' cuerpo (blanco, toma el tinte completo)
//   's' sombra (gris claro, tinte mas oscuro)
//   'k' ojo y nariz (casi negro, el tinte casi no lo afecta)
const char* const kBitmap[kLadoBitmap] = {
    "................",
    "................",
    "...........s....",
    "..........ss....",
    "..........#####.",
    "..........###k#.",
    ".s........######",
    ".s.......#####kk",
    "..s##########...",
    "...##########...",
    "...##########...",
    "...s########s...",
    "...#.#....#.#...",
    "...#.#....#.#...",
    "...s.s....s.s...",
    "................",
};

// Traduce un caracter del bitmap a un pixel RGBA.
SDL_Color colorDe(char c) {
    switch (c) {
        case '#': return SDL_Color{255, 255, 255, 255};
        case 's': return SDL_Color{175, 175, 175, 255};
        case 'k': return SDL_Color{25, 25, 25, 255};
        default:  return SDL_Color{0, 0, 0, 0};
    }
}

}  // namespace

SDL_Texture* crearTexturaChucho(SDL_Renderer* renderer) {
    SDL_Surface* superficie = SDL_CreateRGBSurfaceWithFormat(
        0, config::kTamSprite, config::kTamSprite, 32, SDL_PIXELFORMAT_RGBA32);
    if (superficie == nullptr) {
        return nullptr;
    }

    // Cada celda del bitmap se pinta como un bloque de kEscala x kEscala.
    auto* pixeles = static_cast<std::uint32_t*>(superficie->pixels);
    const int pixelesPorFila = superficie->pitch / 4;

    for (int py = 0; py < config::kTamSprite; ++py) {
        for (int px = 0; px < config::kTamSprite; ++px) {
            const SDL_Color col = colorDe(kBitmap[py / kEscala][px / kEscala]);
            pixeles[py * pixelesPorFila + px] =
                SDL_MapRGBA(superficie->format, col.r, col.g, col.b, col.a);
        }
    }

    SDL_Texture* textura = SDL_CreateTextureFromSurface(renderer, superficie);
    SDL_FreeSurface(superficie);

    if (textura != nullptr) {
        SDL_SetTextureBlendMode(textura, SDL_BLENDMODE_BLEND);
    }
    return textura;
}
