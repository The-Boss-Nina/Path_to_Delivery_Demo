#include "Camera.h"
#include "GameObject.h"
#include "Game.h"
#include "InputManager.h"
#include <SDL.h>

Vec2 Camera::pos(0.0f, 0.0f);
Vec2 Camera::speed(0.0f, 0.0f);
GameObject* Camera::focus = nullptr;

void Camera::Follow(GameObject* newFocus) {
    focus = newFocus;
}

void Camera::Unfollow() {
    focus = nullptr;
}

void Camera::Update(float dt) {
    int w = 0;
    int h = 0;
    SDL_RenderGetLogicalSize(Game::GetInstance("", 0, 0).GetRenderer(), &w, &h);
    if (w == 0 || h == 0) {
        SDL_GetRendererOutputSize(Game::GetInstance("", 0, 0).GetRenderer(), &w, &h);
    }

    if (focus != nullptr) {
        pos.x = focus->box.x + focus->box.w / 2.0f - w / 2.0f;
        pos.y = focus->box.y + focus->box.h / 2.0f - h / 2.0f;
    } else {
        InputManager& input = InputManager::GetInstance();
        speed = Vec2(0.0f, 0.0f);
        const float cameraSpeed = 300.0f;

        if (input.IsKeyDown(LEFT_ARROW_KEY))  speed.x -= cameraSpeed;
        if (input.IsKeyDown(RIGHT_ARROW_KEY)) speed.x += cameraSpeed;
        if (input.IsKeyDown(UP_ARROW_KEY))    speed.y -= cameraSpeed;
        if (input.IsKeyDown(DOWN_ARROW_KEY))  speed.y += cameraSpeed;

        pos = pos + speed * dt;
    }

    const float mapW = 40.0f * 64.0f;
    const float mapH = 40.0f * 64.0f;

    if (pos.x < 0.0f) pos.x = 0.0f;
    if (pos.y < 0.0f) pos.y = 0.0f;

    if (pos.x > mapW - w) pos.x = mapW - w;
    if (pos.y > mapH - h) pos.y = mapH - h;

    if (pos.x < 0.0f) pos.x = 0.0f;
    if (pos.y < 0.0f) pos.y = 0.0f;
}
