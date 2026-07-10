#include "HpMeter.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Vehicle.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr int kFrameCount = 20;
constexpr float kMaxHp = 100.0f;
}

HpMeter::HpMeter(GameObject& associated, Vehicle* vehicle)
    : Component(associated), vehicle(vehicle) {}

void HpMeter::Update(float dt) {
    (void)dt;

    if (!vehicle) return;

    float ratio = static_cast<float>(vehicle->GetHp()) / kMaxHp;
    ratio = std::clamp(ratio, 0.0f, 1.0f);

    int frame = kFrameCount - static_cast<int>(std::round(ratio * (kFrameCount - 1)));

    SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
    if (sr) sr->SetFrame(frame);
}

void HpMeter::Render() {}
