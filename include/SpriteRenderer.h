#pragma once
#include "Component.h"
#include "Sprite.h"
#include <SDL.h>
#include <string>

class SpriteRenderer : public Component {
public:
    SpriteRenderer(GameObject& associated);
    SpriteRenderer(GameObject& associated, std::string file, int frameCountW = 1, int frameCountH = 1);

    void Open(std::string file);
    void SetFrameCount(int frameCountW, int frameCountH);
    void SetCameraFollower(bool cameraFollower);
    void SetScale(float scaleX, float scaleY);
    void Update(float dt) override;
    void Render() override;
    void SetFrame(int frame);
    void SetFrame(int frame, SDL_RendererFlip flip);
    float renderRotation;

private:
    Sprite sprite;
};
