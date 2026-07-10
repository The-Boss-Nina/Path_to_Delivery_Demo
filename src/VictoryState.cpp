#include "VictoryState.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "TitleState.h"
#include <SDL.h>

VictoryState::VictoryState() : State() {}

VictoryState::~VictoryState() {}

void VictoryState::LoadAssets() {
    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/Background.png");
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);

    // recursos/img/vitoria.png é 768x768 — encolhe para caber na altura da
    // tela (720) preservando a proporção, e centraliza horizontalmente.
    GameObject* imageObj = new GameObject();
    SpriteRenderer* imageSr = new SpriteRenderer(*imageObj, "recursos/img/vitoria.png", 1, 1);
    imageSr->SetCameraFollower(true);
    imageObj->AddComponent(imageSr);
    imageObj->box.w = 720.0f;
    imageObj->box.h = 720.0f;
    imageObj->box.x = (1280.0f - imageObj->box.w) / 2.0f;
    imageObj->box.y = 0.0f;
    AddObject(imageObj);

    GameObject* textObj = new GameObject();
    SDL_Color white = { 255, 255, 255, 255 };
    Text* text = new Text(*textObj,
                          "recursos/font/neodgm.ttf",
                          32,
                          Text::BLENDED,
                          "Aperte espaço para reiniciar",
                          white);
    textObj->AddComponent(text);
    textObj->box.x = 640.0f - textObj->box.w / 2.0f;
    textObj->box.y = 650.0f;
    AddObject(textObj);
}

void VictoryState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

void VictoryState::Update(float dt) {
    (void)dt;

    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    if (input.KeyPress(SDLK_SPACE)) {
        Game::GetInstance().Push(new TitleState());
    }
}

void VictoryState::Render() {
    RenderArray();
}

void VictoryState::Pause() {}

void VictoryState::Resume() {}
