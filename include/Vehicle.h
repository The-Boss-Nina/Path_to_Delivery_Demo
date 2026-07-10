#ifndef VEHICLE_H
#define VEHICLE_H
 
#include "Component.h"
#include "GameObject.h"
#include "Vec2.h"
#include <queue>
#include <memory>
#include <string>
#include "Timer.h"
#include "Sound.h"
 
class Vehicle : public Component {
    public:
        class Command {
            public:
                enum CommandType {ACCELERATE, BRAKE, TURN_LEFT, TURN_RIGHT, SHOOT, IDLE};
                Command(CommandType type, float x = 0, float y = 0);
                CommandType type;
                Vec2 pos;
        };
 
        Vehicle(GameObject& associated, std::string sprite, bool isPlayer, int frameCountW = 9, int frameCountH = 1);
        ~Vehicle();
        void Start() override;
        void Update(float dt) override;
        void Render() override;
        void Issue(Command task);
        void NotifyCollision(GameObject& other) override;
        void Damage(int damage);
        GameObject& GetObject() { return associated; }
 
        static Vehicle* player;
        void SetMaxSpeed(float v)      { maxSpeed = v; }
        void SetAcceleration(float v)  { acceleration = v; }
        void SetTurnSpeed(float v)     { turnSpeed = v; }
        float GetSpeed() const         { return currentSpeed; }
        float GetMaxSpeed() const      { return maxSpeed; }
        int GetHp() const              { return hp; }
        Sound engineSound;
        Sound crashSound;
        Sound brakeSound;
 
    private:
        // Sprite
        std::string spritePath;
        int spriteFrameCountW;
        int spriteFrameCountH;
 
        // Física do carro
        float currentSpeed;      // velocidade escalar atual (px/s)
        float maxSpeed;          // velocidade máxima para frente
        float maxReverseSpeed;   // velocidade máxima para ré
        float acceleration;      // taxa de aceleração (px/s²)
        float brakeForce;        // taxa de frenagem (px/s²)
        float friction;          // desaceleração natural (px/s²)
        float turnSpeed;         // graus por segundo de giro
        float angleDeg;          // ângulo atual do carro em graus
 
        std::queue<Command> taskQueue;
 
        // Comandos recebidos neste frame
        bool inputAccel;
        bool inputBrake;
        bool inputLeft;
        bool inputRight;
 
        int hp;
        bool isPlayer;
        Timer deathTimer;
        Timer hitTimer;
};
 
#endif
