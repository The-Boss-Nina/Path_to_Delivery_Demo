#include "Animation.h"

Animation::Animation()
    : frameStart(0), frameEnd(0), frameTime(0), flip(SDL_FLIP_NONE) {}

Animation::Animation(int frameStart, int frameEnd, float frameTime, SDL_RendererFlip flip)
    : frameStart(frameStart), frameEnd(frameEnd), frameTime(frameTime), flip(flip) {}
