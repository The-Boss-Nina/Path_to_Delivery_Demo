#pragma once

#include "DeliveryPlayer.h"
#include "Music.h"
#include "State.h"

class TileSet;

class StageState : public State {
public:
    StageState(VehicleType selectedVehicle = VehicleType::HARLEY);
    ~StageState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

private:
    VehicleType selectedVehicle;
    TileSet* tileSet;
    Music backgroundMusic;
};
