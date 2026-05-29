#include "Music.h"
#include "Resources.h"
#include <SDL.h>

Music::Music() : music(nullptr) {}

Music::Music(std::string file) : music(nullptr) {
    Open(file);
}

Music::~Music() {
    Stop();
}

void Music::Play(int times) {
    if (music) {
        Mix_PlayMusic(music, times);
    }
}

void Music::Stop(int msToStop) {
    Mix_FadeOutMusic(msToStop);
}

void Music::Open(std::string file) {
    music = Resources::GetMusic(file);
}

bool Music::IsOpen() const {
    return music != nullptr;
}
