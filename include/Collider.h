#ifndef COLLIDER_H
#define COLLIDER_H

#include "GameObject.h"
#include "Component.h"
#include "Rect.h"

class Collider : public Component {
    public:
        Collider(GameObject& associated, Vec2 scale = Vec2(1, 1), Vec2 offset = Vec2(0, 0));
        void Update(float dt);
        void Render();
        Rect box;
        void SetScale(Vec2 scale);
        void SetOffset(Vec2 offset);

    private:
        Vec2 scale;
        Vec2 offset;
};

#endif