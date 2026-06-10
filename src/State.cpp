#include "State.h"

State::State()
    : popRequested(false), quitRequested(false), started(false), objectArray() {}

State::~State() {
    objectArray.clear();
}

std::weak_ptr<GameObject> State::AddObject(GameObject* go) {
    std::shared_ptr<GameObject> sp(go);
    objectArray.push_back(sp);

    if (started) {
        sp->Start();
    }

    return std::weak_ptr<GameObject>(sp);
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject* go) {
    for (auto& obj : objectArray) {
        if (obj.get() == go) {
            return std::weak_ptr<GameObject>(obj);
        }
    }

    return std::weak_ptr<GameObject>();
}

bool State::PopRequested() const {
    return popRequested;
}

bool State::QuitRequested() const {
    return quitRequested;
}

void State::StartArray() {
    for (std::size_t i = 0; i < objectArray.size(); ++i) {
        objectArray[i]->Start();
    }
}

void State::UpdateArray(float dt) {
    for (std::size_t i = 0; i < objectArray.size(); ++i) {
        objectArray[i]->Update(dt);
    }

    for (int i = static_cast<int>(objectArray.size()) - 1; i >= 0; --i) {
        if (objectArray[i]->IsDead()) {
            objectArray.erase(objectArray.begin() + i);
        }
    }
}

void State::RenderArray() {
    for (const auto& go : objectArray) {
        go->Render();
    }
}


bool State::IsCollidingWithTileMap(const Rect& rect) {
    return false;
}
