#include "TitleState.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Game.h"
#include "SDL.h"
#include "SpriteRenderer.h"
#include "InstructionsState.h"
#include "Text.h"
#include "Timer.h"
#include "Music.h"

#define BLINK_INTERVAL 1.0f

TitleState::TitleState() : showText(true), menuMusic() {
    GameObject* titleState = new GameObject();
    SpriteRenderer* spriteTitle = new SpriteRenderer(*titleState, "recursos/img/TelaInicial.png", 1, 1);
    spriteTitle->SetCameraFollower(true);
    titleState->AddComponent(spriteTitle);

    titleState->box.x = 0;
    titleState->box.y = 0;
    titleState->box.w = 1280;
    titleState->box.h = 720;

    AddObject(titleState);

    GameObject* textObj = new GameObject();
    SDL_Color white = { 255, 255, 255, 255 };
    Text* text = new Text(*textObj,
                          "recursos/font/neodgm.ttf",
                          40,
                          Text::BLENDED,
                          "Pressione ESPACO para continuar",
                          white);
    textObj->AddComponent(text);
    textObj->box.x = 640 - textObj->box.w / 2;
    textObj->box.y = 650;
    AddObject(textObj);
}

TitleState::~TitleState() {
}

void TitleState::Update(float dt) {
    if (InputManager::GetInstance().QuitRequested() || InputManager::GetInstance().KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
    }

    if(InputManager::GetInstance().KeyPress(SDLK_SPACE)) {
        Game::GetInstance().Push(new InstructionsState());
    }

    // Alterna visibilidade do texto a cada BLINK_INTERVAL segundos
    blinkTimer.Update(dt);
    if (blinkTimer.Get() >= BLINK_INTERVAL) {
        showText = !showText;
        blinkTimer.Restart();
    }

}

void TitleState::LoadAssets() {
}

void TitleState::Start() {
    if (started) return;

    StartArray();
    menuMusic.Open("recursos/sound/MENU.wav");
    menuMusic.Play(-1);
    started = true;
}

void TitleState::Render() {
    objectArray[0]->Render();

    // Se puder mostrar e tiver o objeto no array mostrar
    if (showText && objectArray.size() > 1) {
        objectArray[1]->Render();
    }

}

void TitleState::Pause() {
}

void TitleState::Resume() {
}