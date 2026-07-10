#ifndef TILESTATE_H
#define TILESTATE_H

#include "State.h"
#include "Timer.h"
#include "Music.h"

class TitleState : public State {
    public:
        TitleState();
        ~TitleState();
        void LoadAssets();
        void Update(float df);
        void Render();
        void Start();
        void Pause();
        void Resume();

    private:
        Timer blinkTimer;
        bool showText;
        Music menuMusic;
};

#endif