#include "Vehicle.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "State.h"
#include "Game.h"
#include "Timer.h"
#include "Camera.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Vehicle* Vehicle::player = nullptr;

Vehicle::Vehicle(GameObject& associated, std::string sprite, bool isPlayer, int frameCountW)
    : Component(associated),
      spritePath(sprite),
      spriteFrameCountW(frameCountW),
      currentSpeed(0.0f),
      maxSpeed(300.0f),
      maxReverseSpeed(120.0f),
      acceleration(200.0f),
      brakeForce(350.0f),
      friction(80.0f),
      turnSpeed(120.0f),
      angleDeg(0.0f),
      inputAccel(false), inputBrake(false),
      inputLeft(false), inputRight(false),
      hp(100), isPlayer(isPlayer) {

    SpriteRenderer* sr = new SpriteRenderer(associated, spritePath, spriteFrameCountW, 1);
    sr->SetFrame(0);
    associated.AddComponent(sr);

    if (Vehicle::player == nullptr && isPlayer) {
        Vehicle::player = this;
    }
}

Vehicle::~Vehicle() {}

void Vehicle::Start() {
}

void Vehicle::Update(float dt) {
    deathTimer.Update(dt);
    hitTimer.Update(dt);

    // Reseta inputs do frame
    inputAccel = inputBrake = inputLeft = inputRight = false;

    // Lê comandos acumulados neste frame
    while (!taskQueue.empty()) {
        Command cmd = taskQueue.front();
        taskQueue.pop();
        switch (cmd.type) {
            case Command::ACCELERATE:  inputAccel  = true; break;
            case Command::BRAKE:       inputBrake  = true; break;
            case Command::TURN_LEFT:   inputLeft   = true; break;
            case Command::TURN_RIGHT:  inputRight  = true; break;
            default: break;
        }
    }

    // ── Aceleração / frenagem / fricção ──────────────────────────────────
    if (inputAccel) {
        currentSpeed += acceleration * dt;
    } else if (inputBrake) {
        // Freia se estiver andando para frente; caso contrário, engrena ré
        if (currentSpeed > 0.0f) {
            currentSpeed -= brakeForce * dt;
            if (currentSpeed < 0.0f) currentSpeed = 0.0f;
        } else {
            currentSpeed -= acceleration * dt; // acumula ré
        }
    } else {
        // Fricção natural
        if (currentSpeed > 0.0f) {
            currentSpeed -= friction * dt;
            if (currentSpeed < 0.0f) currentSpeed = 0.0f;
        } else if (currentSpeed < 0.0f) {
            currentSpeed += friction * dt;
            if (currentSpeed > 0.0f) currentSpeed = 0.0f;
        }
    }

    // Clamp velocidade
    if (currentSpeed > maxSpeed)         currentSpeed = maxSpeed;
    if (currentSpeed < -maxReverseSpeed) currentSpeed = -maxReverseSpeed;

    // ── Giro (só quando o carro está em movimento) ────────────────────────
    float speedRatio = std::abs(currentSpeed) / maxSpeed; // 0..1
    float effectiveTurn = turnSpeed * speedRatio * dt;

    if (std::abs(currentSpeed) > 5.0f) {
        // Invertido na ré: virar à esquerda em ré vai para o outro lado
        float dir = (currentSpeed >= 0.0f) ? 1.0f : -1.0f;
        if (inputLeft)  angleDeg -= effectiveTurn * dir;
        if (inputRight) angleDeg += effectiveTurn * dir;
    }

    // Mantém ângulo em [0, 360)
    while (angleDeg <   0.0f) angleDeg += 360.0f;
    while (angleDeg >= 360.0f) angleDeg -= 360.0f;

    // Espelha o ângulo do GameObject para o SpriteRenderer rotacionar corretamente
    associated.angleDeg = angleDeg;

    // ── Movimento com colisão separada por eixo ──────────────────────────
    float rad = angleDeg * (float)M_PI / 180.0f;
    Vec2 forward(std::sin(rad), -std::cos(rad));

    float dx = forward.x * currentSpeed * dt;
    float dy = forward.y * currentSpeed * dt;

    State& state = Game::GetInstance().GetState();

    // Testa eixo X
    Rect nextX = associated.box;
    nextX.x += dx;
    if (!state.IsCollidingWithTileMap(nextX)) {
        associated.box.x += dx;
    } else {
        currentSpeed *= -0.3f;
    }

    // Testa eixo Y
    Rect nextY = associated.box;
    nextY.y += dy;
    if (!state.IsCollidingWithTileMap(nextY)) {
        associated.box.y += dy;
    } else {
        currentSpeed *= -0.3f;
    }

    // setar posição da animação
    SpriteRenderer* sr = associated.GetComponent<SpriteRenderer>();
    if (sr) {
        float normalizedAngle = fmod(angleDeg, 360.0f);
        if (normalizedAngle < 0) {
            normalizedAngle += 360.0f;
        }

        sr -> renderRotation = angleDeg; // Para renderização rotacionada

        // Define quantas direções sua spritesheet possui
        const int numDirections = 8; 
        const float sliceSize = 360.0f / numDirections; // 45 graus

        // Cada Frame 0 cubra de 337.5° até 22.5°
        float shiftedAngle = normalizedAngle + (sliceSize / 2.0f);
        
        // Calcula o frame com divisão inteira e aplica o 'resto' para o caso de passar de 360
        int frame = static_cast<int>(shiftedAngle / sliceSize) % numDirections;

        // Mapeamento para a spritesheet
        int isometricFrames[8] = {5, 7, 4, 2, 0, 1, 3, 6};
        sr->SetFrame(isometricFrames[frame]);
    }
}

void Vehicle::Render() {}

void Vehicle::Issue(Command task) {
    taskQueue.push(task);
}

Vehicle::Command::Command(CommandType type, float x, float y)
    : type(type), pos(x, y) {}

void Vehicle::NotifyCollision(GameObject& other) {
    (void)other;
}

void Vehicle::Damage(int damage) {
    hp -= damage;
}