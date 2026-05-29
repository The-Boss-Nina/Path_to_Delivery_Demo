#pragma once
#include <SDL.h>

class Animation {
public:
    Animation();
    Animation(int frameStart, int frameEnd, float frameTime, SDL_RendererFlip flip = SDL_FLIP_NONE);

    int frameStart;
    int frameEnd;
    float frameTime;
    SDL_RendererFlip flip;
};
