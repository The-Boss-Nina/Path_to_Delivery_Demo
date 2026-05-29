#pragma once

#include "State.h"

class VehicleSelectState : public State {
public:
    VehicleSelectState();
    ~VehicleSelectState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;
};
