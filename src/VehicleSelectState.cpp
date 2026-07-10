#include "VehicleSelectState.h"
#include "DeliveryPlayer.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "ObjectivesState.h"
#include "SpriteRenderer.h"
#include <SDL.h>
#include <string>

VehicleSelectState::VehicleSelectState()
    : State() {}

VehicleSelectState::~VehicleSelectState() {}

static GameObject* CreateOption(State& state,
                                const std::string& file,
                                float x,
                                float y,
                                int frameCountW,
                                int frameCountH,
                                int frame,
                                float scaleX = 2.0f,
                                float scaleY = 2.0f) {
    GameObject* object = new GameObject();
    SpriteRenderer* sr = new SpriteRenderer(*object, file, frameCountW, frameCountH);
    sr->SetFrame(frame);
    sr->SetScale(scaleX, scaleY);
    object->AddComponent(sr);
    object->box.x = x;
    object->box.y = y;
    state.AddObject(object);
    return object;
}

void VehicleSelectState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

void VehicleSelectState::LoadAssets() {
    // Tela simples de seleção dos 30%.
    // Controles:
    // 1 = Harley | 2 = Vespa | 3 = Uno | 4 = Mustang | ESC = sair

    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/Background.png");
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);

    CreateOption(*this, "recursos/img/V3_harley_sheet.png", 225.0f, 300.0f, 10, 1, 10, 1.45f, 1.45f);
    CreateOption(*this, "recursos/img/V3vezpa.png", 450.0f, 300.0f, 5, 3, 4, 1.6f, 1.6f);
    CreateOption(*this, "recursos/img/V3uno.png", 660.0f, 300.0f, 5, 3, 4, 1.6f, 1.6f);
    CreateOption(*this, "recursos/img/V3_mustang0008-sheet.png", 885.0f, 300.0f, 10, 1, 10, 1.75f, 1.75f);
}

void VehicleSelectState::Update(float dt) {
    (void)dt;

    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    if (input.KeyPress(SDLK_1) || input.KeyPress(SDLK_KP_1)) {
        Game::GetInstance().Push(new ObjectivesState(VehicleType::HARLEY));
    }

    if (input.KeyPress(SDLK_2) || input.KeyPress(SDLK_KP_2)) {
        Game::GetInstance().Push(new ObjectivesState(VehicleType::VESPA));
    }

    if (input.KeyPress(SDLK_3) || input.KeyPress(SDLK_KP_3)) {
        Game::GetInstance().Push(new ObjectivesState(VehicleType::UNO));
    }

    if (input.KeyPress(SDLK_4) || input.KeyPress(SDLK_KP_4)) {
        Game::GetInstance().Push(new ObjectivesState(VehicleType::MUSTANG));
    }

    UpdateArray(dt);
}

void VehicleSelectState::Render() {
    RenderArray();
}

void VehicleSelectState::Pause() {}
void VehicleSelectState::Resume() {}
