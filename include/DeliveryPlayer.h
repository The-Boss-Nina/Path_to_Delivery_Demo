#pragma once

#include "Component.h"
#include "Vec2.h"

// Veículos disponíveis para o jogador escolher antes de iniciar a fase.
enum class VehicleType {
    HARLEY,
    VESPA,
    BUGGY
};

class DeliveryPlayer : public Component {
public:
    DeliveryPlayer(GameObject& associated, VehicleType type = VehicleType::HARLEY);

    void Update(float dt) override;
    void Render() override;

private:
    VehicleType type;
    float speed;

    void UpdateSpriteDirection(const Vec2& movement);
};
