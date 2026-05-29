#include "DeliveryPlayer.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Vehicle.h"
#include <SDL.h>

DeliveryPlayer::DeliveryPlayer(GameObject& associated, VehicleType type)
    : Component(associated), type(type) {}

void DeliveryPlayer::Start() {
    Vehicle* v = associated.GetComponent<Vehicle>();
    if (!v) return;

    // Cada veículo tem características próprias de física.
    switch (type) {
        case VehicleType::HARLEY:
            v->SetMaxSpeed(360.0f);
            v->SetAcceleration(220.0f);
            v->SetTurnSpeed(130.0f);
            break;
        case VehicleType::VESPA:
            v->SetMaxSpeed(320.0f);
            v->SetAcceleration(180.0f);
            v->SetTurnSpeed(150.0f);
            break;
        case VehicleType::BUGGY:
            v->SetMaxSpeed(260.0f);
            v->SetAcceleration(280.0f);
            v->SetTurnSpeed(100.0f);
            break;
    }
}

void DeliveryPlayer::Update(float dt) {
    (void)dt;

    Vehicle* v = associated.GetComponent<Vehicle>();
    if (!v) return;

    InputManager& input = InputManager::GetInstance();

    if (input.IsKeyDown(SDLK_w) || input.IsKeyDown(UP_ARROW_KEY))
        v->Issue(Vehicle::Command(Vehicle::Command::ACCELERATE));

    if (input.IsKeyDown(SDLK_s) || input.IsKeyDown(DOWN_ARROW_KEY))
        v->Issue(Vehicle::Command(Vehicle::Command::BRAKE));

    if (input.IsKeyDown(SDLK_a) || input.IsKeyDown(LEFT_ARROW_KEY))
        v->Issue(Vehicle::Command(Vehicle::Command::TURN_LEFT));

    if (input.IsKeyDown(SDLK_d) || input.IsKeyDown(RIGHT_ARROW_KEY))
        v->Issue(Vehicle::Command(Vehicle::Command::TURN_RIGHT));
}

void DeliveryPlayer::Render() {}