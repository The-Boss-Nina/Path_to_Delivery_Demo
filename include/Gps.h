#pragma once

#include "Component.h"
#include "Delivery.h"
#include <vector>

class Gps : public Component {
public:
    Gps(GameObject& associated, Delivery* delivery);

    void Update(float dt) override;
    void Render() override;

private:
    int frameForRoute(const std::vector<Delivery::Position>& route) const;

    Delivery* delivery;
};
