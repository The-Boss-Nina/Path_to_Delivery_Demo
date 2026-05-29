#include "Animator.h"
#include "GameObject.h"
#include "SpriteRenderer.h"

Animator::Animator(GameObject& associated)
    : Component(associated), current(""), frameStart(0), frameEnd(0), frameTime(0),
      currentFrame(0), timeElapsed(0), flip(SDL_FLIP_NONE) {}

void Animator::Update(float dt) {
    if (frameTime == 0) return;

    timeElapsed += dt;
    if (timeElapsed > frameTime) {
        timeElapsed -= frameTime;
        currentFrame++;
        if (currentFrame > frameEnd) currentFrame = frameStart;

        SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
        if (sr) sr->SetFrame(currentFrame, flip);
    }
}

void Animator::Render() {}

void Animator::SetAnimation(std::string name) {
    if (current == name) return;

    auto it = animations.find(name);
    if (it == animations.end()) return;

    current = name;
    frameStart = it->second.frameStart;
    frameEnd = it->second.frameEnd;
    frameTime = it->second.frameTime;
    flip = it->second.flip;
    currentFrame = frameStart;
    timeElapsed = 0;

    SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
    if (sr) sr->SetFrame(currentFrame, flip);
}

void Animator::AddAnimation(std::string name, Animation anim) {
    if (animations.find(name) == animations.end()) animations.emplace(name, anim);
}
