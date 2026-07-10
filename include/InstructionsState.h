#pragma once

#include "State.h"
#include "Timer.h"

class InstructionsState : public State {
public:
    InstructionsState();
    ~InstructionsState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    Timer slideTimer;
    Timer blinkTimer;
    int currentSlide;
    bool showingPrompt;
    bool showText;
};
