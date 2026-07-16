#pragma once

#include "DeliveryPlayer.h"
#include "Music.h"
#include "State.h"
#include "TileMap.h"

class TileSet;
class Delivery;
class Text;

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
    bool IsCollidingWithTileMap(const Rect& rect) override;

private:
    VehicleType selectedVehicle;
    TileSet* tileSet;
    Music backgroundMusic;
    TileMap* tileMap;

    Delivery* delivery;
    Text* timeText;
    Text* goalText;
    Text* deliveryCountText;
    Text* endBannerText;
    GameObject* endBannerObj;

    float timeRemaining;
    bool gameEnded;
    int lastDisplayedSeconds;
    int lastDisplayedCount;
    int deliveryGoal;
};
