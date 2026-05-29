#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Game.h"

SpriteRenderer::SpriteRenderer(GameObject& associated)
    : Component(associated), sprite() {}

SpriteRenderer::SpriteRenderer(GameObject& associated, std::string file, int frameCountW, int frameCountH)
    : Component(associated), sprite(), renderRotation(0.0f) {
    sprite.SetFrameCount(frameCountW, frameCountH);
    Open(file);
    SetFrame(0);
}

void SpriteRenderer::Open(std::string file) {
    sprite.Open(Game::GetInstance("", 0, 0).GetRenderer(), file);
    associated.box.w = static_cast<float>(sprite.GetWidth());
    associated.box.h = static_cast<float>(sprite.GetHeight());
}

void SpriteRenderer::SetFrameCount(int frameCountW, int frameCountH) {
    sprite.SetFrameCount(frameCountW, frameCountH);
    associated.box.w = static_cast<float>(sprite.GetWidth());
    associated.box.h = static_cast<float>(sprite.GetHeight());
}

void SpriteRenderer::SetCameraFollower(bool cameraFollower) {
    sprite.SetCameraFollower(cameraFollower);
}

void SpriteRenderer::SetScale(float scaleX, float scaleY) {
    Vec2 center = associated.box.Center();
    sprite.SetScale(scaleX, scaleY);
    associated.box.w = static_cast<float>(sprite.GetWidth());
    associated.box.h = static_cast<float>(sprite.GetHeight());
    associated.box.x = center.x - associated.box.w / 2.0f;
    associated.box.y = center.y - associated.box.h / 2.0f;
}

void SpriteRenderer::Update(float dt) { (void)dt; }

void SpriteRenderer::Render() {
    sprite.Render(Game::GetInstance("", 0, 0).GetRenderer(),
                  static_cast<int>(associated.box.x),
                  static_cast<int>(associated.box.y),
                  static_cast<int>(associated.box.w),
                  static_cast<int>(associated.box.h),
                  renderRotation);
}

void SpriteRenderer::SetFrame(int frame) {
    sprite.SetFrame(frame);
}

void SpriteRenderer::SetFrame(int frame, SDL_RendererFlip flip) {
    sprite.SetFlip(flip);
    sprite.SetFrame(frame);
}
