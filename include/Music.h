#pragma once
#include <SDL_mixer.h>
#include <string>

class Music {
public:
    Music();
    Music(std::string file);
    ~Music();

    void Play(int times = -1);
    void Stop(int msToStop = 1500);
    void Open(std::string file);
    bool IsOpen() const;

private:
    Mix_Music* music;
};
