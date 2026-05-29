#include "TileMap.h"
#include "GameObject.h"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <string>

TileMap::TileMap(GameObject& associated, std::string file, TileSet* tileSet)
    : Component(associated), tileSet(nullptr), mapWidth(0), mapHeight(0), mapDepth(0) {
    Load(file);
    SetTileSet(tileSet);
}

void TileMap::Load(std::string file) {
    std::ifstream mapFile(file);
    if (!mapFile.is_open()) {
        SDL_Log("Erro ao abrir mapa: %s", file.c_str());
        return;
    }

    std::string content((std::istreambuf_iterator<char>(mapFile)), std::istreambuf_iterator<char>());
    for (char& c : content) {
        if (c == ',') c = ' ';
    }

    std::stringstream ss(content);
    ss >> mapWidth >> mapHeight >> mapDepth;

    tileMatrix.clear();
    tileMatrix.reserve(mapWidth * mapHeight * mapDepth);

    int tile = -1;
    while (ss >> tile) {
        tileMatrix.emplace_back(tile);
    }

    int expectedSize = mapWidth * mapHeight * mapDepth;
    while (static_cast<int>(tileMatrix.size()) < expectedSize) {
        tileMatrix.emplace_back(-1);
    }
}

void TileMap::SetTileSet(TileSet* tileSet) {
    this->tileSet.reset(tileSet);
}

int& TileMap::At(int x, int y, int z) {
    int index = x + y * mapWidth + z * mapWidth * mapHeight;
    return tileMatrix[index];
}

void TileMap::Update(float dt) {
    (void)dt;
}

void TileMap::Render() {
    for (int layer = 0; layer < mapDepth; ++layer) {
        RenderLayer(layer);
    }
}

void TileMap::RenderLayer(int layer) {
    if (!tileSet) return;

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            int tileIndex = At(x, y, layer);
            if (tileIndex >= 0) {
                float posX = associated.box.x + x * tileSet->GetTileWidth();
                float posY = associated.box.y + y * tileSet->GetTileHeight();
                tileSet->RenderTile(static_cast<unsigned>(tileIndex), posX, posY);
            }
        }
    }
}

int TileMap::GetWidth() const { return mapWidth; }
int TileMap::GetHeight() const { return mapHeight; }
int TileMap::GetDepth() const { return mapDepth; }
