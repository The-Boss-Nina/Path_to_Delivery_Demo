#include "DificultyState.h"
#include "DeliveryPlayer.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "InstructionsState.h"
#include "SpriteRenderer.h"
#include <SDL.h>

DificultyState::DificultyState() : State() {}
DificultyState::~DificultyState() {}

void DificultyState::Start() {
    if (started) return;
    LoadAssets();
    StartArray();
    started = true;
}

void DificultyState::LoadAssets() {
    // Usa exatamente a tela ESCOLHA enviada para o projeto.
    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/dificuldade.jpeg", 1, 1);
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);
}

void DificultyState::Update(float dt) {
    (void)dt;
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    // A tela desenhada mostra somente duas opções:
    // 1 = Harley | 2 = Uno.
    if (input.KeyPress(SDLK_1) || input.KeyPress(SDLK_KP_1)) {
        Game::GetInstance().dificulty = 1;
        Game::GetInstance().Push(new InstructionsState());
        return;
    }

    if (input.KeyPress(SDLK_2) || input.KeyPress(SDLK_KP_2)) {
        Game::GetInstance().dificulty = 2;
        Game::GetInstance().Push(new InstructionsState());
        return;
    }

    if (input.KeyPress(SDLK_3) || input.KeyPress(SDLK_KP_3)) {
        Game::GetInstance().dificulty = 3;
        Game::GetInstance().Push(new InstructionsState());
        return;
    }
}

void DificultyState::Render() {
    RenderArray();
}

void DificultyState::Pause() {}
void DificultyState::Resume() {}