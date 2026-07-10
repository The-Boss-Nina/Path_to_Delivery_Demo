#include "Delivery.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "State.h"
#include "TileMap.h"
#include <algorithm>
#include <cstdio>
#include <queue>
#include <random>
#include <vector>

static std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

Delivery::Delivery(GameObject& associated, State& state, TileMap* tileMap, GameObject* player)
    : Component(associated), state(state), tileMap(tileMap), player(player),
      deliveryActive(false), lastPrintedDirection(NORTH), hasLastDirection(false) {

    pickupPos = generateRandomLocation();

    GameObject* icon = new GameObject();
    icon->AddComponent(new SpriteRenderer(*icon, "recursos/img/exclamacao_sheet.png", 4, 4));
    icon->box.x = pickupPos.x;
    icon->box.y = pickupPos.y;
    pickupIcon = state.AddObject(icon);
}

Vec2 Delivery::generateRandomLocation() {
    int mapW = tileMap->GetWidth();
    int mapH = tileMap->GetHeight();
    int tileW = tileMap->GetTileWidth();
    int tileH = tileMap->GetTileHeight();

    std::uniform_int_distribution<int> distX(0, mapW - 1);
    std::uniform_int_distribution<int> distY(0, mapH - 1);

    
    auto isRoadTile = [&](int tx, int ty) {
        int tile = tileMap->At(tx, ty, 0);
        return tile == 4 || tile == 5 || tile == 16;
    };

    for (int attempts = 0; attempts < 1000; ++attempts) {
        int tx = distX(getRng());
        int ty = distY(getRng());
        if (isRoadTile(tx, ty) && !tileMap->IsTileBlocked(tx, ty)) {
            return Vec2(
                static_cast<float>(tx * tileW + tileW / 2),
                static_cast<float>(ty * tileH + tileH / 2)
            );
        }
    }

    return Vec2(760.0f, 1180.0f);
}

void Delivery::startDelivery() {
    deliveryActive = true;
    hasLastDirection = false;

    if (auto icon = pickupIcon.lock()) {
        icon->RequestDelete();
    }

    destinationPos = generateRandomLocation();

    GameObject* arrow = new GameObject();
    arrow->AddComponent(new SpriteRenderer(*arrow, "recursos/img/seta_sheet.png", 4, 4));
    arrow->box.x = destinationPos.x;
    arrow->box.y = destinationPos.y;
    destinationIcon = state.AddObject(arrow);
}

void Delivery::finishDelivery() {
    deliveryActive = false;
    deliveryCount += 1;
    hasLastDirection = false;

    if (auto arrow = destinationIcon.lock()) {
        arrow->RequestDelete();
    }

    pickupPos = generateRandomLocation();

    GameObject* icon = new GameObject();
    icon->AddComponent(new SpriteRenderer(*icon, "recursos/img/exclamacao_sheet.png", 4, 4));
    icon->box.x = pickupPos.x;
    icon->box.y = pickupPos.y;
    pickupIcon = state.AddObject(icon);
}

bool Delivery::isDeliveryActive() const {
    return deliveryActive;
}

// Monta o grafo de intersecções (nós = tile 16, arestas = ruas entre eles)
Delivery::Graph Delivery::buildGraph() const {
    Graph g;
    int mapW = tileMap->GetWidth();
    int mapH = tileMap->GetHeight();

    for (int y = 0; y < mapH; y++)
        for (int x = 0; x < mapW; x++)
            if (tileMap->At(x, y, 0) == 16)
                g.nodes.push_back({x, y});

    int N = (int)g.nodes.size();
    g.adj.resize(N);

    auto nodeIdx = [&](int tx, int ty) -> int {
        for (int i = 0; i < N; i++)
            if (g.nodes[i].tx == tx && g.nodes[i].ty == ty) return i;
        return -1;
    };

    for (int i = 0; i < N; i++) {
        int tx = g.nodes[i].tx;
        int ty = g.nodes[i].ty;

        // Percorre horizontalmente (tile 5) até a próxima intersecção
        for (int x = tx + 1; x < mapW; x++) {
            int tile = tileMap->At(x, ty, 0);
            if (tile == 16) {
                int j = nodeIdx(x, ty);
                if (j >= 0) {
                    float cost = (float)(x - tx);
                    g.adj[i].push_back({j, cost});
                    g.adj[j].push_back({i, cost});
                }
                break;
            }
            if (tile != 5) break;
        }

        // Percorre verticalmente (tile 4) até a próxima intersecção
        for (int y = ty + 1; y < mapH; y++) {
            int tile = tileMap->At(tx, y, 0);
            if (tile == 16) {
                int j = nodeIdx(tx, y);
                if (j >= 0) {
                    float cost = (float)(y - ty);
                    g.adj[i].push_back({j, cost});
                    g.adj[j].push_back({i, cost});
                }
                break;
            }
            if (tile != 4) break;
        }
    }

    return g;
}

