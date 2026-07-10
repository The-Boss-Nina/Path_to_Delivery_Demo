#include "VehicleSelectState.h"
#include "DeliveryPlayer.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "StageState.h"
#include "SpriteRenderer.h"
#include <SDL.h>

VehicleSelectState::VehicleSelectState() : State() {}
VehicleSelectState::~VehicleSelectState() {}

void VehicleSelectState::Start() {
    if (started) return;
    LoadAssets();
    StartArray();
    started = true;
}

void VehicleSelectState::LoadAssets() {
    // Usa exatamente a tela ESCOLHA enviada para o projeto.
    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/escolha.png", 1, 1);
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);
}

void VehicleSelectState::Update(float dt) {
    (void)dt;
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    // A tela desenhada mostra somente duas opções:
    // 1 = Harley | 2 = Uno.
    if (input.KeyPress(SDLK_1) || input.KeyPress(SDLK_KP_1)) {
        Game::GetInstance().Push(new StageState(VehicleType::HARLEY));
        return;
    }

    if (input.KeyPress(SDLK_2) || input.KeyPress(SDLK_KP_2)) {
        Game::GetInstance().Push(new StageState(VehicleType::UNO));
        return;
    }
}

void VehicleSelectState::Render() {
    RenderArray();
}

void VehicleSelectState::Pause() {}
void VehicleSelectState::Resume() {}