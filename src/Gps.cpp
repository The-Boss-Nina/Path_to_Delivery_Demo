#include "Gps.h"
#include "GameObject.h"
#include "SpriteRenderer.h"

Gps::Gps(GameObject& associated, Delivery* delivery)
    : Component(associated), delivery(delivery) {}

int Gps::frameForRoute(const std::vector<Delivery::Position>& route) const {
    if (route.empty()) return 0;

    using P = Delivery::Position;

    switch (route[0]) {
        case P::EAST:  return 8;
        case P::WEST:  return 7;
        case P::NORTH:
            if (route.size() > 1) {
                if (route[1] == P::EAST) return 3;
                if (route[1] == P::WEST) return 5;
            }
            return 1;
        case P::SOUTH:
            if (route.size() > 1) {
                if (route[1] == P::EAST) return 4;
                if (route[1] == P::WEST) return 6;
            }
            return 2;
    }

    return 0;
}

void Gps::Update(float dt) {
    (void)dt;

    if (!delivery) return;

    auto route = delivery->GetRoute();
    int frame = frameForRoute(route);

    SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
    if (sr) sr->SetFrame(frame);
}

void Gps::Render() {}
