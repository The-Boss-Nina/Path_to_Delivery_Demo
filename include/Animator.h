#pragma once
#include "Component.h"
#include "Animation.h"
#include <string>
#include <unordered_map>

class Animator : public Component {
public:
    Animator(GameObject& associated);

    void Update(float dt) override;
    void Render() override;
    void SetAnimation(std::string name);
    void AddAnimation(std::string name, Animation anim);

private:
    std::unordered_map<std::string, Animation> animations;
    std::string current;
    int frameStart;
    int frameEnd;
    float frameTime;
    int currentFrame;
    float timeElapsed;
    SDL_RendererFlip flip;
};
