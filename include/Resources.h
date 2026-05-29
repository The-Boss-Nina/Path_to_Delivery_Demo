#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <unordered_map>

class Resources {
public:
    static SDL_Texture* GetImage(std::string file);
    static void ClearImages();

    static Mix_Music* GetMusic(std::string file);
    static void ClearMusics();

    static Mix_Chunk* GetSound(std::string file);
    static void ClearSounds();

private:
    static std::unordered_map<std::string, SDL_Texture*> imageTable;
    static std::unordered_map<std::string, Mix_Music*> musicTable;
    static std::unordered_map<std::string, Mix_Chunk*> soundTable;
};
