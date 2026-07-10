#pragma once

#include <string>

// Ferramenta que gera um mapa de cidade aleatório no mesmo formato usado por
// recursos/map/cidade2.txt: um cabeçalho "largura,altura,profundidade," seguido
// por duas camadas de tiles separadas por uma linha em branco.
//
// Camada 0 (visual/ruas): -1 vazio, 4 rua vertical, 5 rua horizontal, 16 cruzamento.
// Camada 1 (colisão, ver TileMap::IsTileBlocked): -1 livre, 11 prédio (ou borda de
// um prédio grande), 12 interior de um prédio grande (largura e altura > 3 tiles).
class GenerateCity {
public:
    struct Params {
        int width = 50;             // tiles
        int height = 40;            // tiles
        int blockWidth = 5;         // largura de cada quarteirão, em tiles
        int blockHeight = 3;        // altura de cada quarteirão, em tiles
        int streetWidth = 3;        // largura da rua entre quarteirões, em tiles
        double buildingChance = 0.85; // chance de um quarteirão virar prédio
        unsigned seed = 0;          // 0 = usa uma semente aleatória
    };

    // Gera a cidade e grava o resultado em outputPath. Retorna false se os
    // parâmetros forem inválidos ou o arquivo não puder ser aberto.
    static bool Generate(const std::string& outputPath);
    static bool Generate(const std::string& outputPath, const Params& params);
};
