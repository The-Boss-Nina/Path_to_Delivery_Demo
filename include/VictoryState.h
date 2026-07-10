#pragma once

#include "State.h"

class VictoryState : public State {
public:
    VictoryState();
    ~VictoryState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;
};
