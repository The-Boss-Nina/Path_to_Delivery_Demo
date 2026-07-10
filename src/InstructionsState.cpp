#include "InstructionsState.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "VehicleSelectState.h"
#include <SDL.h>
#include <string>

namespace {
constexpr float kSlideDuration = 2.0f;
constexpr float kBlinkInterval = 1.0f;
constexpr int kSlideCount = 3;
}  // namespace

InstructionsState::InstructionsState()
    : State(), currentSlide(0), showingPrompt(false), showText(true) {}

InstructionsState::~InstructionsState() {}

static GameObject* CreateFullscreenImage(State& state, const std::string& file) {
    GameObject* object = new GameObject();
    SpriteRenderer* sr = new SpriteRenderer(*object, file, 1, 1);
    sr->SetCameraFollower(true);
    object->AddComponent(sr);
    object->box.x = 0;
    object->box.y = 0;
    object->box.w = 1280;
    object->box.h = 720;
    state.AddObject(object);
    return object;
}

void InstructionsState::LoadAssets() {
    // Slides na ordem em que devem aparecer (índices 0, 1, 2 do objectArray).
    CreateFullscreenImage(*this, "recursos/img/guia_de_interface.png");
    CreateFullscreenImage(*this, "recursos/img/como_jogar.png");
    CreateFullscreenImage(*this, "recursos/img/historia.png");

    // Texto piscante — mesmo padrão da TitleState (índice 3 do objectArray).
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

void InstructionsState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

void InstructionsState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    if (!showingPrompt) {
        slideTimer.Update(dt);
        if (slideTimer.Get() >= kSlideDuration) {
            slideTimer.Restart();
            currentSlide++;
            if (currentSlide >= kSlideCount) {
                showingPrompt = true;
            }
        }
        return;
    }

    // Alterna visibilidade do texto a cada kBlinkInterval segundos.
    blinkTimer.Update(dt);
    if (blinkTimer.Get() >= kBlinkInterval) {
        showText = !showText;
        blinkTimer.Restart();
    }

    if (input.KeyPress(SDLK_SPACE)) {
        Game::GetInstance().Push(new VehicleSelectState());
    }
}

void InstructionsState::Render() {
    if (!showingPrompt) {
        objectArray[currentSlide]->Render();
        return;
    }

    // Mantém o último slide (história) de fundo enquanto o texto pisca.
    objectArray[kSlideCount - 1]->Render();
    if (showText) {
        objectArray[kSlideCount]->Render();
    }
}

void InstructionsState::Pause() {}

void InstructionsState::Resume() {}
