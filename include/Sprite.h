#pragma once
#include "Vec2.h"
#include <SDL.h>
#include <string>

class Sprite {
public:
    Sprite();
    Sprite(std::string file, int frameCountW = 1, int frameCountH = 1);
    ~Sprite();

    void Open(std::string file);
    void Open(SDL_Renderer* renderer, std::string file);
    void SetClip(int x, int y, int w, int h);
    void Render(SDL_Renderer* renderer, int x, int y, int w, int h, double angle = 0.0);
    void SetFrame(int frame);
    void SetFrameCount(int frameCountW, int frameCountH);
    void SetCameraFollower(bool cameraFollower);
    void SetScale(float scaleX, float scaleY);
    Vec2 GetScale() const;
    void SetFlip(SDL_RendererFlip flip);

    int GetWidth() const;
    int GetHeight() const;
    bool IsOpen() const;

private:
    SDL_Texture* texture;
    int width;
    int height;
    int frameCountW;
    int frameCountH;
    bool cameraFollower;
    SDL_RendererFlip flip;
    Vec2 scale;
    SDL_Rect clipRect;
};
