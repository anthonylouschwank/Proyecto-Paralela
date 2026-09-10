#include "render.h"

#include "config.h"

void dibujarEscena(SDL_Renderer* renderer, SDL_Texture* texturaChucho,
                   const std::vector<Chucho>& chuchos) {
    SDL_SetRenderDrawColor(renderer, config::kFondoR, config::kFondoG, config::kFondoB, 255);
    SDL_RenderClear(renderer);

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
