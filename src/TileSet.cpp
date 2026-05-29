#include "TileSet.h"
#include "Game.h"

TileSet::TileSet(int tileWidth, int tileHeight, std::string file)
    : tileSet(file), tileWidth(tileWidth), tileHeight(tileHeight), tileCount(0) {

    if (tileSet.IsOpen() && tileWidth > 0 && tileHeight > 0) {
        int columns = tileSet.GetWidth() / tileWidth;
        int rows = tileSet.GetHeight() / tileHeight;
        tileCount = columns * rows;
        tileSet.SetFrameCount(columns, rows);
    }
}

void TileSet::RenderTile(unsigned index, float x, float y) {
    if (index >= static_cast<unsigned>(tileCount)) return;

    tileSet.SetFrame(static_cast<int>(index));
    tileSet.Render(Game::GetInstance().GetRenderer(),
                   static_cast<int>(x),
                   static_cast<int>(y),
                   tileWidth,
                   tileHeight);
}

int TileSet::GetTileWidth() const { return tileWidth; }
int TileSet::GetTileHeight() const { return tileHeight; }
