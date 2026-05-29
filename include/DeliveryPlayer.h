#pragma once

#include "Component.h"

enum class VehicleType {
    HARLEY,
    VESPA,
    BUGGY
};

class DeliveryPlayer : public Component {
public:
    DeliveryPlayer(GameObject& associated, VehicleType type = VehicleType::HARLEY);

    void Start() override;
    void Update(float dt) override;
    void Render() override;

private:
    VehicleType type;
};
