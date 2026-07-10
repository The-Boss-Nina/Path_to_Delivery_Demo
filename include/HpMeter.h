#pragma once

#include "Component.h"

class Vehicle;

class HpMeter : public Component {
public:
    HpMeter(GameObject& associated, Vehicle* vehicle);

    void Update(float dt) override;
    void Render() override;

private:
    Vehicle* vehicle;
};
