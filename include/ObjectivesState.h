#pragma once

#include "DeliveryPlayer.h"
#include "State.h"
#include "Timer.h"

class ObjectivesState : public State {
public:
    explicit ObjectivesState(VehicleType selectedVehicle);
    ~ObjectivesState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    VehicleType selectedVehicle;
    Timer frameTimer;
    int currentFrame;
};
