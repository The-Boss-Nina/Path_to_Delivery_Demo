#include "DeliveryPlayer.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include <SDL.h>
#include <cmath>

static float SpeedForVehicle(VehicleType type) {
    switch (type) {
        case VehicleType::HARLEY:
            return 360.0f;
        case VehicleType::VESPA:
            return 320.0f;
        case VehicleType::BUGGY:
            return 260.0f;
        default:
            return 320.0f;
    }
}

DeliveryPlayer::DeliveryPlayer(GameObject& associated, VehicleType type)
    : Component(associated), type(type), speed(SpeedForVehicle(type)) {}

void DeliveryPlayer::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    Vec2 movement(0.0f, 0.0f);

    if (input.IsKeyDown(SDLK_w) || input.IsKeyDown(UP_ARROW_KEY)) {
        movement.y -= 1.0f;
    }
    if (input.IsKeyDown(SDLK_s) || input.IsKeyDown(DOWN_ARROW_KEY)) {
        movement.y += 1.0f;
    }
    if (input.IsKeyDown(SDLK_a) || input.IsKeyDown(LEFT_ARROW_KEY)) {
        movement.x -= 1.0f;
    }
    if (input.IsKeyDown(SDLK_d) || input.IsKeyDown(RIGHT_ARROW_KEY)) {
        movement.x += 1.0f;
    }

    if (movement.x != 0.0f || movement.y != 0.0f) {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x = movement.x / length;
        movement.y = movement.y / length;

        associated.box.x += movement.x * speed * dt;
        associated.box.y += movement.y * speed * dt;

        UpdateSpriteDirection(movement);
    }

    // Limites simples para o jogador não sair da primeira área do mapa.
    if (associated.box.x < 0) associated.box.x = 0;
    if (associated.box.y < 0) associated.box.y = 0;
    if (associated.box.x > 3600) associated.box.x = 3600;
    if (associated.box.y > 2300) associated.box.y = 2300;
}

void DeliveryPlayer::Render() {}

void DeliveryPlayer::UpdateSpriteDirection(const Vec2& movement) {
    SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
    if (!sr) return;

    int frame = 0;

    // Os sprites dos veículos estão organizados em uma linha com direções.
    if (movement.x > 0.4f && movement.y < -0.4f) frame = 5;        // diagonal cima-direita
    else if (movement.x > 0.4f && movement.y > 0.4f) frame = 7;   // diagonal baixo-direita
    else if (movement.x < -0.4f && movement.y < -0.4f) frame = 0; // diagonal cima-esquerda
    else if (movement.x < -0.4f && movement.y > 0.4f) frame = 2;  // diagonal baixo-esquerda
    else if (movement.x > 0.4f) frame = 6;                        // direita
    else if (movement.x < -0.4f) frame = 1;                       // esquerda
    else if (movement.y < -0.4f) frame = 3;                       // cima
    else if (movement.y > 0.4f) frame = 4;                        // baixo

    sr->SetFrame(frame);
}
