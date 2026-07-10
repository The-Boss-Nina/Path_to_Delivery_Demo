#include "ObjectivesState.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "StageState.h"
#include <SDL.h>

namespace {
constexpr float kFrameDuration = 3.0f;
constexpr int kFrameCount = 6;
}  // namespace

ObjectivesState::ObjectivesState(VehicleType selectedVehicle)
    : State(), selectedVehicle(selectedVehicle), currentFrame(0) {}

ObjectivesState::~ObjectivesState() {}

void ObjectivesState::LoadAssets() {
    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/Background.png");
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);

    // Painel de objetivos — spritesheet 6x1, um quadro a cada kFrameDuration
    // segundos, ancorado no topo da tela e centralizado horizontalmente.
    GameObject* objectives = new GameObject();
    SpriteRenderer* objectivesSr = new SpriteRenderer(*objectives, "recursos/img/stage1-0.png", kFrameCount, 1);
    objectivesSr->SetCameraFollower(true);
    objectivesSr->SetFrame(0);
    objectives->AddComponent(objectivesSr);
    objectives->box.x = (1280.0f - objectives->box.w) / 2.0f;
    objectives->box.y = 0.0f;
    AddObject(objectives);
}

void ObjectivesState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

void ObjectivesState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    frameTimer.Update(dt);
    if (frameTimer.Get() >= kFrameDuration) {
        frameTimer.Restart();
        currentFrame++;

        if (currentFrame >= kFrameCount) {
            Game::GetInstance().Push(new StageState(selectedVehicle));
            return;
        }

        SpriteRenderer* sr = objectArray[1]->GetComponent<SpriteRenderer>();
        if (sr) sr->SetFrame(currentFrame);
    }
}

void ObjectivesState::Render() {
    RenderArray();
}

void ObjectivesState::Pause() {}

void ObjectivesState::Resume() {}
