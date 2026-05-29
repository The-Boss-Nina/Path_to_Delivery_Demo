#pragma once

#include <SDL.h>
#include <memory>
#include <stack>

class State;

class Game {
public:
    static Game& GetInstance(const char* title = "", int width = 0, int height = 0);
    ~Game();

    void Run();
    void Push(State* state);

    void CalculateDeltaTime();
    float GetDeltaTime() const;

    SDL_Renderer* GetRenderer() const;
    State& GetCurrentState() const;
    State& GetState() const;

private:
    Game(const char* title, int width, int height);
    static Game* instance;

    SDL_Window* window;
    SDL_Renderer* renderer;

    State* storedState;
    std::stack<std::unique_ptr<State>> stateStack;

    int frameStart;
    float dt;
};
