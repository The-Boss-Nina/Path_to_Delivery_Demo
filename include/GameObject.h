#pragma once
#include "Rect.h"
#include "Component.h"
#include <vector>
#include <algorithm>

class GameObject {
public:
    GameObject();
    ~GameObject();

    void Start();
    void Update(float dt);
    void Render();
    bool IsDead() const;
    void RequestDelete();
    void AddComponent(Component* cpt);
    void RemoveComponent(Component* cpt);
    void NotifyCollision(GameObject& other);

    template <class T>
    T* GetComponent() {
        for (Component* cpt : components) {
            T* casted = dynamic_cast<T*>(cpt);
            if (casted != nullptr) return casted;
        }
        return nullptr;
    }

    Rect box;
    double angleDeg;
    bool started;

private:
    std::vector<Component*> components;
    bool isDead;
};
