#pragma once

#include "GameObject.h"
#include <memory>
#include <vector>

class State {
public:
    State();
    virtual ~State();

    virtual void LoadAssets() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void Start() = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual bool IsCollidingWithTileMap(const Rect& rect);

    virtual std::weak_ptr<GameObject> AddObject(GameObject* go);
    virtual std::weak_ptr<GameObject> GetObjectPtr(GameObject* go);

    bool PopRequested() const;
    bool QuitRequested() const;

protected:
    virtual void StartArray();
    virtual void UpdateArray(float dt);
    virtual void RenderArray();

    bool popRequested;
    bool quitRequested;
    bool started;
    std::vector<std::shared_ptr<GameObject>> objectArray;
};
