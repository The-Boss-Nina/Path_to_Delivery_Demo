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
constexpr int kSlideCount = 3;

// Botões inferiores. As áreas são grandes para facilitar o clique.
const SDL_Rect kBackButton = {70, 635, 240, 58};
const SDL_Rect kNextButton = {970, 635, 240, 58};

GameObject* CreateFullscreenImage(State& state, const std::string& file) {
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

GameObject* CreateButtonText(State& state,
                             const std::string& label,
                             float centerX,
                             float y) {
    GameObject* obj = new GameObject();
    SDL_Color white = {255, 255, 255, 255};
    Text* text = new Text(*obj,
                          "recursos/font/neodgm.ttf",
                          30,
                          Text::BLENDED,
                          label,
                          white);
    obj->AddComponent(text);
    obj->box.x = centerX - obj->box.w / 2.0f;
    obj->box.y = y;
    state.AddObject(obj);
    return obj;
}
}  // namespace

InstructionsState::InstructionsState()
    : State(), currentSlide(0) {}

InstructionsState::~InstructionsState() {}

void InstructionsState::LoadAssets() {
    // Ordem das telas: guia -> como jogar -> história.
    CreateFullscreenImage(*this, "recursos/img/guia_de_interface.jpeg");
    CreateFullscreenImage(*this, "recursos/img/como_jogar.jpeg");
    CreateFullscreenImage(*this, "recursos/img/historia.jpeg");

    // Textos dos botões. Os fundos são desenhados no Render().
    CreateButtonText(*this, "<  VOLTAR", 190.0f, 649.0f);   // índice 3
    CreateButtonText(*this, "PROXIMO  >", 1090.0f, 649.0f); // índice 4
}

void InstructionsState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

bool InstructionsState::IsMouseOverBack() const {
    const InputManager& input = InputManager::GetInstance();
    const int x = input.GetMouseX();
    const int y = input.GetMouseY();
    return x >= kBackButton.x && x <= kBackButton.x + kBackButton.w &&
           y >= kBackButton.y && y <= kBackButton.y + kBackButton.h;
}

bool InstructionsState::IsMouseOverNext() const {
    const InputManager& input = InputManager::GetInstance();
    const int x = input.GetMouseX();
    const int y = input.GetMouseY();
    return x >= kNextButton.x && x <= kNextButton.x + kNextButton.w &&
           y >= kNextButton.y && y <= kNextButton.y + kNextButton.h;
}

void InstructionsState::GoBack() {
    if (currentSlide > 0) {
        --currentSlide;
    } else {
        // Na primeira tela, VOLTAR retorna ao menu inicial.
        quitRequested = true;
    }
}

void InstructionsState::GoNext() {
    if (currentSlide < kSlideCount - 1) {
        ++currentSlide;
    } else {
        // Na última tela, PRÓXIMO abre a seleção de veículo.
        Game::GetInstance().Push(new VehicleSelectState());
    }
}

void InstructionsState::Update(float) {
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested()) {
        quitRequested = true;
        return;
    }

    // ESC também volta uma tela; na primeira, retorna ao menu.
    if (input.KeyPress(ESCAPE_KEY) ||
        input.KeyPress(LEFT_ARROW_KEY) ||
        input.KeyPress(SDLK_a)) {
        GoBack();
        return;
    }

    // Setas, A/D, ESPAÇO e ENTER funcionam junto com os botões visuais.
    if (input.KeyPress(RIGHT_ARROW_KEY) ||
        input.KeyPress(SDLK_d) ||
        input.KeyPress(SDLK_SPACE) ||
        input.KeyPress(SDLK_RETURN)) {
        GoNext();
        return;
    }

    if (input.MousePress(LEFT_MOUSE_BUTTON)) {
        if (IsMouseOverBack()) {
            GoBack();
        } else if (IsMouseOverNext()) {
            GoNext();
        }
    }
}

void InstructionsState::Render() {
    // Tela atual.
    objectArray[currentSlide]->Render();

    SDL_Renderer* renderer = Game::GetInstance().GetRenderer();

    // Fundo dos botões em preto com borda roxa, seguindo o estilo das telas.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
    SDL_RenderFillRect(renderer, &kBackButton);
    SDL_RenderFillRect(renderer, &kNextButton);

    const bool overBack = IsMouseOverBack();
    const bool overNext = IsMouseOverNext();

    SDL_SetRenderDrawColor(renderer,
                           overBack ? 255 : 111,
                           overBack ? 0 : 42,
                           overBack ? 220 : 167,
                           255);
    SDL_RenderDrawRect(renderer, &kBackButton);

    SDL_SetRenderDrawColor(renderer,
                           overNext ? 255 : 111,
                           overNext ? 0 : 42,
                           overNext ? 220 : 167,
                           255);
    SDL_RenderDrawRect(renderer, &kNextButton);

    // Textos dos botões.
    objectArray[3]->Render();
    objectArray[4]->Render();

    // Indicador da página: 1/3, 2/3 ou 3/3.
    const int dotY = 674;
    const int startX = 620;
    for (int i = 0; i < kSlideCount; ++i) {
        SDL_Rect dot = {startX + i * 20, dotY, 10, 10};
        if (i == currentSlide) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 240, 255);
            SDL_RenderFillRect(renderer, &dot);
        } else {
            SDL_SetRenderDrawColor(renderer, 111, 42, 167, 255);
            SDL_RenderDrawRect(renderer, &dot);
        }
    }
}

void InstructionsState::Pause() {}

void InstructionsState::Resume() {}
