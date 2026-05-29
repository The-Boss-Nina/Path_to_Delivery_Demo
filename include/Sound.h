#pragma once

#include <SDL_mixer.h>
#include <string>

class Sound {
public:
    Sound();
    Sound(std::string file);
    ~Sound();

    void Play(int times = 1);
    void Stop();
    void Open(std::string file);
    bool IsOpen() const;

private:
    Mix_Chunk* chunk;
    int channel;
};
