#include "GenerateCity.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

namespace {

// Tiles da camada 0 (visual/ruas) — ver TileMap::RenderLayer.
constexpr int kEmpty = -1;
constexpr int kRoadVertical = 4;
constexpr int kRoadHorizontal = 5;
constexpr int kRoadCross = 16;

// Tiles da camada 1 (colisão) — ver TileMap::IsTileBlocked. Qualquer valor
// acima de 10 bloqueia passagem; 11 marca prédio/borda do mundo e 12 marca um
// prédio "grande" (largura ou altura do quarteirão maior que 3 tiles), do
// mesmo jeito que foi feito manualmente em cidade2.txt.
constexpr int kBuildingSmall = 11;
constexpr int kBuildingBig = 12;

std::mt19937 MakeRng(unsigned seed) {
    if (seed != 0) return std::mt19937(seed);
    return std::mt19937(std::random_device{}());
}

void FillRect(std::vector<std::vector<int>>& layer, int x0, int y0, int x1, int y1, int value) {
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            layer[y][x] = value;
        }
    }
}

void WriteLayer(std::ofstream& out, const std::vector<std::vector<int>>& layer, bool isLastLayer) {
    int height = static_cast<int>(layer.size());
    for (int y = 0; y < height; ++y) {
        const std::vector<int>& row = layer[y];
        bool isLastRow = isLastLayer && (y == height - 1);
        for (size_t x = 0; x < row.size(); ++x) {
            out << row[x];
            bool isLastValue = isLastRow && (x == row.size() - 1);
            if (!isLastValue) out << ",";
        }
        out << "\n";
    }
}

}  // namespace

bool GenerateCity::Generate(const std::string& outputPath) {
    return Generate(outputPath, Params());
}

bool GenerateCity::Generate(const std::string& outputPath, const Params& params) {
    if (params.width < 3 || params.height < 3 ||
        params.blockWidth < 1 || params.blockHeight < 1 || params.streetWidth < 1) {
        std::cerr << "GenerateCity: parametros invalidos." << std::endl;
        return false;
    }

    const int width = params.width;
    const int height = params.height;

    std::vector<std::vector<int>> roads(height, std::vector<int>(width, kEmpty));
    std::vector<std::vector<int>> collision(height, std::vector<int>(width, kEmpty));

    // Moldura externa: limite do mundo, sempre bloqueada e nunca sorteada.
    FillRect(collision, 0, 0, width - 1, 0, kBuildingSmall);
    FillRect(collision, 0, height - 1, width - 1, height - 1, kBuildingSmall);
    FillRect(collision, 0, 0, 0, height - 1, kBuildingSmall);
    FillRect(collision, width - 1, 0, width - 1, height - 1, kBuildingSmall);

    const int interiorX0 = 1;
    const int interiorY0 = 1;
    const int interiorX1 = width - 2;
    const int interiorY1 = height - 2;

    const int periodX = params.blockWidth + params.streetWidth;
    const int periodY = params.blockHeight + params.streetWidth;

    // Posições dos quarteirões que cabem inteiros dentro da área interna.
    std::vector<int> blockStartsX;
    for (int x = interiorX0; x + params.blockWidth - 1 <= interiorX1; x += periodX) {
        blockStartsX.push_back(x);
    }
    std::vector<int> blockStartsY;
    for (int y = interiorY0; y + params.blockHeight - 1 <= interiorY1; y += periodY) {
        blockStartsY.push_back(y);
    }

    if (blockStartsX.empty() || blockStartsY.empty()) {
        std::cerr << "GenerateCity: mapa pequeno demais para os quarteiroes pedidos." << std::endl;
        return false;
    }

    // Rua no meio de cada vao entre quarteiroes (nunca cruza um quarteirao).
    std::vector<int> roadColumns;
    for (int bx : blockStartsX) {
        int gapStart = bx + params.blockWidth;
        int roadX = gapStart + params.streetWidth / 2;
        if (roadX <= interiorX1) roadColumns.push_back(roadX);
    }
    std::vector<int> roadRows;
    for (int by : blockStartsY) {
        int gapStart = by + params.blockHeight;
        int roadY = gapStart + params.streetWidth / 2;
        if (roadY <= interiorY1) roadRows.push_back(roadY);
    }

    for (int y = interiorY0; y <= interiorY1; ++y) {
        bool isRoadRow = std::find(roadRows.begin(), roadRows.end(), y) != roadRows.end();
        for (int x = interiorX0; x <= interiorX1; ++x) {
            bool isRoadCol = std::find(roadColumns.begin(), roadColumns.end(), x) != roadColumns.end();
            if (isRoadCol && isRoadRow) roads[y][x] = kRoadCross;
            else if (isRoadCol) roads[y][x] = kRoadVertical;
            else if (isRoadRow) roads[y][x] = kRoadHorizontal;
        }
    }

    // Sorteia cada quarteirao: vazio, predio pequeno (11, ate 3x3) ou
    // predio grande (12, ocupa o quarteirao inteiro).
    std::mt19937 rng = MakeRng(params.seed);
    std::uniform_real_distribution<double> chance(0.0, 1.0);

    const int smallW = std::min(params.blockWidth, 3);
    const int smallH = std::min(params.blockHeight, 3);
    const bool canBeBig = params.blockWidth > 3 || params.blockHeight > 3;

    for (int by : blockStartsY) {
        for (int bx : blockStartsX) {
            if (chance(rng) >= params.buildingChance) continue;  // lote vazio

            bool big = canBeBig && chance(rng) < 0.5;
            if (big) {
                FillRect(collision, bx, by, bx + params.blockWidth - 1, by + params.blockHeight - 1,
                          kBuildingBig);
            } else {
                // Predio pequeno centralizado dentro do lote reservado.
                int offsetX = (params.blockWidth - smallW) / 2;
                int offsetY = (params.blockHeight - smallH) / 2;
                int x0 = bx + offsetX;
                int y0 = by + offsetY;
                FillRect(collision, x0, y0, x0 + smallW - 1, y0 + smallH - 1, kBuildingSmall);
            }
        }
    }

    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "GenerateCity: nao foi possivel abrir '" << outputPath << "' para escrita."
                   << std::endl;
        return false;
    }

    out << width << "," << height << ",2,\n";
    out << "\n";
    WriteLayer(out, roads, false);
    out << "\n";
    WriteLayer(out, collision, true);

    return true;
}

#ifndef GENERATE_CITY_NO_MAIN
int main(int argc, char** argv) {
    GenerateCity::Params params;
    std::string outputPath = "recursos/map/cidade_criada.txt";

    if (argc > 1) outputPath = argv[1];
    if (argc > 2) params.width = std::atoi(argv[2]);
    if (argc > 3) params.height = std::atoi(argv[3]);
    if (argc > 4) params.seed = static_cast<unsigned>(std::atoi(argv[4]));

    if (!GenerateCity::Generate(outputPath, params)) {
        return 1;
    }

    std::cout << "Cidade gerada em " << outputPath << " (" << params.width << "x" << params.height
               << ")" << std::endl;
    return 0;
}
#endif
