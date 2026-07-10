#pragma once

#include "State.h"

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
    int currentSlide;

    bool IsMouseOverBack() const;
    bool IsMouseOverNext() const;
    void GoBack();
    void GoNext();
};

