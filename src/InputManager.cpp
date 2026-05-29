#include "InputManager.h"

InputManager::InputManager()
    : quitRequested(false), updateCounter(0), mouseX(0), mouseY(0) {
    for (int i = 0; i < 6; ++i) {
        mouseState[i] = false;
        mouseUpdate[i] = 0;
    }
}

InputManager::~InputManager() {}

InputManager& InputManager::GetInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::Update() {
    ++updateCounter;
    quitRequested = false;

    SDL_GetMouseState(&mouseX, &mouseY);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested = true;
                break;

            case SDL_KEYDOWN:
                if (event.key.repeat == 0) {
                    int key = event.key.keysym.sym;
                    keyState[key] = true;
                    keyUpdate[key] = updateCounter;
                }
                break;

            case SDL_KEYUP: {
                int key = event.key.keysym.sym;
                keyState[key] = false;
                keyUpdate[key] = updateCounter;
                break;
            }

            case SDL_MOUSEBUTTONDOWN: {
                int button = event.button.button;
                if (button >= 0 && button < 6) {
                    mouseState[button] = true;
                    mouseUpdate[button] = updateCounter;
                }
                break;
            }

            case SDL_MOUSEBUTTONUP: {
                int button = event.button.button;
                if (button >= 0 && button < 6) {
                    mouseState[button] = false;
                    mouseUpdate[button] = updateCounter;
                }
                break;
            }

            default:
                break;
        }
    }
}

bool InputManager::KeyPress(int key) {
    return keyState[key] && keyUpdate[key] == updateCounter;
}

bool InputManager::KeyRelease(int key) {
    return !keyState[key] && keyUpdate[key] == updateCounter;
}

bool InputManager::IsKeyDown(int key) {
    return keyState[key];
}

bool InputManager::MousePress(int button) {
    if (button < 0 || button >= 6) return false;
    return mouseState[button] && mouseUpdate[button] == updateCounter;
}

bool InputManager::MouseRelease(int button) {
    if (button < 0 || button >= 6) return false;
    return !mouseState[button] && mouseUpdate[button] == updateCounter;
}

bool InputManager::IsMouseDown(int button) {
    if (button < 0 || button >= 6) return false;
    return mouseState[button];
}

int InputManager::GetMouseX() const { return mouseX; }
int InputManager::GetMouseY() const { return mouseY; }
bool InputManager::QuitRequested() const { return quitRequested; }

