#include "GameObject.h"

GameObject::GameObject() : box(), angleDeg(0.0), started(false), isDead(false) {}

GameObject::~GameObject() {
    for (int i = static_cast<int>(components.size()) - 1; i >= 0; --i) {
        delete components[i];
    }
    components.clear();
}

void GameObject::Start() {
    if (started) return;
    for (std::size_t i = 0; i < components.size(); ++i) {
        components[i]->Start();
    }
    started = true;
}

void GameObject::Update(float dt) {
    for (std::size_t i = 0; i < components.size(); ++i) {
        components[i]->Update(dt);
    }
}

void GameObject::Render() {
    for (Component* cpt : components) cpt->Render();
}

bool GameObject::IsDead() const { return isDead; }
void GameObject::RequestDelete() { isDead = true; }

void GameObject::AddComponent(Component* cpt) {
    components.emplace_back(cpt);
    if (started) cpt->Start();
}

void GameObject::RemoveComponent(Component* cpt) {
    auto it = std::find(components.begin(), components.end(), cpt);
    if (it != components.end()) {
        delete *it;
        components.erase(it);
    }
}

void GameObject::NotifyCollision(GameObject& other) {
    std::vector<Component*> currentComponents = components;
    for (Component* cpt : currentComponents) {
        if (std::find(components.begin(), components.end(), cpt) != components.end()) {
            cpt->NotifyCollision(other);
        }
    }
}
