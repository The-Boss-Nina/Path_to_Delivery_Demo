#pragma once

#include "Component.h"
#include "Vec2.h"
#include <memory>
#include <vector>

class State;
class TileMap;
class GameObject;

class Delivery : public Component {
public:
    enum Position { NORTH, EAST, WEST, SOUTH };

    Delivery(GameObject& associated, State& state, TileMap* tileMap, GameObject* player);

    void Update(float dt) override;
    void Render() override;

    void startDelivery();
    void finishDelivery();
    bool isDeliveryActive() const;

    std::vector<Position> GetRoute();

    int deliveryCount = 0;

private:
    struct Node { int tx, ty; };
    struct Edge { int to; float cost; };
    struct Graph {
        std::vector<Node> nodes;
        std::vector<std::vector<Edge>> adj;
    };

    Vec2 generateRandomLocation();
    Graph buildGraph() const;
    std::vector<int> runDijkstra(const Graph& g, int src, int dst) const;

    State& state;
    TileMap* tileMap;
    GameObject* player;

    std::weak_ptr<GameObject> pickupIcon;
    std::weak_ptr<GameObject> destinationIcon;

    Vec2 pickupPos;
    Vec2 destinationPos;

    bool deliveryActive;
    Position lastPrintedDirection;
    bool hasLastDirection = false;

    static constexpr float TRIGGER_RADIUS = 150.0f;
};
