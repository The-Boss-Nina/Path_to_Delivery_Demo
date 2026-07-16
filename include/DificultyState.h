#pragma once

#include "DificultyState.h"
#include "State.h"

class DificultyState : public State {
public:
    DificultyState();
    ~DificultyState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

    int selectedDificulty;
};
