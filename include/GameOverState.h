#pragma once

#include "State.h"

class GameOverState : public State {
public:
    GameOverState();
    ~GameOverState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;
};
