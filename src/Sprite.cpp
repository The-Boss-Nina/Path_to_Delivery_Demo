#include "Sprite.h"
#include "Game.h"
#include "Resources.h"
#include "Camera.h"
#include <SDL_image.h>

Sprite::Sprite()
    : texture(nullptr), width(0), height(0), frameCountW(1), frameCountH(1),
      cameraFollower(false), flip(SDL_FLIP_NONE), scale(1.0f, 1.0f), clipRect{0, 0, 0, 0} {}

Sprite::Sprite(std::string file, int frameCountW, int frameCountH)
    : texture(nullptr), width(0), height(0), frameCountW(frameCountW), frameCountH(frameCountH),
      cameraFollower(false), flip(SDL_FLIP_NONE), scale(1.0f, 1.0f), clipRect{0, 0, 0, 0} {
    Open(Game::GetInstance("", 0, 0).GetRenderer(), file);
}

Sprite::~Sprite() {}

void Sprite::Open(std::string file) {
    texture = Resources::GetImage(file);
    if (!texture) {
        width = 0;
        height = 0;
        return;
    }
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    SetFrame(0);
}

void Sprite::Open(SDL_Renderer* renderer, std::string file) {
    (void)renderer;
    Open(file);
}

void Sprite::SetClip(int x, int y, int w, int h) {
    clipRect = {x, y, w, h};
}

void Sprite::Render(SDL_Renderer* renderer, int x, int y, int w, int h, double angle) {
    if (!texture) return;

    SDL_Rect dst;
    if (cameraFollower) {
        dst = {x, y, w, h};
    } else {
        dst = {static_cast<int>(x - Camera::pos.x), static_cast<int>(y - Camera::pos.y), w, h};
    }

    SDL_RenderCopyEx(renderer, texture, &clipRect, &dst, angle, nullptr, flip);
}

void Sprite::SetFrame(int frame) {
    if (frameCountW <= 0) frameCountW = 1;
    if (frameCountH <= 0) frameCountH = 1;

    int frameW = width / frameCountW;
    int frameH = height / frameCountH;
    int maxFrames = frameCountW * frameCountH;

    if (frame < 0) frame = 0;
    if (frame >= maxFrames) frame = maxFrames - 1;

    int col = frame % frameCountW;
    int row = frame / frameCountW;

    SetClip(col * frameW, row * frameH, frameW, frameH);
}

void Sprite::SetFrameCount(int frameCountW, int frameCountH) {
    this->frameCountW = frameCountW > 0 ? frameCountW : 1;
    this->frameCountH = frameCountH > 0 ? frameCountH : 1;
    SetFrame(0);
}

void Sprite::SetCameraFollower(bool cameraFollower) {
    this->cameraFollower = cameraFollower;
}

void Sprite::SetScale(float scaleX, float scaleY) {
    if (scaleX != 0) scale.x = scaleX;
    if (scaleY != 0) scale.y = scaleY;
}

Vec2 Sprite::GetScale() const {
    return scale;
}

void Sprite::SetFlip(SDL_RendererFlip flip) {
    this->flip = flip;
}

int Sprite::GetWidth() const {
    int base = frameCountW == 0 ? width : width / frameCountW;
    return static_cast<int>(base * scale.x);
}

int Sprite::GetHeight() const {
    int base = frameCountH == 0 ? height : height / frameCountH;
    return static_cast<int>(base * scale.y);
}

bool Sprite::IsOpen() const { return texture != nullptr; }
