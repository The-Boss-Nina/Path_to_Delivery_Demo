#pragma once

class Timer {
public:
    Timer();

    void Update(float dt);
    void Restart();
    float Get() const;

private:
    float time;
};
