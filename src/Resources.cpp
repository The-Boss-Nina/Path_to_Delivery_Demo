#include "Resources.h"
#include "Game.h"
#include <SDL_image.h>

std::unordered_map<std::string, SDL_Texture*> Resources::imageTable;
std::unordered_map<std::string, Mix_Music*> Resources::musicTable;
std::unordered_map<std::string, Mix_Chunk*> Resources::soundTable;

SDL_Texture* Resources::GetImage(std::string file) {
    auto it = imageTable.find(file);
    if (it != imageTable.end()) {
        return it->second;
    }

    SDL_Texture* texture = IMG_LoadTexture(Game::GetInstance("", 0, 0).GetRenderer(), file.c_str());
    if (!texture) {
        SDL_Log("Erro ao carregar imagem (%s): %s", file.c_str(), IMG_GetError());
        return nullptr;
    }

    imageTable.emplace(file, texture);
    return texture;
}

void Resources::ClearImages() {
    for (auto& pair : imageTable) {
        SDL_DestroyTexture(pair.second);
    }
    imageTable.clear();
}

Mix_Music* Resources::GetMusic(std::string file) {
    auto it = musicTable.find(file);
    if (it != musicTable.end()) {
        return it->second;
    }

    Mix_Music* music = Mix_LoadMUS(file.c_str());
    if (!music) {
        SDL_Log("Erro ao carregar musica (%s): %s", file.c_str(), Mix_GetError());
        return nullptr;
    }

    musicTable.emplace(file, music);
    return music;
}

void Resources::ClearMusics() {
    for (auto& pair : musicTable) {
        Mix_FreeMusic(pair.second);
    }
    musicTable.clear();
}

Mix_Chunk* Resources::GetSound(std::string file) {
    auto it = soundTable.find(file);
    if (it != soundTable.end()) {
        return it->second;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(file.c_str());
    if (!chunk) {
        SDL_Log("Erro ao carregar som (%s): %s", file.c_str(), Mix_GetError());
        return nullptr;
    }

    soundTable.emplace(file, chunk);
    return chunk;
}

void Resources::ClearSounds() {
    for (auto& pair : soundTable) {
        Mix_FreeChunk(pair.second);
    }
    soundTable.clear();
}