// Dijkstra: retorna sequência de índices de nós do src ao dst
std::vector<int> Delivery::runDijkstra(const Graph& g, int src, int dst) const {
    int N = (int)g.nodes.size();

    std::vector<float> dist(N, 1e9f);
    std::vector<int>   prev(N, -1);
    using PQEntry = std::pair<float, int>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<PQEntry>> pq;

    dist[src] = 0.0f;
    pq.push({0.0f, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (const auto& e : g.adj[u]) {
            float nd = d + e.cost;
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                prev[e.to] = u;
                pq.push({nd, e.to});
            }
        }
    }

    std::vector<int> path;
    for (int cur = dst; cur != -1; cur = prev[cur])
        path.push_back(cur);
    std::reverse(path.begin(), path.end());

    if (path.empty() || path[0] != src) return {};
    return path;
}

// Função principal: monta grafo, roda Dijkstra e retorna direções
std::vector<Delivery::Position> Delivery::GetRoute() {
    if (!player || !tileMap) return {};

    int tileW = tileMap->GetTileWidth();
    int tileH = tileMap->GetTileHeight();

    Graph g = buildGraph();
    int N = (int)g.nodes.size();
    if (N == 0) return {};

    // Encontra intersecção mais próxima de cada posição
    auto nearestNode = [&](int tx, int ty) -> int {
        int best = -1;
        float bestDist = 1e9f;
        for (int i = 0; i < N; i++) {
            float dx = (float)(g.nodes[i].tx - tx);
            float dy = (float)(g.nodes[i].ty - ty);
            float d = dx * dx + dy * dy;
            if (d < bestDist) { bestDist = d; best = i; }
        }
        return best;
    };

    Vec2 target = deliveryActive ? destinationPos : pickupPos;
    int src = nearestNode((int)((player->box.x + player->box.w / 2.0f) / tileW),
                          (int)((player->box.y + player->box.h / 2.0f) / tileH));
    int dst = nearestNode((int)(target.x / tileW),
                          (int)(target.y / tileH));

    if (src < 0 || dst < 0 || src == dst) return {};

    std::vector<int> path = runDijkstra(g, src, dst);
    if (path.empty()) return {};

    // Converte sequência de nós em direções
    std::vector<Position> route;
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        int ax = g.nodes[path[i]].tx,   ay = g.nodes[path[i]].ty;
        int bx = g.nodes[path[i+1]].tx, by = g.nodes[path[i+1]].ty;

        if      (bx > ax) route.push_back(EAST);
        else if (bx < ax) route.push_back(WEST);
        else if (by > ay) route.push_back(SOUTH);
        else              route.push_back(NORTH);
    }

    // Debug: imprime primeira direção quando muda
    // if (!route.empty() && (!hasLastDirection || lastPrintedDirection != route[0])) {
    //     const char* names[] = {"NORTH", "EAST", "WEST", "SOUTH"};
    //     printf("[Delivery] Next direction: %s  (intersection (%d,%d) -> (%d,%d))\n",
    //            names[route[0]],
    //            g.nodes[path[0]].tx, g.nodes[path[0]].ty,
    //            g.nodes[path[1]].tx, g.nodes[path[1]].ty);
    //     lastPrintedDirection = route[0];
    //     hasLastDirection = true;
    // }

    return route;
}

void Delivery::Update(float dt) {
    (void)dt;

    if (!player) return;

    Vec2 playerCenter = Vec2(
        player->box.x + player->box.w / 2.0f,
        player->box.y + player->box.h / 2.0f
    );

    if (!deliveryActive) {
        if (auto icon = pickupIcon.lock()) {
            Vec2 iconCenter = Vec2(
                icon->box.x + icon->box.w / 2.0f,
                icon->box.y + icon->box.h / 2.0f
            );
            if (playerCenter.Distance(iconCenter) < TRIGGER_RADIUS) {
                startDelivery();
            }
        }
    } else {
        if (auto arrow = destinationIcon.lock()) {
            Vec2 arrowCenter = Vec2(
                arrow->box.x + arrow->box.w / 2.0f,
                arrow->box.y + arrow->box.h / 2.0f
            );
            if (playerCenter.Distance(arrowCenter) < TRIGGER_RADIUS) {
                finishDelivery();
            }
        }
    }

}

void Delivery::Render() {}
