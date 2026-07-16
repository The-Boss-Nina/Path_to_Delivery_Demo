#include "Game.h"
#include "InputManager.h"
#include "Resources.h"
#include "State.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <ctime>

Game* Game::instance = nullptr;

Game& Game::GetInstance(const char* title, int width, int height) {
    if (!instance) {
        instance = new Game(title, width, height);
    }

    return *instance;
}

Game::Game(const char* title, int width, int height)
    : dificulty(0),
      window(nullptr),
      renderer(nullptr),
      storedState(nullptr),
      stateStack(),
      frameStart(0),
      dt(0.0f) {

    if (instance != nullptr) {
        SDL_Log("Game ja foi criado!");
        exit(1);
    }

    instance = this;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Erro SDL_Init: %s", SDL_GetError());
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Erro IMG_Init: %s", IMG_GetError());
        exit(1);
    }

    if (TTF_Init() != 0) {
        SDL_Log("Erro TTF_Init: %s", TTF_GetError());
        exit(1);
    }

    if (!(Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) & (MIX_INIT_OGG | MIX_INIT_MP3))) {
        SDL_Log("Aviso Mix_Init: %s", Mix_GetError());
    }

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0) {
        SDL_Log("Erro Mix_OpenAudio: %s", Mix_GetError());
        exit(1);
    }

    Mix_AllocateChannels(32);

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_Log("Erro SDL_CreateWindow: %s", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        SDL_Log("Erro SDL_CreateRenderer: %s", SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, width, height);

    srand(static_cast<unsigned int>(time(nullptr)));

    frameStart = SDL_GetTicks();
    dt = 0.0f;
}

Game::~Game() {
    if (storedState) {
        delete storedState;
        storedState = nullptr;
    }

    while (!stateStack.empty()) {
        stateStack.pop();
    }

    Resources::ClearImages();
    Resources::ClearSounds();
    Resources::ClearMusics();
    Resources::ClearFonts();

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Renderer* Game::GetRenderer() const {
    return renderer;
}

State& Game::GetCurrentState() const {
    return *stateStack.top();
}

State& Game::GetState() const {
    return GetCurrentState();
}

void Game::Push(State* state) {
    if (storedState) {
        delete storedState;
    }

    storedState = state;
}

void Game::CalculateDeltaTime() {
    int currentFrame = SDL_GetTicks();
    dt = (currentFrame - frameStart) / 1000.0f;
    frameStart = currentFrame;
}

float Game::GetDeltaTime() const {
    return dt;
}

void Game::Run() {
    if (storedState) {
        stateStack.emplace(storedState);
        storedState = nullptr;
        stateStack.top()->Start();
    }

    while (!stateStack.empty() && !stateStack.top()->QuitRequested()) {
        if (stateStack.top()->PopRequested()) {
            stateStack.pop();

            Resources::ClearImages();
            Resources::ClearSounds();
            Resources::ClearMusics();
            Resources::ClearFonts();

            if (!stateStack.empty()) {
                stateStack.top()->Resume();
            }
        }

        if (storedState) {
            if (!stateStack.empty()) {
                stateStack.top()->Pause();
            }

            stateStack.emplace(storedState);
            storedState = nullptr;
            stateStack.top()->Start();
        }

        if (stateStack.empty()) {
            break;
        }

        CalculateDeltaTime();
        InputManager::GetInstance().Update();

        stateStack.top()->Update(GetDeltaTime());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        stateStack.top()->Render();
        SDL_RenderPresent(renderer);
    }

    while (!stateStack.empty()) {
        stateStack.pop();
    }

    Resources::ClearImages();
    Resources::ClearSounds();
    Resources::ClearMusics();
    Resources::ClearFonts();
}
