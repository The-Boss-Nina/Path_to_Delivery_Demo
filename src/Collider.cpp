#include "Collider.h"
#include "GameObject.h"
#include "Rect.h"
#include "Vec2.h"
#include <cmath>

Collider::Collider(GameObject& associated, Vec2 scale, Vec2 offset) : Component(associated), box(0, 0, 0, 0), scale(scale), offset(offset) {
}

void Collider::Update(float dt) {
    box.x = associated.box.x + offset.x * (associated.angleDeg / 180.0f * M_PI);
    box.y = associated.box.y + offset.y * (associated.angleDeg / 180.0f * M_PI);
    box.w = associated.box.w * scale.x;
    box.h = associated.box.h * scale.y;
}

void Collider::Render() {
    #ifdef DEBUG
	Vec2 center( box.Center() );
	SDL_Point points[5];

	Vec2 point = (Vec2(box.x, box.y) - center).Rotate( associated.angleDeg/(180/PI) )
					+ center - Camera::pos;
	points[0] = {(int)point.x, (int)point.y};
	points[4] = {(int)point.x, (int)point.y};
	
	point = (Vec2(box.x + box.w, box.y) - center).Rotate( associated.angleDeg/(180/PI) )
					+ center - Camera::pos;
	points[1] = {(int)point.x, (int)point.y};
	
	point = (Vec2(box.x + box.w, box.y + box.h) - center).Rotate( associated.angleDeg/(180/PI) )
					+ center - Camera::pos;
	points[2] = {(int)point.x, (int)point.y};
	
	point = (Vec2(box.x, box.y + box.h) - center).Rotate( associated.angleDeg/(180/PI) )
					+ center - Camera::pos;
	points[3] = {(int)point.x, (int)point.y};

	SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLines(Game::GetInstance().GetRenderer(), points, 5);
    #endif // DEBUG
}

void Collider::SetScale(Vec2 scale) {
    this->scale = scale;
}

void Collider::SetOffset(Vec2 offset) {
    this->offset = offset;
}